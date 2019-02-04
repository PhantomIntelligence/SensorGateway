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

#ifndef SENSORGATEWAY_GUARDIANACCESSLINK_H
#define SENSORGATEWAY_GUARDIANACCESSLINK_H

#include "SensorAccessLink.hpp"

#include "sensor-gateway/sensor-communication/GuardianUSBCommunicationStrategy.h"
#include "sensor-gateway/data-translation/GuardianTranslationStrategy.h"

namespace SensorGateway {

    using GuardianStructures = Sensor::Guardian::Structures;
    using GuardianGatewayStructures = Sensor::Gateway::Structures<
            GuardianStructures::GuardianMessageDefinition,
            GuardianStructures::GuardianRawDataDefinition,
            GuardianStructures::GuardianCommandDefinition>;

    class GuardianAccessLink final : public SensorAccessLink<GuardianStructures, GuardianGatewayStructures> {

    protected:

        using super = SensorAccessLink<GuardianStructures, GuardianGatewayStructures>;

        using super::ServerCommunicationStrategy;

    public:

        explicit GuardianAccessLink(ServerCommunicationStrategy* serverCommunicationStrategy)
                : super(serverCommunicationStrategy,
                        &guardianTranslationStrategy,
                        &guardianUSBCommunicationStrategy) {}

        ~GuardianAccessLink() noexcept = default;

        GuardianAccessLink(GuardianAccessLink const& other) = delete;

        GuardianAccessLink(GuardianAccessLink&& other) noexcept = delete;

        GuardianAccessLink& operator=(GuardianAccessLink const& other)& = delete;

        GuardianAccessLink& operator=(GuardianAccessLink&& other)& noexcept = delete;

    private:

        DataTranslation::GuardianTranslationStrategy guardianTranslationStrategy;
        SensorCommunication::GuardianUSBCommunicationStrategy guardianUSBCommunicationStrategy;
    };
}

#endif //SENSORGATEWAY_GUARDIANACCESSLINK_H
