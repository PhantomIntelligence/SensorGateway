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
        using AquisitionBuffer = std::array<int16_t, Sensor::Guardian::ACQUISITION_BUFFER_SIZE>;
        using Detections = std::array<RawData::DetectionType, Sensor::Guardian::DETECTIONS_SIZE>;
        typedef struct {
            AquisitionBuffer acquisitionBuffer;
            Detections detections;
        } RawDataBuffer;
    }

    class TBDSensorNameRawData {
    protected:

    public:

        explicit TBDSensorNameRawData(Guardian::RawDataBuffer rawDataBuffer);

        explicit TBDSensorNameRawData();

        ~TBDSensorNameRawData() noexcept;

        TBDSensorNameRawData(TBDSensorNameRawData const& other);

        TBDSensorNameRawData(TBDSensorNameRawData&& other) noexcept;

        TBDSensorNameRawData& operator=(TBDSensorNameRawData const& other)&;

        TBDSensorNameRawData& operator=(TBDSensorNameRawData&& other)& noexcept;

        void swap(TBDSensorNameRawData& current, TBDSensorNameRawData& other) noexcept;

        static TBDSensorNameRawData const& returnDefaultData() noexcept;

        Guardian::RawDataBuffer rawDataBuffer;
    };

    namespace Defaults {
        using DataFlow::TBDSensorNameRawData;
        Guardian::AquisitionBuffer const DEFAULT_ACQUISITION_BUFFER = Guardian::AquisitionBuffer();
        Guardian::Detections const DEFAULT_DETECTIONS = Guardian::Detections();
        Guardian::RawDataBuffer const DEFAULT_RAW_DATA_BUFFER{DEFAULT_ACQUISITION_BUFFER, DEFAULT_DETECTIONS};
        TBDSensorNameRawData const DEFAULT_TBD_SENSOR_NAME_RAW_DATA = TBDSensorNameRawData(DEFAULT_RAW_DATA_BUFFER);
    }
}

#endif //SENSORGATEWAY_TBDSENSORNAMERAWDATA_H
