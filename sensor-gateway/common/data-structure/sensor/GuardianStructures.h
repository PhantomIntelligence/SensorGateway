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

#ifndef SENSORGATEWAY_GUARDIANSTRUCTURES_H
#define SENSORGATEWAY_GUARDIANSTRUCTURES_H

#include "sensor-gateway/common/data-structure/DataStructures.h"
#include "AWLMessage.h"

namespace Sensor {
    namespace Guardian {

        class Structures final : public Communication::DataStructures {

        public :

            typedef typename
            Sensor::SensorMessageDefinition<
                    Guardian::NUMBER_OF_PIXELS
            > GuardianMessageDefinition;

            typedef typename
            Sensor::RawDataDefinition<
                    RawDataTypes::GUARDIAN,
                    Guardian::NUMBER_OF_CHANNELS,
                    Guardian::RAW_DATA_SAMPLING_LENGTH
            > GuardianRawDataDefinition;

            typedef typename
            Sensor::CommandDefinition<
                    CommandPayloadTypes::GUARDIAN,
                    Guardian::MAX_COMMAND_PAYLOAD_SIZE
            > GuardianCommandDefinition;

            typedef typename DataFlow::AWLMessage Message;
            typedef typename DataFlow::RawData<GuardianRawDataDefinition> RawData;
            typedef typename DataFlow::Command<GuardianCommandDefinition> Command;

            static size_t const MAX_NUMBER_OF_BULK_FETCHABLE_MESSAGES = 32;
            static size_t const MAX_NUMBER_OF_BULK_FETCHABLE_RAW_DATA_CYCLES = 8;

            static std::array<RawDataTypes::GUARDIAN,
                    Guardian::NUMBER_OF_CHANNELS> constexpr CHANNEL_POSITIONS = {
                    8, 0, 9, 1, 10, 2, 11, 3, 12, 4, 13, 5, 14, 6, 15, 7
            };
        };
    }
}

#endif //SENSORGATEWAY_GUARDIANSTRUCTURES_H
