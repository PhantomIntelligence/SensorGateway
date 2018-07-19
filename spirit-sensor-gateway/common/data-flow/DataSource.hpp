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

#ifndef SPIRITSENSORGATEWAY_DATASOURCE_HPP
#define SPIRITSENSORGATEWAY_DATASOURCE_HPP

#include "spirit-sensor-gateway/domain/AWLMessage.h"
#include "ThreadSafeRingBuffer.hpp"

namespace DataFlow {

    template<class T>
    class DataSource {

    public:

        virtual ~DataSource() noexcept = default;

        void linkConsumer(ConsumerLink<T>* consumer) {
            if (std::is_same<T, AWLMessage>::value){
                std::cout << 7 << std::endl;
            }
            else{
                std::cout << 6 << std::endl;
            }
            consumer->linkWith(&outputBuffer);
        }

        virtual void produce(T&& data) {
            outputBuffer.write(std::forward<T>(data));
        }

    protected:
        DataFlow::RingBuffer<T> outputBuffer;

    };
}

#endif //SPIRITSENSORGATEWAY_DATASOURCE_HPP
