/**
	Copyright 2014-2018 Phantom Intelligence Inc.

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#ifndef SPIRITSENSORGATEWAY_DATAPROCESSINGSCHEDULER_HPP
#define SPIRITSENSORGATEWAY_DATAPROCESSINGSCHEDULER_HPP

#include "DataSink.hpp"

namespace DataFlow {

    template<class T, class SINK, uint8_t const NUMBER_OF_CONCURENT_INPUTS>
    class DataProcessingScheduler : public ConsumerLink<T> {

        typedef T DATA;
        typedef RingBuffer<DATA> InputBuffer;
        typedef Container::ConstantSizedPointerList<InputBuffer, NUMBER_OF_CONCURENT_INPUTS> InputBufferStates;

    public:

        /**
         * @brief Default constructor. A  should be only instantiated once and live the same amount of time it's controlling structure does.
         */
        DataProcessingScheduler(SINK* dataSink) :
                terminateOrderReceived(false),
                numberOfLinkedBuffers(0),
                dataSink(dataSink),
                schedulerThread(JoinableThread(voidAction)) {

            schedulerThread.safeExit();
            schedulerThread = JoinableThread(&DataProcessingScheduler::start, this);
        }

        /**
         * @brief The DataProcessingSchedulers are intended to be used as const instances. They shouldn't be moved.
         */
        DataProcessingScheduler(DataProcessingScheduler&& other) = delete;

        ~DataProcessingScheduler() noexcept = default;

        /**
         * @brief The DataProcessingSchedulers are intended to be used as const instances. They shouldn't be assigned.
         */
        DataProcessingScheduler(DataProcessingScheduler const& other) = delete;

        /**
         * @brief The DataProcessingSchedulers are intended to be used as const instances. They shouldn't be assigned.
         */
        DataProcessingScheduler& operator=(const DataProcessingScheduler& other) = delete;

        /**
         * @brief The DataProcessingSchedulers are intended to be used as const instances. They shouldn't be copied.
         */
        DataProcessingScheduler& operator=(DataProcessingScheduler&& other) = delete;

        void linkWith(InputBuffer* buffer) override {
            LockGuard guard(inputBufferStatusMutex);
            validateStillAcceptsInputBuffers();
            validateBufferIsNotLinked(buffer);
            validateStatusArrayNotFull();
            ++numberOfLinkedBuffers;
            catchedUpInputBuffers.store(buffer);
            buffer->linkWith(this);
        }

        void activateFor(InputBuffer* buffer) override {
            LockGuard guard(inputBufferStatusMutex);
            validateBufferIsLinked(buffer, ExceptionMessage::DATA_PROCESSING_SCHEDULER_ILLEGAL_ACTIVATION_MESSAGE);
            if (catchedUpInputBuffers.contains(buffer)) {
                catchedUpInputBuffers.remove(buffer);
                readyToConsumeInputBuffers.store(buffer);
            }
        }

        void deactivateFor(InputBuffer* buffer) override {
            LockGuard guard(inputBufferStatusMutex);
            validateBufferIsLinked(buffer, ExceptionMessage::DATA_PROCESSING_SCHEDULER_ILLEGAL_DEACTIVATION_MESSAGE);
            if (readyToConsumeInputBuffers.contains(buffer)) {
                readyToConsumeInputBuffers.remove(buffer);
                catchedUpInputBuffers.store(buffer);
            }
        }


        void terminateAndJoin() {
            if (!terminateOrderHasBeenReceived()) {
                terminateOrderReceived.store(true);
            }
            schedulerThread.safeExit();
        }

    private:

        void start() {
            while (cannotExitSafely()) {
                if (!readyToConsumeInputBuffers.empty()) {
                    auto inputBufferToConsumeFrom = readyToConsumeInputBuffers.readNextPointerToConsume();
                    auto data = inputBufferToConsumeFrom->consumeNextDataFor(this);
                    dataSink->consume(std::move(data));
                }
            }
        }

        bool cannotExitSafely() const noexcept {
            return !(readyToConsumeInputBuffers.empty() && terminateOrderHasBeenReceived());
        }

        bool terminateOrderHasBeenReceived() const {
            return terminateOrderReceived.load();
        }

        void validateStillAcceptsInputBuffers() const {
            if (terminateOrderHasBeenReceived()) {
                throwIllegalActionException(
                        ExceptionMessage::DATA_PROCESSING_SCHEDULER_ILLEGAL_LINKING_SCHEDULER_HAS_BEEN_STOPPED);
            }
        }

        bool isBufferLinked(InputBuffer* buffer) const noexcept {
            return catchedUpInputBuffers.contains(buffer) || readyToConsumeInputBuffers.contains(buffer);
        }

        void validateBufferIsNotLinked(InputBuffer* buffer) const {
            if (isBufferLinked(buffer)) {
                throwIllegalActionException(
                        ExceptionMessage::DATA_PROCESSING_SCHEDULER_ILLEGAL_LINKING_OF_ALREADY_LINKED_BUFFER_MESSAGE);
            }
        }

        void validateStatusArrayNotFull() const {
            if (numberOfLinkedBuffers.load() == NUMBER_OF_CONCURENT_INPUTS) {
                throwIllegalActionException(ExceptionMessage::DATA_PROCESSING_SCHEDULER_ILLEGAL_NUMBER_OF_INPUT_BUFFER_MESSAGE);
            }
        }

        void validateBufferIsLinked(InputBuffer* buffer, char const* message) const {
            if (!isBufferLinked(buffer)) {
                throwIllegalActionException(message);
            }
        }

        JoinableThread schedulerThread;

        AtomicFlag terminateOrderReceived;
        Mutex inputBufferStatusMutex;
        AtomicCounter numberOfLinkedBuffers;
        InputBufferStates readyToConsumeInputBuffers;
        InputBufferStates catchedUpInputBuffers;

        SINK* dataSink;
    };
}

#endif //SPIRITSENSORGATEWAY_DATAPROCESSINGSCHEDULER_HPP
