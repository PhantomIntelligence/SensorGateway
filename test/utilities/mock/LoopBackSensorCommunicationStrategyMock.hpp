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

#ifndef SENSORGATEWAY_LOOPBACKSENSORCOMMUNICATIONSTRATEGYMOCK_HPP
#define SENSORGATEWAY_LOOPBACKSENSORCOMMUNICATIONSTRATEGYMOCK_HPP

#include "Function.hpp"
#include "sensor-gateway/sensor-communication/SensorCommunicationStrategy.hpp"

namespace Mock {
    
    template<typename T>
    class LoopBackSensorCommunicationStrategyMock final :
            public SensorCommunication::SensorCommunicationStrategy<T> {

    protected:

        using super = SensorCommunication::SensorCommunicationStrategy<T>;

        using Message = typename super::Message;
        using Request = typename super::Request;
        using Messages = typename super::Messages;
        using RawData = typename super::RawData;
        using RawDataCycles = typename super::RawDataCycles;

    public:

        LoopBackSensorCommunicationStrategyMock() = default;

        ~LoopBackSensorCommunicationStrategyMock() noexcept = default;

        void openConnection() override {};

        void closeConnection() override {};

        Messages fetchMessages() override {
            Messages messages;
            return messages;
        }

        RawDataCycles fetchRawDataCycles() override {
            RawDataCycles rawDataCycles;
            return rawDataCycles;
        }

        void sendRequest(Request&& request) override {
            // TODO : Complete with test/utilities/mock/Function.hpp
        }

        void waitUntilSendRequestIsCalled() {
                // TODO : Complete with test/utilities/mock/Function.hpp
        };

        bool hasSendRequestBeenCalled() const {
            // TODO : Complete with test/utilities/mock/Function.hpp
            return false;
        };

    };
}

#endif //SENSORGATEWAY_LOOPBACKSENSORCOMMUNICATIONSTRATEGYMOCK_HPP
