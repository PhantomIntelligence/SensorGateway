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


#ifndef SENSORGATEWAY_RINGBUFFERPAD_HPP
#define SENSORGATEWAY_RINGBUFFERPAD_HPP


namespace DataFlow {

    /**
     * @warning Concurrency Warning: No thread safety has been implemented in this class. This is the responsibility of
     * the RingBuffer.
     */
    template<class T>
    class RingBufferPad {

    public:

        RingBufferPad() : nextPadSet(false), currentData(T::returnDefaultData()), nextPad(this) {}

        ~RingBufferPad() = default;

        /**
         * @warning The RingBufferPads are intended to be used as const instances. They shouldn't be moved.
         */
        RingBufferPad(RingBufferPad&& other) noexcept = default;


        /**
         * @warning The RingBufferPads are intended to be used as const instances. They shouldn't be copied.
         */
        RingBufferPad(RingBufferPad const& other) = delete;

        /**
         * @warning  The RingBufferPads are intended to be used as const instances. They shouldn't be assigned.
         */
        RingBufferPad& operator=(const RingBufferPad& other) = delete;

        /**
         * @warning The RingBufferPads are intended to be used as const instances. They shouldn't be assigned.
         */
        RingBufferPad& operator=(RingBufferPad&& other) = delete;


        void setNext(RingBufferPad* nextBufferPad) {
            if (!nextPadSet) {
                nextPad = nextBufferPad;
                nextPadSet = !nextPadSet;
            }
        }

        RingBufferPad* next() {
            return nextPad;
        }

        void write(T&& dataToWrite) {
            currentData = std::forward<T>(dataToWrite);
        }

        auto read() const -> T const& {
            return currentData;
        }

    protected:

        bool nextPadSet;

        T currentData;

        RingBufferPad<T>* nextPad;
    };
}


#endif //SENSORGATEWAY_RINGBUFFERPAD_HPP
