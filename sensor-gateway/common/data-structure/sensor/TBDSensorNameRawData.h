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

#include "sensor-gateway/common/data-structure/DataStructures.h"

namespace DataFlow {

    class TBDSensorNameRawData {
    public:

        using Content = Sensor::RawDataContent<Sensor::RawDataTypes::GUARDIAN, Sensor::Guardian::RAW_DATA_ACQUISITION_BUFFER_SIZE>::Data;

        explicit TBDSensorNameRawData(Content content) noexcept;

        explicit TBDSensorNameRawData() noexcept;

        ~TBDSensorNameRawData() noexcept;

        TBDSensorNameRawData(TBDSensorNameRawData const& other);

        TBDSensorNameRawData(TBDSensorNameRawData&& other) noexcept;

        TBDSensorNameRawData& operator=(TBDSensorNameRawData const& other)&;

        TBDSensorNameRawData& operator=(TBDSensorNameRawData&& other)& noexcept;

        void swap(TBDSensorNameRawData& current, TBDSensorNameRawData& other) noexcept;

        bool operator==(TBDSensorNameRawData const& other) const;

        bool operator!=(TBDSensorNameRawData const& other) const;

        static TBDSensorNameRawData const& returnDefaultData() noexcept;

        Content content;
    };

    namespace Defaults {
        using DataFlow::TBDSensorNameRawData;
        TBDSensorNameRawData::Content const DEFAULT_TBD_SENSOR_NAME_RAW_DATA_CONTENT{};
        TBDSensorNameRawData const DEFAULT_TBD_SENSOR_NAME_RAW_DATA = TBDSensorNameRawData(DEFAULT_TBD_SENSOR_NAME_RAW_DATA_CONTENT);
    }
}

#endif //SENSORGATEWAY_TBDSENSORNAMERAWDATA_H
