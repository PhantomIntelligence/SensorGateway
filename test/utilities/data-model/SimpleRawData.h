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

#ifndef SENSORGATEWAY_SIMPLERAWDATA_H
#define SENSORGATEWAY_SIMPLERAWDATA_H

#include "sensor-gateway/common/data-structure/DataStructures.h"
#include "SimpleMessage.h"

namespace DataModel {

    class SimpleRawData {
    protected:
        static size_t const NUMBER_OF_CHANNELS = 1;
        static size_t const NUMBER_OF_DATA_IN_RAW_DATA = 16;
    public:

        typedef typename Sensor::RawDataDefinition<
                uint32_t,
                NUMBER_OF_CHANNELS,
                NUMBER_OF_DATA_IN_RAW_DATA
        > RawDataDefinition;

        explicit SimpleRawData(RawDataDefinition::Data content) noexcept;

        explicit SimpleRawData() noexcept;

        ~SimpleRawData() noexcept;

        SimpleRawData(SimpleRawData const& other) = default;

        SimpleRawData(SimpleRawData&& other) noexcept;

        SimpleRawData& operator=(SimpleRawData const& other)&;

        SimpleRawData& operator=(SimpleRawData&& other)& noexcept;

        void swap(SimpleRawData& current, SimpleRawData& other) noexcept;

        bool operator==(SimpleRawData const& other) const;

        bool operator!=(SimpleRawData const& other) const;

        void inverseContent();

        bool isTheInverseOf(SimpleRawData const& other) const;

        std::string toString() const noexcept;

        SimpleRawData static const returnDefaultData() noexcept;

    private:

        RawDataDefinition::Data content;
    };

    namespace Defaults {
        using DataModel::SimpleRawData;

        SimpleRawData::RawDataDefinition::Data const DEFAULT_SIMPLE_RAW_DATA_CONTENT = SimpleRawData::RawDataDefinition::Data();
        SimpleRawData const DEFAULT_SIMPLE_RAW_DATA = SimpleRawData(DEFAULT_SIMPLE_RAW_DATA_CONTENT);
    }
}

#endif //SENSORGATEWAY_SIMPLERAWDATA_H
