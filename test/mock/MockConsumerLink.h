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

#ifndef SPIRITSENSORGATEWAY_MOCKCONSUMERLINK_H
#define SPIRITSENSORGATEWAY_MOCKCONSUMERLINK_H

#include "spirit-sensor-gateway/common/ring-buffer/ConsumerLink.hpp"

namespace Mock {

    template<class DATA>
    class MockConsumerLink : public DataFlow::ConsumerLink<DATA> {

        using Buffer = DataFlow::RingBuffer<DATA>;

    public:
        MockConsumerLink() : active(false) {}

        void linkWith(Buffer* buffer) {
            buffer->linkWith(this);
        }

        void activateFor(Buffer* buffer) {
            active = true;
        }

        void deactivateFor(Buffer* buffer) {
            active = false;
        }

        Boolean isActive() const noexcept {
            return active;
        }

    private:
        Boolean active;
    };
}

#endif //SPIRITSENSORGATEWAY_MOCKCONSUMERLINK_H
