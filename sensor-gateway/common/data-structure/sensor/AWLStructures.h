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

#ifndef SENSORGATEWAY_AWLSTRUCTURES_H
#define SENSORGATEWAY_AWLSTRUCTURES_H

#include "sensor-gateway/common/data-structure/DataStructures.h"
#include "AWLMessage.h"

namespace Sensor {
    namespace AWL {

        class Structures final : public Communication::DataStructures {

        public :

            typedef typename
            Sensor::SensorMessageDefinition<
                    AWL::_16::NUMBER_OF_PIXELS
            > AWLMessageDefinition;

            typedef typename
            Sensor::RawDataDefinition<
                    Sensor::RawDataTypes::AWL,
                    Sensor::AWL::_16::NUMBER_OF_CHANNELS,
                    Sensor::AWL::RAW_DATA_SAMPLING_LENGTH
            > AWLRawDataDefinition;

            typedef typename
            Sensor::CommandDefinition<
                    Sensor::CommandPayloadTypes::AWL,
                    Sensor::AWL::MAX_COMMAND_PAYLOAD_SIZE
            > AWLCommandDefinition;

            typedef typename DataFlow::AWLMessage Message;
            typedef typename DataFlow::RawData<AWLRawDataDefinition> RawData;
            typedef typename DataFlow::Command<AWLCommandDefinition> Command;

            static size_t const MAX_NUMBER_OF_BULK_FETCHABLE_MESSAGES = 1;
            static size_t const MAX_NUMBER_OF_BULK_FETCHABLE_RAW_DATA_CYCLES = 1;
        };
    }
}

#endif //SENSORGATEWAY_AWLSTRUCTURES_H
