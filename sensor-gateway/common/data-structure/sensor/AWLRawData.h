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

#ifndef SENSORGATEWAY_AWLRAWDATA_H
#define SENSORGATEWAY_AWLRAWDATA_H

#include "sensor-gateway/common/data-structure/DataStructures.h"

namespace DataFlow {

    class AWLRawData {
    public:

        typedef typename
        Sensor::RawDataContent<
                Sensor::RawDataTypes::AWL,
                Sensor::AWL::_16::NUMBER_OF_CHANNELS,
                Sensor::AWL::RAW_DATA_SAMPLING_LENGTH
        > RawDataContent;

        using Content = typename RawDataContent::Data;

        explicit AWLRawData(Content content) noexcept;

        explicit AWLRawData() noexcept;

        ~AWLRawData() noexcept;

        AWLRawData(AWLRawData const& other);

        AWLRawData(AWLRawData&& other) noexcept;

        AWLRawData& operator=(AWLRawData const& other)&;

        AWLRawData& operator=(AWLRawData&& other)& noexcept;

        void swap(AWLRawData& current, AWLRawData& other) noexcept;

        bool operator==(AWLRawData const& other) const;

        bool operator!=(AWLRawData const& other) const;

        static AWLRawData const& returnDefaultData() noexcept;

        Content content;
    };

    namespace Defaults {
        using DataFlow::AWLRawData;
        AWLRawData::Content const DEFAULT_AWL_RAW_DATA_CONTENT{};
        AWLRawData const DEFAULT_AWL_RAW_DATA = AWLRawData(DEFAULT_AWL_RAW_DATA_CONTENT);
    }
}

#endif //SENSORGATEWAY_AWLRAWDATA_H
