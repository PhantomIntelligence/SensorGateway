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

#ifndef SENSORGATEWAY_TBDSENSORNAMESTRUCTURES_H
#define SENSORGATEWAY_TBDSENSORNAMESTRUCTURES_H

#include "TBDSensorNameRawData.h"
#include "AbstractStructures.h"

namespace Sensor {
    namespace TBDSensorName {
        class Structures final : public Communication::AbstractStructures {
        public :

            typedef typename DataFlow::AWLMessage Message;
            typedef typename DataFlow::TBDSensorNameRawData RawData;

            static std::size_t const MAX_NUMBER_OF_BULK_FETCHABLE_MESSAGES = 32;
//            typedef struct
//            {
//                int16_t AcqFifo[GUARDIAN_NUM_CHANNEL * GUARDIAN_SAMPLING_LENGTH];
//                sizeof(AcqFifo) = (int16_t)*16*100
//                detection_type detections[GUARDIAN_NUM_CHANNEL * GUARDIAN_NUM_DET_PER_CH];
//                detection_type {float distance, float intensity}, float => 32 bits
//            } tDataFifo;
            static size_t const MAX_NUMBER_OF_BULK_FETCHABLE_RAW_DATA_CYCLES = 32; // todo: update this for the correct value;
        };
    }
}

#endif //SENSORGATEWAY_TBDSENSORNAMESTRUCTURES_H
