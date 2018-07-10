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

#ifndef SPIRITSENSORGATEWAY_CONSUMERLINKMOCK_H
#define SPIRITSENSORGATEWAY_CONSUMERLINKMOCK_H

#include "spirit-sensor-gateway/common/buffer/ConsumerLink.hpp"

namespace Mock {

    template<class DATA>
    class ConsumerLinkMock : public DataFlow::ConsumerLink<DATA> {

        using Buffer = DataFlow::RingBuffer<DATA>;

    public:
        ConsumerLinkMock() : active(false) {}

        void linkWith(Buffer* buffer) {
            buffer->linkWith(this);
        }

        void activateFor(Buffer* buffer) {
            active = true;
        }

        void deactivateFor(Buffer* buffer) {
            active = false;
        }

        bool isActive() const noexcept {
            return active;
        }

    private:
        bool active;
    };
}

#endif //SPIRITSENSORGATEWAY_CONSUMERLINKMOCK_H
