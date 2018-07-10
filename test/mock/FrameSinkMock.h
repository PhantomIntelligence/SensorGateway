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

#ifndef SPIRITSENSORGATEWAY_FRAMESINKMOCK_H
#define SPIRITSENSORGATEWAY_FRAMESINKMOCK_H

#include "spirit-sensor-gateway/common/data-flow/DataSink.hpp"
#include "spirit-sensor-gateway/domain/Frame.h"

using DataFlow::DataSink;
using DataFlow::Frame;

namespace Mock {

    class FrameSinkMock : public DataSink<Frame> {

    public:

        FrameSinkMock(uint8_t numberOfDataToConsume) :
                actualNumberOfDataConsumed(0),
                numberOfDataToConsume(numberOfDataToConsume) {

        }

        void consume(DATA&& data) override {
            ++actualNumberOfDataConsumed;
            consumedData.push_back(data);
        }

        bool hasBeenCalledExpectedNumberOfTimes() const {
            return actualNumberOfDataConsumed.load() == numberOfDataToConsume;
        };


        std::vector<Frame> getConsumedData() const noexcept {
            return consumedData;
        }


    private:
        AtomicCounter actualNumberOfDataConsumed;
        AtomicCounter numberOfDataToConsume;
        std::vector<Frame> consumedData;
    };

    using FrameProcessingScheduler = DataFlow::DataProcessingScheduler<Frame, FrameSinkMock, 1>;

}

#endif //SPIRITSENSORGATEWAY_FRAMESINKMOCK_H

