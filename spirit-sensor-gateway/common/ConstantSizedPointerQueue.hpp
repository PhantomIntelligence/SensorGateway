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


#ifndef SPIRITSENSORGATEWAY_CONSTANTSIZEDPOINTERQUEUE_HPP
#define SPIRITSENSORGATEWAY_CONSTANTSIZEDPOINTERQUEUE_HPP

#include "ConstantSizedAbstractPointerArray.hpp"

namespace Container {

    /**
     * @brief Constant sized FIFO pointer structure.
     * It basically acts as a buffer for pointers.
     * It only offers the basic store and consumeNext operations. Note that it offers no random access.
     * @template <class T, std::size_t SIZE> refers to the SIZE number of data of type T that this Queue will contain.
     */
    template<class T, std::size_t SIZE>
    class ConstantSizedPointerQueue final : ConstantSizedAbstractPointerArray<T, SIZE> {

        using super = ConstantSizedAbstractPointerArray<T, SIZE>;

    public:

        using super::ConstantSizedAbstractPointerArray;
        using super::empty;
        using super::full;


        void store(T* newPointer) override {
            LockGuard guard(contentMutex);
            super::validateNotFull();
            super::storePointerAtProducerMarker(newPointer);
            producerMarker = super::advanceMarker(producerMarker);
        }

        auto consumeNext() -> T* override {
            LockGuard guard(contentMutex);
            super::validateNotEmpty();
            T* contentToReturn = super::consumePointerAtConsumerMarker();
            consumerMarker = super::advanceMarker(consumerMarker);
            return contentToReturn;
        }

    private:

        using super::pointers;

        using super::contentMutex;
        using super::producerMarker;
        using super::consumerMarker;
        using super::BEGIN;
        using super::END;

        using super::numberOfPointersHeld;

    };
}


#endif //SPIRITSENSORGATEWAY_CONSTANTSIZEDPOINTERQUEUE_HPP

