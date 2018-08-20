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

#ifndef SENSORGATEWAY_TBDSENSORNAMERAWDATA_H
#define SENSORGATEWAY_TBDSENSORNAMERAWDATA_H

#include "AWLMessage.h"

namespace DataFlow {

    namespace Guardian {
        using AquisitionBuffer = int16_t[Sensor::Guardian::ACQUISITION_BUFFER_SIZE];
        using Detections = RawData::DetectionType[Sensor::Guardian::DETECTIONS_SIZE];
        typedef struct {
            AquisitionBuffer aquisitionBuffer;
            Detections detections;
        } RawDataBuffer;
    }

    class TBDSensorNameRawData {
    protected:

    public:

        explicit TBDSensorNameRawData(Guardian::RawDataBuffer);

        explicit TBDSensorNameRawData();

        ~TBDSensorNameRawData() noexcept;

        TBDSensorNameRawData(TBDSensorNameRawData const& other);

        TBDSensorNameRawData(TBDSensorNameRawData&& other) noexcept;

        TBDSensorNameRawData& operator=(TBDSensorNameRawData const& other)&;

        TBDSensorNameRawData& operator=(TBDSensorNameRawData&& other)& noexcept;

        void swap(TBDSensorNameRawData& current, TBDSensorNameRawData& other) noexcept;
    };

}

#endif //SENSORGATEWAY_TBDSENSORNAMERAWDATA_H
