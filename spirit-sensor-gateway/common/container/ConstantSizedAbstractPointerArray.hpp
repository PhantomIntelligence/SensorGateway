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


#ifndef SPIRITSENSORGATEWAY_ABSTRACTPOINTERARRAY_HPP
#define SPIRITSENSORGATEWAY_ABSTRACTPOINTERARRAY_HPP

#include <algorithm>
#include "spirit-sensor-gateway/common/ConstantFunctionsDefinition.h"

namespace Container {

    template<class T, std::size_t SIZE>
    class ConstantSizedAbstractPointerArray {
    protected:

        using Array = std::array<T, SIZE>;
        using PointerArray = std::array<T*, SIZE>;
        using IteratorMarker = typename PointerArray::iterator;

        inline void storePointerAtProducerMarker(T* pointer) {
            *producerMarker = pointer;
            ++numberOfPointersHeld;
        }

        inline T* readPointerAtConsumerMarker() const noexcept {
            return *consumerMarker;
        }

        inline T* consumePointerAtConsumerMarker() {
            T* contentToReturn = readPointerAtConsumerMarker();
            *consumerMarker = nullptr;
            --numberOfPointersHeld;
            return contentToReturn;
        }

        inline IteratorMarker advanceMarker(IteratorMarker marker) const noexcept {
            auto newPosition = ++marker;
            if (newPosition == END) {
                return BEGIN;
            }
            return newPosition;
        }

        void validateNotFull() const {
            if (isFull()) {
                throwIllegalActionException(ExceptionMessage::ABSTRACT_POINTER_ARRAY_ILLEGAL_STORE_FULL);
            }
        }

        void validateNotEmpty() const {
            if (isEmpty()) {
                throwIllegalActionException(ExceptionMessage::ABSTRACT_POINTER_ARRAY_ILLEGAL_CONSUMPTION_EMPTY);
            }
        }

        PointerArray pointers;
        mutable Mutex contentMutex;

        IteratorMarker producerMarker;
        IteratorMarker consumerMarker;
        IteratorMarker const BEGIN;
        IteratorMarker const END;

        AtomicCounter numberOfPointersHeld;

    public:

        ConstantSizedAbstractPointerArray() :
                producerMarker(pointers.begin()),
                consumerMarker(pointers.begin()),
                BEGIN(pointers.begin()),
                END(pointers.end()),
                numberOfPointersHeld(0) {
            pointers.fill(nullptr);
        }

        ~ConstantSizedAbstractPointerArray() noexcept = default;

        /**
         * @brief The CircularPointerArray are intended to be used as const instances. They shouldn't be moved.
         */
        ConstantSizedAbstractPointerArray(ConstantSizedAbstractPointerArray&& other) noexcept = delete;

        /**
         * @brief The CircularPointerArray are intended to be used as const instances. They shouldn't be moved.
         */
        ConstantSizedAbstractPointerArray(ConstantSizedAbstractPointerArray const& other) = delete;

        /**
         * @brief The CircularPointerArray are intended to be used as const instances. They shouldn't be assigned.
         */
        ConstantSizedAbstractPointerArray& operator=(ConstantSizedAbstractPointerArray const& other) = delete;

        /**
         * @brief The CircularPointerArray are intended to be used as const instances. They shouldn't be copied.
         */
        ConstantSizedAbstractPointerArray& operator=(ConstantSizedAbstractPointerArray&& other) noexcept = delete;

        bool isEmpty() const noexcept {
            return numberOfPointersHeld.load() == 0;
        }

        bool isFull() const noexcept {
            return numberOfPointersHeld.load() == SIZE;
        }

        virtual void store(T* newPointer) = 0;

        virtual T* consumeNext() = 0;
    };
}


#endif //SPIRITSENSORGATEWAY_ABSTRACTPOINTERARRAY_HPP

