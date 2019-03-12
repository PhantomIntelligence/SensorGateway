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


#ifndef SENSORGATEWAY_CONSUMERLINK_HPP
#define SENSORGATEWAY_CONSUMERLINK_HPP

#include "sensor-gateway/common/container/ConstantSizedPointerList.hpp"

namespace {
    constexpr size_t const ONLY_ONE_CONSUMER = 1;
    constexpr size_t const ONLY_ONE_PRODUCER = 1;
}

namespace DataFlow {

    template<class T, size_t N = RING_BUFFER_DEFAULT_SIZE, size_t C = NUMBER_OF_CONSUMER_PER_BUFFER>
    class RingBuffer;

    template<class T, size_t N = RING_BUFFER_DEFAULT_SIZE, size_t C = NUMBER_OF_CONSUMER_PER_BUFFER>
    class ConsumerLink {

    protected:

        ConsumerLink() = default;

        virtual ~ConsumerLink() {}

    public:

        virtual void linkWith(RingBuffer<T, N, C>* buffer) = 0;

        virtual void activateFor(RingBuffer<T, N, C>* buffer) = 0;

        virtual void deactivateFor(RingBuffer<T, N, C>* buffer) = 0;

    };
}

#endif //SENSORGATEWAY_CONSUMERLINK_HPP
