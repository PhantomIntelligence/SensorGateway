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

#ifndef SENSORGATEWAY_AWL16ACCESSLINK_H
#define SENSORGATEWAY_AWL16ACCESSLINK_H

#include "SensorAccessLink.hpp"
#include "sensor-gateway/sensor-communication/KvaserCanCommunicationStrategy.h"
#include "sensor-gateway/data-translation/AWLTranslationStrategy.h"

namespace SensorGateway {

    struct AWL16AccessLink {

        using CommunicationStructures = Sensor::AWL::Structures;
        using Factory = typename SensorGateway::SensorAccessLinkFactory<CommunicationStructures>;
        using GatewayStructures = typename Factory::Structures;
        using AccessLink = typename Factory::AccessLink;

        using ServerCommunicationStrategy = AccessLink::ServerCommunicationStrategy;
        using DataTranslationStrategy = DataTranslation::AWLTranslationStrategy;
        using SensorCommunicationStrategy = SensorCommunication::KvaserCanCommunicationStrategy;
    };
}

#endif //SENSORGATEWAY_AWL16ACCESSLINK_H
