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

namespace DataFlow {

    template<class T>
    class RingBuffer;

    template<class T>
    class ConsumerLink {

    protected:

        ConsumerLink() = default;

        virtual ~ConsumerLink() {}

    public:

        virtual void linkWith(RingBuffer<T>* buffer) = 0;

        virtual void activateFor(RingBuffer<T>* buffer) = 0;

        virtual void deactivateFor(RingBuffer<T>* buffer) = 0;
    };
}

#endif //SENSORGATEWAY_CONSUMERLINK_HPP
