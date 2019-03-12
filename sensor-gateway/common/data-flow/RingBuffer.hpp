/**
	Copyright 2014-2019 Phantom Intelligence Inc.

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


#ifndef SENSORGATEWAY_RINGBUFFER_H
#define SENSORGATEWAY_RINGBUFFER_H

#include <unordered_map>
#include "ConsumerLink.hpp"
#include "RingBufferPad.hpp"

namespace DataFlow {

    template<class T, size_t N, size_t C>
    class RingBuffer {

    protected:

        using Consumer = ConsumerLink<T, N, C>;

        using ConsumerLocationsMap = std::unordered_map<Consumer*, RingBufferPad<T>*>;

    public:

        static constexpr size_t const size = N;
        static constexpr size_t const numberOfConsumers = C;

        RingBuffer() : writerLocation(&buffer[0]) {
            consumerLocationsMap.reserve(numberOfConsumers);
            chainPadsInBuffer();
        }

        virtual ~RingBuffer() = default;

        /**
         *@warning The RingBuffers are intended to be used as const instances. They shouldn't be moved.
         */
        RingBuffer(RingBuffer&& other) noexcept = default;

        /**
         * @warning The RingBuffers are intended to be used as const instances. They shouldn't be assigned.
         */
        RingBuffer(RingBuffer const& other) = delete;

        /**
         * @warning The RingBuffers are intended to be used as const instances. They shouldn't be assigned.
         */
        RingBuffer& operator=(RingBuffer const& other) = delete;

        /**
         * @warning The RingBuffers are intended to be used as const instances. They shouldn't be copied.
         */
        RingBuffer& operator=(RingBuffer&& other) = delete;


        virtual void write(T&& data) {
            writerLocation->write(std::forward<T>(data));
            writerLocation = writerLocation->next();
            notifyConsumersIfAnyDataIsPresent();
        }


        virtual auto consumeNextDataFor(Consumer* consumer) -> T const& {
            addLinkIfNoneExists(consumer);
            throwErrorIfIllegalConsumption(consumer);
            T const& currentData = consumeFor(consumer);
            advanceOrDeactivateConsumer(consumer);
            return currentData;
        }

        virtual void linkWith(Consumer* consumer) noexcept {
            addLinkIfNoneExists(consumer);
        }

    private:

        void chainPadsInBuffer() {
            auto firstPad = &buffer[0];
            auto currentPad = firstPad;
            RingBufferPad<T>* nextPad;
            for (size_t i = 1; i < size; ++i) {
                nextPad = &buffer[i];
                currentPad->setNext(nextPad);
                currentPad = nextPad;
            }
            buffer[size - 1].setNext(firstPad);
        }

        void addLinkIfNoneExists(Consumer* consumer) {
            if (hasNoExistingLinkWith(consumer)) {
                RingBufferPad<T>* consumerLocation = &buffer[0];
                auto consumerLocationPair = std::make_pair(consumer, consumerLocation);
                this->consumerLocationsMap.insert(consumerLocationPair);
            }
        }

        bool hasNoExistingLinkWith(Consumer* consumer) {
            return consumerLocationsMap.count(consumer) == 0;
        }

        void notifyConsumersIfAnyDataIsPresent() {
            if (!consumerLocationsMap.empty()) {
                for (auto location = consumerLocationsMap.begin(), end = consumerLocationsMap.end();
                     location != end; ++location) {
                    if (!istWriterLocation((*location).second)) {
                        (*location).first->activateFor(this);
                    }
                }
            }
        }

        bool istWriterLocation(RingBufferPad<T>* pad) const {
            return pad == writerLocation;
        }

        bool isAtWriterLocation(Consumer* consumer) const {
            return istWriterLocation(consumerLocationsMap.at(consumer));
        }

        auto consumeFor(Consumer* consumer) -> T const& {
            T const& data = consumerLocationsMap.at(consumer)->read();
            return data;
        }

        void throwErrorIfIllegalConsumption(Consumer* consumer) {
            if (isAtWriterLocation(consumer)) {
                // TODO : Find a better way to report the error than this e.g.: logger
                std::cout << ExceptionMessage::RING_BUFFER_ILLEGAL_CONSUMPTION_ON_WRITER_LOCATION_MESSAGE << std::endl;

            }
        }

        void advanceOrDeactivateConsumer(Consumer* consumer) {
            if (!isAtWriterLocation(consumer)) {
                consumerLocationsMap.at(consumer) = consumerLocationsMap.at(consumer)->next();
            }
            if (isAtWriterLocation(consumer)) {
                consumer->deactivateFor(this);
            }
        }

        RingBufferPad<T>* writerLocation;

        RingBufferPad<T> buffer[size];

        ConsumerLocationsMap consumerLocationsMap;

    };
}
#endif //SENSORGATEWAY_RINGBUFFER_H
