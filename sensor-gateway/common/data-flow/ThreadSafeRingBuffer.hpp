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


#ifndef SENSORGATEWAY_THREADSAFERINGBUFFER_H
#define SENSORGATEWAY_THREADSAFERINGBUFFER_H

#include "RingBuffer.hpp"

namespace DataFlow {

    /**
     * @warning Use this class ONLY when more than one thread needs to write to the RingBuffer
     */
    template<class T, size_t N = RING_BUFFER_DEFAULT_SIZE, size_t C = NUMBER_OF_CONSUMER_PER_BUFFER>
    class ThreadSafeRingBuffer : public RingBuffer<T, N, C> {
        using super = RingBuffer<T, N, C>;

    public:

        ThreadSafeRingBuffer() : super() {
        }

        ThreadSafeRingBuffer(ThreadSafeRingBuffer&& other) noexcept = default;

        virtual ~ThreadSafeRingBuffer() = default;

        /**
         * @brief The ThreadSafeRingBuffers are intended to be used as const instances. They shouldn't be assigned.
         */
        ThreadSafeRingBuffer(ThreadSafeRingBuffer const& other) = delete;

        /**
         * @brief The ThreadSafeRingBuffers are intended to be used as const instances. They shouldn't be assigned.
         */
        ThreadSafeRingBuffer& operator=(ThreadSafeRingBuffer const& other) = delete;

        /**
         * @brief The ThreadSafeRingBuffers are intended to be used as const instances. They shouldn't be copied.
         */
        ThreadSafeRingBuffer& operator=(ThreadSafeRingBuffer&& other) = delete;

        void write(T&& data) override {
            LockGuard guard(writeMutex);
            super::write(std::forward(data));
        }

    private:
        Mutex writeMutex;
    };


}
#endif //SENSORGATEWAY_THREADSAFERINGBUFFER_H
