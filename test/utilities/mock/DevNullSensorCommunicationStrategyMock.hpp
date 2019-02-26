/**
	Copyright 2014-2019 Phantom Intelligence Inc.

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

#ifndef SENSORGATEWAY_DEVNULLSENSORCOMMUNICATIONSTRATEGYMOCK_HPP
#define SENSORGATEWAY_DEVNULLSENSORCOMMUNICATIONSTRATEGYMOCK_HPP

#include "test/utilities/data-model/DataModelFixture.h"
#include "sensor-gateway/sensor-communication/SensorCommunicationStrategy.hpp"

namespace Mock {

    template<typename T>
    class DevNullSensorCommunicationStrategyMock final :
            public SensorCommunication::SensorCommunicationStrategy<T> {

    protected:

        using super = SensorCommunication::SensorCommunicationStrategy<T>;

        using typename super::Message;
        using typename super::Messages;
        using typename super::RawData;
        using typename super::RawDataCycles;

    public:

        DevNullSensorCommunicationStrategyMock() = default;

        ~DevNullSensorCommunicationStrategyMock() noexcept = default;

        void openConnection() {};

        Messages fetchMessages() {
            Messages messages;
            return messages;
        }

        RawDataCycles fetchRawDataCycles() {
            RawDataCycles rawDataCycles;
            return rawDataCycles;
        }

        void closeConnection() {}
    };
}

#endif //SENSORGATEWAY_DEVNULLSENSORCOMMUNICATIONSTRATEGYMOCK_HPP
