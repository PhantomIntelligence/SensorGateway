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

#ifndef SENSORGATEWAY_SPIRITRAWDATA_HPP
#define SENSORGATEWAY_SPIRITRAWDATA_HPP

#include "sensor-gateway/common/data-structure/DataStructures.h"

namespace DataFlow {

    template<typename RawDataDefinition>
    class SpiritRawData {
    public:
        using Content = typename RawDataDefinition::Data;

        explicit SpiritRawData(Content content) :
                content(std::move(content)) {}

        explicit SpiritRawData() :
                SpiritRawData(SpiritRawData::returnDefaultData()) {}

        ~SpiritRawData() noexcept = default;

        SpiritRawData(SpiritRawData const& other) :
                SpiritRawData(other.content) {}

        SpiritRawData(SpiritRawData&& other) noexcept :
                content(std::move(other.content)) {
        }

        SpiritRawData& operator=(SpiritRawData const& other)& {
            SpiritRawData temporary(other);
            swap(*this, temporary);
            return *this;
        }

        SpiritRawData& operator=(SpiritRawData&& other)& noexcept {
            swap(*this, other);
            return *this;
        }

        void swap(SpiritRawData& current, SpiritRawData& other) noexcept {
            std::swap(current.content, other.content);
        }

        bool operator==(SpiritRawData const other) const {
            auto sameContent = (content == other.content);
            auto messagesAreEqual = (sameContent);
            return messagesAreEqual;
        }

        bool operator!=(SpiritRawData const& other) const {
            return !operator==(other);
        }

        static SpiritRawData const& returnDefaultData() noexcept;

        Content content;
    };

    namespace Defaults {
        using DataFlow::SpiritRawData;

        template<typename RawDataDefinition>
        typename RawDataDefinition::Data const DEFAULT_SPIRIT_RAW_DATA_CONTENT{};

        template<typename RawDataDefinition>
        SpiritRawData<RawDataDefinition> const DEFAULT_SPIRIT_RAW_DATA = SpiritRawData<RawDataDefinition>(
                DEFAULT_SPIRIT_RAW_DATA_CONTENT<RawDataDefinition>);
    }

    template<typename RawDataDefinition>
    SpiritRawData<RawDataDefinition> const& SpiritRawData<RawDataDefinition>::returnDefaultData() noexcept {
        return Defaults::DEFAULT_SPIRIT_RAW_DATA<RawDataDefinition>;
    }
}

#endif //SENSORGATEWAY_SPIRITRAWDATA_HPP
