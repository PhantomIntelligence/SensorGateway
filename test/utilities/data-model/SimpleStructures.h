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

#ifndef SENSORGATEWAY_SIMPLESTRUCTURES_H
#define SENSORGATEWAY_SIMPLESTRUCTURES_H

#include "SimpleData.h"
#include "sensor-gateway/common/sensor-structures/AbstractStructures.h"

namespace Sensor {
    namespace Test {
        namespace Simple {
            class Structures final : public Sensor::Communication::AbstractStructures {
            public :
                typedef typename DataModel::SimpleData Message;
                typedef typename DataModel::SimpleData RawData;

                static size_t const MAX_NUMBER_OF_BULK_FETCHABLE_MESSAGES = 1;
                static size_t const MAX_NUMBER_OF_BULK_FETCHABLE_RAW_DATA_CYCLES = 1;
            };
        }
    }
}

#endif //SENSORGATEWAY_SIMPLESTRUCTURES_H
