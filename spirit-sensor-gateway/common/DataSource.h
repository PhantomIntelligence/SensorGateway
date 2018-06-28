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

#ifndef SPIRITSENSORGATEWAY_DATASOURCE_H
#define SPIRITSENSORGATEWAY_DATASOURCE_H

#include "RingBuffer.hpp"

namespace DataFlow {
/**
 * @brief DataSource interface, has the necessary element (i.e. an OutputBuffer) to be used as an input with ConsumerLink
 */
    template<class T>
    class DataSource {
        using OutputBuffer = DataFlow::RingBuffer<T>;
    protected:
        typedef T DATA;

    public:

        virtual ~DataSource() noexcept = default;

        void linkOutput(ConsumerLink<DATA>* consumer) {
            consumer->linkWith(&outputBuffer);
        }

        virtual void produce(DATA&& data) {
            outputBuffer.write(std::forward<DATA>(data));
        }

    protected:
        OutputBuffer outputBuffer;

    };
}

#endif //SPIRITSENSORGATEWAY_DATASOURCE_H
