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


#ifndef SENSORGATEWAY_CONSTANTSIZEDPOINTERLIST_HPP
#define SENSORGATEWAY_CONSTANTSIZEDPOINTERLIST_HPP

#include "ConstantSizedPointerQueue.hpp"

namespace Container {

    /**
     * @brief Constant sized list pointer structure.
     * It acts as a basic FIFO container with added methods to ease the removal and to check if a pointer is held.
     * It also offers to peek at the next pointer that will be consume without removing it.
     * @template <class T, std::size_t SIZE> refers to the SIZE number of data of type T that this List will contain.
     */
    template<class T, std::size_t SIZE>
    class ConstantSizedPointerList final : ConstantSizedAbstractPointerArray<T, SIZE> {

        using super = ConstantSizedAbstractPointerArray<T, SIZE>;
        using MarkerPosition = typename super::IteratorMarker;

    public:

        using super::ConstantSizedAbstractPointerArray;
        using super::isEmpty;
        using super::isFull;

        void store(T* newPointer) override {
            LockGuard guard(contentMutex);
            super::validateNotFull();
            placeProducerMarkerOnNextUnoccupiedPosition();
            super::storePointerAtProducerMarker(newPointer);
        }

        auto consumeNext() -> T* override {
            LockGuard guard(contentMutex);
            super::validateNotEmpty();
            placeConsumerMarkerOnNextOccupiedPosition();
            T* contentToReturn = super::consumePointerAtConsumerMarker();
            return contentToReturn;
        }

        auto readNextPointerToConsume() -> T* {
            LockGuard guard(contentMutex);
            super::validateNotEmpty();
            placeConsumerMarkerOnNextOccupiedPosition();
            T* contentToReturn = super::readPointerAtConsumerMarker();
            return contentToReturn;
        }

        bool contains(T* pointerToFind) const noexcept {
            LockGuard guard(contentMutex);
            auto position = findPositionOf(pointerToFind);
            return position != END;
        }

        void remove(T* pointerToRemove) {
            LockGuard guard(contentMutex);
            auto position = findPositionOf(pointerToRemove);
            if (position == END) {
                throwIllegalRemovalException();
            }
            *position = nullptr;
            --numberOfPointersHeld;
        }

        uint16_t getNumberOfPointerHeld() const noexcept {
            return numberOfPointersHeld.load();
        }

    private:
        void placeProducerMarkerOnNextUnoccupiedPosition() {
            while (*producerMarker != nullptr) {
                producerMarker = super::advanceMarker(producerMarker);
            }
        }

        void placeConsumerMarkerOnNextOccupiedPosition() {
            while (*consumerMarker == nullptr) {
                consumerMarker = super::advanceMarker(consumerMarker);
            }
        }

        MarkerPosition findPositionOf(T* pointerToFind) const noexcept {
            return std::find(BEGIN, END, pointerToFind);
        }

        [[noreturn]] void throwIllegalRemovalException() const {
            throwIllegalActionException(ExceptionMessage::CONSTANT_SIZED_POINTER_LIST_ILLEGAL_REMOVAL_OF_POINTER);
        }

        using super::pointers;
        using super::contentMutex;

        using super::producerMarker;
        using super::consumerMarker;
        using super::BEGIN;
        using super::END;

        using super::numberOfPointersHeld;
    };
}


#endif //SENSORGATEWAY_CONSTANTSIZEDPOINTERLIST_HPP

