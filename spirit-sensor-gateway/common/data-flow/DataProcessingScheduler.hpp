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

    template<class T, class SINK, size_t const NUMBER_OF_CONCURRENT_INPUTS>
    class DataProcessingScheduler : public ConsumerLink<T> {

        typedef RingBuffer<T> InputBuffer;
        typedef Container::ConstantSizedPointerList<InputBuffer, NUMBER_OF_CONCURRENT_INPUTS> InputBuffers;

    public:

        DataProcessingScheduler(SINK* dataSink) :
                terminateOrderReceived(false),
                numberOfLinkedBuffers(0),
                dataSink(dataSink),
                schedulerThread(JoinableThread(doNothing)) {
            schedulerThread.exitSafely();
            schedulerThread = JoinableThread(&DataProcessingScheduler::start, this);
        }

        ~DataProcessingScheduler() noexcept = default;

        /**
         * @warning The DataProcessingSchedulers are intended to be used as const instances. They shouldn't be moved.
         */
        DataProcessingScheduler(DataProcessingScheduler&& other) = delete;

        /**
         * @warning The DataProcessingSchedulers are intended to be used as const instances. They shouldn't be assigned.
         */
        DataProcessingScheduler(DataProcessingScheduler const& other) = delete;

        /**
         * @warning The DataProcessingSchedulers are intended to be used as const instances. They shouldn't be assigned.
         */
        DataProcessingScheduler& operator=(const DataProcessingScheduler& other) = delete;

        /**
         * @warning The DataProcessingSchedulers are intended to be used as const instances. They shouldn't be copied.
         */
        DataProcessingScheduler& operator=(DataProcessingScheduler&& other) = delete;

        void linkWith(InputBuffer* inputBuffer) override {
            LockGuard guard(inputBufferStatusMutex);
            throwExceptionIfInputBufferIsLinked(inputBuffer);
            throwExceptionIfSchedulerHasReceivedTerminationOrder();
            throwExceptionIfMaximumOfLinkedBufferReached();
            ++numberOfLinkedBuffers;
            notReadyToConsumeInputBuffers.store(inputBuffer);
            inputBuffer->linkWith(this);
        }

        void activateFor(InputBuffer* inputBuffer) override {
            LockGuard guard(inputBufferStatusMutex);
            throwExceptionIfInputBufferIsNotLinked(inputBuffer,
                                                   ExceptionMessage::DATA_PROCESSING_SCHEDULER_ILLEGAL_ACTIVATION_MESSAGE);
            if (notReadyToConsumeInputBuffers.contains(inputBuffer)) {
                notReadyToConsumeInputBuffers.remove(inputBuffer);
                readyToConsumeInputBuffers.store(inputBuffer);
            }
        }

        void deactivateFor(InputBuffer* inputBuffer) override {
            LockGuard guard(inputBufferStatusMutex);
            throwExceptionIfInputBufferIsNotLinked(inputBuffer,
                                                   ExceptionMessage::DATA_PROCESSING_SCHEDULER_ILLEGAL_DEACTIVATION_MESSAGE);
            if (readyToConsumeInputBuffers.contains(inputBuffer)) {
                readyToConsumeInputBuffers.remove(inputBuffer);
                notReadyToConsumeInputBuffers.store(inputBuffer);
            }
        }

        void terminateAndJoin() {
            if (!terminateOrderHasBeenReceived()) {
                terminateOrderReceived.store(true);
            }
            schedulerThread.exitSafely();
        }

    private:

        void start() {
            while (cannotExitSafely()) {
                if (!readyToConsumeInputBuffers.isEmpty()) {
                    auto inputBufferToConsumeFrom = readyToConsumeInputBuffers.readNextPointerToConsume();
                    auto data = inputBufferToConsumeFrom->consumeNextDataFor(this);
                    dataSink->consume(std::move(data));
                }
            }
        }

        bool cannotExitSafely() const noexcept {
            return !(readyToConsumeInputBuffers.isEmpty() && terminateOrderHasBeenReceived());
        }

        bool terminateOrderHasBeenReceived() const {
            return terminateOrderReceived.load();
        }

        void throwExceptionIfSchedulerHasReceivedTerminationOrder() const {
            if (terminateOrderHasBeenReceived()) {
                throwIllegalActionException(
                        ExceptionMessage::DATA_PROCESSING_SCHEDULER_ILLEGAL_LINKING_SCHEDULER_HAS_BEEN_STOPPED);
            }
        }

        bool isInputBufferLinked(InputBuffer* inputBuffer) const noexcept {
            return notReadyToConsumeInputBuffers.contains(inputBuffer) ||
                   readyToConsumeInputBuffers.contains(inputBuffer);
        }


        void throwExceptionIfInputBufferIsLinked(InputBuffer* inputBuffer) const {
            if (isInputBufferLinked(inputBuffer)) {
                throwIllegalActionException(
                        ExceptionMessage::DATA_PROCESSING_SCHEDULER_ILLEGAL_LINKING_OF_ALREADY_LINKED_BUFFER_MESSAGE);
            }
        }

        void throwExceptionIfMaximumOfLinkedBufferReached() const {
            if (numberOfLinkedBuffers.load() == NUMBER_OF_CONCURRENT_INPUTS) {
                throwIllegalActionException(
                        ExceptionMessage::DATA_PROCESSING_SCHEDULER_ILLEGAL_NUMBER_OF_INPUT_BUFFER_MESSAGE);
            }
        }

        void throwExceptionIfInputBufferIsNotLinked(InputBuffer* inputBuffer, char const* message) const {
            if (!isInputBufferLinked(inputBuffer)) {
                throwIllegalActionException(message);
            }
        }

        JoinableThread schedulerThread;

        AtomicFlag terminateOrderReceived;
        Mutex inputBufferStatusMutex;
        AtomicCounter numberOfLinkedBuffers;
        InputBuffers readyToConsumeInputBuffers;
        InputBuffers notReadyToConsumeInputBuffers;
        SINK* dataSink;
    };
}

#endif //SPIRITSENSORGATEWAY_DATAPROCESSINGSCHEDULER_HPP
