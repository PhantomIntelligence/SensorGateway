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


#ifndef SPIRITSENSORGATEWAY_RINGBUFFER_H
#define SPIRITSENSORGATEWAY_RINGBUFFER_H

#include <unordered_map>
#include "ConsumerLink.hpp"
#include "RingBufferPad.hpp"

namespace {
    size_t const RING_BUFFER_SIZE = 64;
}

namespace DataFlow {

    /**
     * @brief Monitors and allows access to different data types. This is a Single-Producer-Multiple-Consumer structure.
     * @template <class T> refers to the data type that this buffer will contain.
     */
    template<class T>
    class RingBuffer {

    protected:

        /**
         * @brief Name encapsulation to ease reading and decouple the class from the template's name
         */
        typedef T DATA;

        /**
         * @brief RingBufferPad specialized at compile time for the specified DATA
         */
        typedef RingBufferPad<DATA> Pad;

        /**
         * @brief ConsumerLink specialized at compile time for the specified DATA
         */
        typedef ConsumerLink<DATA> Consumer;

        /**
         * @brief Main tool of this class, is used to keep track of the various positions of the different consumers in the buffer.
         */
        typedef std::unordered_map<Consumer*, Pad*> ConsumerMap;

        /**
         * @brief A std::pair specialization for the ConsumerMap
         */
        typedef std::pair<Consumer*, Pad*> ConsumerLocationPair;

        const uint16_t NUMBER_OF_CONSUMER_PER_BUFFER = 8;

    public:

        /**
         * @brief Default constructor.
         * @note The map size is *by default* set to NUMBER_OF_CONSUMER_PER_BUFFER. The intention is to prevent dynamic resizing. Would a better way to make the memory usage be as static and efficient as possible, it should be implemented.
         */
        RingBuffer() : writerLocation(&ring[0]) {
            locations.reserve(NUMBER_OF_CONSUMER_PER_BUFFER);
            chainPadsInARing();
        }

        /**
         * @brief Defaulted move constructor
         * @param other the other RingBuffer (to be moved)
         */
        RingBuffer(RingBuffer&& other) noexcept = default;

        virtual ~RingBuffer() = default;

        /**
         * @brief The RingBuffer are intended to be used as const instances. They shouldn't be assigned.
         */
        RingBuffer(RingBuffer const& other) = delete;

        /**
         * @brief The RingBuffer are intended to be used as const instances. They shouldn't be assigned.
         */
        RingBuffer& operator=(RingBuffer const& other) = delete;

        /**
         * @brief The RingBuffer are intended to be used as const instances. They shouldn't be copied.
         */
        RingBuffer& operator=(RingBuffer&& other) = delete;


       /**
        * @brief Writes the data to the RingBufferPad the writer is currently on, moves the writer and notifies all subscribed consumers they can be activated and start to read.
        * @param data The data that will be written
        */
        virtual void write(DATA&& data) {
            writerLocation->write(std::forward<DATA>(data));
            writerLocation = writerLocation->next();
            notifyConsumersIfAnyIsPresent();
        }


        /**
         * @brief Read allows to access the DATA. It does not modify nor invalidate it.
         * @see SensorData::RingBufferPad's class documentation for Concurrency Warning
         * @param consumer A pointer to the ConsumerLink<DATA> which is related to the consumption
         * @return A copy of the current DATA
         */
        virtual auto consumeNextDataFor(Consumer* consumer) -> DATA const& {
            addLinkIfNoneExists(consumer);
            throwErrorIfIllegalConsumption(consumer);
            DATA const& currentData = consumeFor(consumer);
            advanceOrDeactivateConsumer(consumer);
            return currentData;

        }


        /**
         * @brief Adds a Consumer* to its list. This consumer will be notified when a new data is written
         * @param consumer a pointer to ConsumerLink<DATA>
         */
        virtual void linkWith(Consumer* consumer) noexcept {
            addLinkIfNoneExists(consumer);
        }


    private:

        void chainPadsInARing() {
            auto firstPad = &ring[0];
            auto currentPad = firstPad;
            Pad* nextPad;
            for (auto i = 1; i < RING_BUFFER_SIZE; ++i) {
                nextPad = &ring[i];
                currentPad->setNext(nextPad);
                currentPad = nextPad;
            }
            ring[RING_BUFFER_SIZE - 1].setNext(firstPad);
        }

        void addLinkIfNoneExists(Consumer* consumer) {
            if (linkNotExistingWith(consumer)) {
                Pad* consumerLocation = &ring[0];
                this->locations.insert(ConsumerLocationPair(consumer, consumerLocation));
            }
        }

        bool linkNotExistingWith(Consumer* consumer) {
            return locations.count(consumer) == 0;
        }

        void notifyConsumersIfAnyIsPresent() {
            if (!locations.empty()) {
                for (auto location = locations.begin(), end = locations.end(); location != end; ++location) {
                    if (!isOnWriter((*location).second)) {
                        (*location).first->activateFor(this);
                    }
                }
            }
        }

        bool isOnWriter(Pad* pad) const {
            return pad == writerLocation;
        }

        bool isOnWriter(Consumer* consumer) const {
            return isOnWriter(locations.at(consumer));
        }

        auto consumeFor(Consumer* consumer) -> DATA const& {
            DATA const& data = locations.at(consumer)->read();
            return data;
        }

        void throwErrorIfIllegalConsumption(Consumer* consumer) {
            if (isOnWriter(consumer)) {
                throwConsumerOnWriterException();
            }
        }

        [[noreturn]] void throwConsumerOnWriterException() const {
            auto CONSUMPTION_ON_WRITER_MESSAGE = "Illegal consumption, execution should not reach this point. The calling entity should not be allowed to proceed to this call";
            std::runtime_error schedulerIsFullException(CONSUMPTION_ON_WRITER_MESSAGE);
            throw schedulerIsFullException;
        }

        void advanceOrDeactivateConsumer(Consumer* consumer) {
            if (!isOnWriter(consumer)) {
                locations.at(consumer) = locations.at(consumer)->next();
            }
            if (isOnWriter(consumer)) {
                consumer->deactivateFor(this);
            }
        }

        Pad* writerLocation;

        Pad ring[RING_BUFFER_SIZE];

        ConsumerMap locations;

    };


}
#endif //SPIRITSENSORGATEWAY_RINGBUFFER_H
