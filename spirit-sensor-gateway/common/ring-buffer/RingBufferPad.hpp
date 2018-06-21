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


#ifndef SPIRITSENSORGATEWAY_RINGBUFFERPAD_HPP
#define SPIRITSENSORGATEWAY_RINGBUFFERPAD_HPP

/**
 * @brief Buffer and data containing related classes
 */
namespace DataFlow {

    /**
     * @brief A RingBufferPad is a custom forward-list node used by RingBuffer to ease the circular chaining, allow to read/write data
     * @template <class T> refers to the data type
     * @warning Concurrency Warning: No thread safety has been implemented in this class. This is the responsibility the RingBuffer. It must be assumed any concurrent interaction potentially dangerous is dealt with before instances of this class are called
     */
    template<class T>
    class RingBufferPad {

        /**
         * @brief Name conversion to ease reading in the class. The data held in this call will be reference to by T).
         */
        typedef RingBufferPad<T> Pad;

    public:

        /**
         * @brief Default constructor, which sets the currentData to its Default value.
         * @attention the function "T::returnDefaults()" MUST be implemented in the passed Data type.
         * @see NativeData for a valid implementation example of the function returnDefaults()
         */
        RingBufferPad() : nextPadSet(false), currentData(T::returnDefaultData()), nextPad(this) {}

        /**
         * @brief default destructor
         */
        ~RingBufferPad() = default;

        /**
         * @brief Defaulted move constructor
         * @param other the other RingBufferPad (to be moved)
         */
        RingBufferPad(RingBufferPad&& other) noexcept = default;


        /**
         * @brief The RingBufferPads are intended to be used as const instances. They shouldn't be copied.
         */
        RingBufferPad(RingBufferPad const& other) = delete;

        /**
         * @brief The RingBufferPads are intended to be used as const instances. They shouldn't be assigned.
         */
        RingBufferPad& operator=(const RingBufferPad& other) = delete;

        /**
         * @brief The RingBufferPads are intended to be used as const instances. They shouldn't be assigned.
         */
        RingBufferPad& operator=(RingBufferPad&& other) = delete;


        /**
         * @brief Links this RingBufferPad to the one passed in parameters. Note that
         * @param nextPad The RingBufferPad pointer which will be obtained on .next() call
         * @note The "next" RingBufferPad can only be set once. It shall remain this way.
         */
        void setNext(RingBufferPad* nextBufferPad) {
            if (!nextPadSet) {
                nextPad = nextBufferPad;
                nextPadSet = !nextPadSet;
            }
        }

        /**
         * @brief Allows to move forward in the RingBuffer
         * @return a pointer to the next RingBufferPad, which is set on .setNext() call
         * @note if the next RingBufferPad hasn't been set, this function will return a pointer to the same instance with which it was called.
         */
        RingBufferPad* next() {
            return nextPad;
        }

        /**
         * @brief Replaces the currently held T instance with the passed on
         * @see RingBufferPad's concurrency warning
         * @param dataToWrite Universal Reference to the new T.
         */
        void write(T&& dataToWrite) {
            currentData = dataToWrite;
        }

        /**
         * @brief Read allows to access the T. It does not modify nor invalidate it.
         * @see RingBufferPad's concurrency warning
         * @return A copy of the current T
         */
        auto read() const -> T const& {
            return currentData;
        }

    protected:
        /**
         * @brief A flag which is to be set "true" only once, controls the setting of "nextBufferPad"
         * @see nextBufferPad
         */
        bool nextPadSet;

        /**
         * @brief The currently held T, which is accessible for both read and write
         * @see RingBufferPad's concurrency warning
         */
        T currentData;

        /**
         * @brief The next RingBufferPad instance in the forward-list. It can be set only once.
         * @see nextPadSet, setNext()
         */
        Pad* nextPad;
    };
}


#endif //SPIRITSENSORGATEWAY_RINGBUFFERPAD_HPP
