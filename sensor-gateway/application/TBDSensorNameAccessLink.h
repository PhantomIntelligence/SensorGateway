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

#ifndef SENSORGATEWAY_TBDSENSORNAMEACCESSLINK_H
#define SENSORGATEWAY_TBDSENSORNAMEACCESSLINK_H

#include "SensorAccessLink.hpp"
#include "sensor-gateway/sensor-communication/TBDSensorNameUSBCommunicationStrategy.h"
#include "sensor-gateway/message-translation/AWLTranslationStrategy.h"

namespace SensorGateway {

class TBDSensorNameAccessLink final : public SensorAccessLink<Sensor::TBDSensorName::Structures, DataFlow::Frame> {
    protected:
        using super = SensorAccessLink<Sensor::TBDSensorName::Structures, DataFlow::Frame>;

        using super::ServerCommunicationStrategy;

    public:

        explicit TBDSensorNameAccessLink(ServerCommunicationStrategy* serverCommunicationStrategy)
                : super(serverCommunicationStrategy,
                        &awlTranslationStrategy,
                        &tbdSensorNameUSBCommunicationStrategy) {}

        ~TBDSensorNameAccessLink() noexcept = default;

        TBDSensorNameAccessLink(TBDSensorNameAccessLink const& other) = delete;

        TBDSensorNameAccessLink(TBDSensorNameAccessLink&& other) noexcept = delete;

        TBDSensorNameAccessLink& operator=(TBDSensorNameAccessLink const& other)& = delete;

        TBDSensorNameAccessLink& operator=(TBDSensorNameAccessLink&& other)& noexcept = delete;

    private:

        MessageTranslation::AWLTranslationStrategy awlTranslationStrategy;
        SensorCommunication::TBDSensorNameUSBCommunicationStrategy tbdSensorNameUSBCommunicationStrategy;
    };
}

#endif //SENSORGATEWAY_TBDSENSORNAMEACCESSLINK_H