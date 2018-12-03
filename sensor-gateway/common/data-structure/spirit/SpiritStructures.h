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

#ifndef SENSORGATEWAY_SPIRITSTRUCTURES_H
#define SENSORGATEWAY_SPIRITSTRUCTURES_H

#include "sensor-gateway/common/data-structure/DataStructures.h"
#include "SensorMessage.hpp"

namespace Sensor {
    namespace Spirit {

        template<typename SensorMessageDefinition, typename RawDataDefinition, typename CommandDefinition>
        class Structures final : public Communication::DataStructures {

        public :

            enum DataType : uint32_t {
//            [0, 499] -> reserved for SensorGateway messages


//            [500, +] -> reserved for Sensor message

                SENSOR_MESSAGE = 500,
                SENSOR_RAW_DATA = 501
            };

            typedef typename DataFlow::SensorMessage<SensorMessageDefinition> Message;
            typedef typename DataFlow::RawData<RawDataDefinition> RawData;
            typedef typename DataFlow::Command<CommandDefinition> Command;

            static size_t const MAX_NUMBER_OF_BULK_FETCHABLE_MESSAGES = 32;
            static size_t const MAX_NUMBER_OF_BULK_FETCHABLE_RAW_DATA_CYCLES = 8;
        };
    }
}

#endif //SENSORGATEWAY_SPIRITSTRUCTURES_H
