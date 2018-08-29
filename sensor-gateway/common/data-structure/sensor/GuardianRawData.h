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

#ifndef SENSORGATEWAY_GUARDIANRAWDATA_H
#define SENSORGATEWAY_GUARDIANRAWDATA_H

#include "sensor-gateway/common/data-structure/DataStructures.h"

namespace DataFlow {

    class GuardianRawData {
    public:

        std::array<uint8_t, 16> const CHANNEL_POSITIONS = {
                8, 0, 9, 1, 10, 2, 11, 3, 12, 4, 13, 5, 14, 6, 15, 7
        };

        typedef typename
        Sensor::RawDataContent<
                Sensor::RawDataTypes::GUARDIAN,
                Sensor::Guardian::NUMBER_OF_CHANNELS,
                Sensor::Guardian::RAW_DATA_SAMPLING_LENGTH
        > RawDataContent;

        using Content = RawDataContent::Data;

        explicit GuardianRawData(Content content) noexcept;

        explicit GuardianRawData() noexcept;

        ~GuardianRawData() noexcept;

        GuardianRawData(GuardianRawData const& other);

        GuardianRawData(GuardianRawData&& other) noexcept;

        GuardianRawData& operator=(GuardianRawData const& other)&;

        GuardianRawData& operator=(GuardianRawData&& other)& noexcept;

        void swap(GuardianRawData& current, GuardianRawData& other) noexcept;

        bool operator==(GuardianRawData const& other) const;

        bool operator!=(GuardianRawData const& other) const;

        static GuardianRawData const& returnDefaultData() noexcept;

        Content content;
    };

    namespace Defaults {
        using DataFlow::GuardianRawData;
        GuardianRawData::Content const DEFAULT_TBD_SENSOR_NAME_RAW_DATA_CONTENT{};
        GuardianRawData const DEFAULT_TBD_SENSOR_NAME_RAW_DATA = GuardianRawData(
                DEFAULT_TBD_SENSOR_NAME_RAW_DATA_CONTENT);
    }
}

#endif //SENSORGATEWAY_GUARDIANRAWDATA_H
