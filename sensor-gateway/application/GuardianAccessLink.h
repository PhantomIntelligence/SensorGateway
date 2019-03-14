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

#ifndef SENSORGATEWAY_GUARDIANACCESSLINK_H
#define SENSORGATEWAY_GUARDIANACCESSLINK_H

#include "SensorAccessLink.hpp"

#include "sensor-gateway/sensor-communication/GuardianUSBCommunicationStrategy.hpp"
#include "sensor-gateway/data-translation/GuardianTranslationStrategy.hpp"

namespace SensorGateway {

    struct GuardianAccessLink {

        using CommunicationStructures = Sensor::Guardian::Structures;
        using Factory = SensorAccessLinkFactory<CommunicationStructures>;
        using AccessLink = typename Factory::AccessLink;
        using GatewayStructures = typename Factory::GatewayStructures;

        using ServerCommunicationStrategy = AccessLink::ServerCommunicationStrategy;
        using DataTranslationStrategy = DataTranslation::GuardianTranslationStrategy;
        using SensorCommunicationStrategy = SensorCommunication::GuardianUSBCommunicationStrategy;

        SensorCommunication::USBConnectionParameters sensorConnectionParameters{0x058b, 0x0050,
                                                                                  (129),
                                                                                  (2),
                                                                                  3000};
    };
}

#endif //SENSORGATEWAY_GUARDIANACCESSLINK_H
