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


#ifndef SPIRITSENSORGATEWAY_THREADSAFERINGBUFFER_H
#define SPIRITSENSORGATEWAY_THREADSAFERINGBUFFER_H

#include "RingBuffer.hpp"

namespace DataFlow {

    /**
     * @brief Thread safe variant of the RingBuffer. This is a Multiple-Producer-Multiple-Consumer structure.
     * @template <class T> refers to the data type that this buffer will contain.
     * @note Use this class ONLY when more than one thread needs to write to the RingBuffer
     */
    template<class T>
    class ThreadSafeRingBuffer : public RingBuffer<T> {
        using super = RingBuffer<T>;

    protected:

        typedef T DATA;

    public:

        ThreadSafeRingBuffer() : super() {
        }

        /**
         * @brief Defaulted move constructor
         * @param other the other RingBuffer (to be moved)
         */
        ThreadSafeRingBuffer(ThreadSafeRingBuffer&& other) noexcept = default;

        virtual ~ThreadSafeRingBuffer() = default;

        /**
         * @brief The ThreadSafeRingBuffer are intended to be used as const instances. They shouldn't be assigned.
         */
        ThreadSafeRingBuffer(ThreadSafeRingBuffer const& other) = delete;

        /**
         * @brief The ThreadSafeRingBuffer are intended to be used as const instances. They shouldn't be assigned.
         */
        ThreadSafeRingBuffer& operator=(ThreadSafeRingBuffer const& other) = delete;

        /**
         * @brief The ThreadSafeRingBuffer are intended to be used as const instances. They shouldn't be copied.
         */
        ThreadSafeRingBuffer& operator=(ThreadSafeRingBuffer&& other) = delete;


        /**
        * @brief Writes the data to the RingBufferPad the writer is currently on, moves the writer and notifies all subscribed consumers they can be activated and start to read.
        * @param data The data that will be written
        */
        void write(DATA&& data) override {
            LockGuard guard(writeMutex);
            super::write(std::forward(data));
        }

    private:
        Mutex writeMutex;
    };


}
#endif //SPIRITSENSORGATEWAY_THREADSAFERINGBUFFER_H
