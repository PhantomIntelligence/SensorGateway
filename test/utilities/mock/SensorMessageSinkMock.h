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

#ifndef SENSORGATEWAY_SENSORMESSAGESINKMOCK_H
#define SENSORGATEWAY_SENSORMESSAGESINKMOCK_H

#include "sensor-gateway/common/data-flow/DataSink.hpp"
#include "test/utilities/files/SensorMessageFileManager.h"


namespace Mock {

    using DataFlow::DataSink;
    using DataFlow::SensorMessage;
    using TestUtilities::Structures::SensorMessages;

    class SensorMessageSinkMock : public DataSink<SensorMessage> {

    protected:

    public:

        explicit SensorMessageSinkMock(uint8_t numberOfDataToConsume) :
                actualNumberOfDataConsumed(0),
                numberOfDataToConsume(numberOfDataToConsume) {
        }

        void consume(DATA&& data) override {
            consumedData.at(actualNumberOfDataConsumed) = data;
            ++actualNumberOfDataConsumed;
        }

        bool hasBeenCalledExpectedNumberOfTimes() const {
            return actualNumberOfDataConsumed.load() == numberOfDataToConsume;
        };


        SensorMessages const& getConsumedData() const noexcept {
            return consumedData;
        }

    private:

        AtomicCounter actualNumberOfDataConsumed;
        AtomicCounter numberOfDataToConsume;
        SensorMessages consumedData;
    };

    using SensorMessageProcessingScheduler = DataFlow::DataProcessingScheduler<SensorMessage, SensorMessageSinkMock, 1>;

}

#endif //SENSORGATEWAY_SENSORMESSAGESINKMOCK_H

