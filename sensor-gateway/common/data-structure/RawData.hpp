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

#ifndef SENSORGATEWAY_RAWDATA_HPP
#define SENSORGATEWAY_RAWDATA_HPP

#include "Command.hpp"

namespace DataFlow {

    template<typename RawDataDefinition>
    class RawData {

    public:

        using Definitions = RawDataDefinition;
        using Content = typename Definitions::Data;

        explicit RawData(Content content) :
        content(std::move(content)) {}

        explicit RawData() :
                RawData(RawData::returnDefaultData()) {}

        ~RawData() noexcept = default;

        RawData(RawData const& other) :
        RawData(other.content) {}

        RawData(RawData&& other) noexcept :
        content(std::move(other.content)) {
        }

        RawData& operator=(RawData const& other)& {
            RawData temporary(other);
            swap(*this, temporary);
            return *this;
        }

        RawData& operator=(RawData&& other)& noexcept {
            swap(*this, other);
            return *this;
        }

        void swap(RawData& current, RawData& other) noexcept {
            std::swap(current.content, other.content);
        }

        bool operator==(RawData const& other) const {
            auto sameContent = (content == other.content);
            auto messagesAreEqual = (sameContent);
            return messagesAreEqual;
        }

        bool operator!=(RawData const& other) const {
            return !operator==(other);
        }

        static RawData const& returnDefaultData() noexcept;

        Content content;
    };

    namespace Defaults {
        using DataFlow::RawData;

        template<typename RawDataDefinition>
        typename RawDataDefinition::Data const DEFAULT_RAW_DATA_CONTENT{};

        template<typename RawDataDefinition>
        RawData<RawDataDefinition> const DEFAULT_RAW_DATA = RawData<RawDataDefinition>(
                DEFAULT_RAW_DATA_CONTENT<RawDataDefinition>);
    }

    template<typename RawDataDefinition>
    RawData<RawDataDefinition> const& RawData<RawDataDefinition>::returnDefaultData() noexcept {
        return Defaults::DEFAULT_RAW_DATA<RawDataDefinition>;
    }
}

#endif //SENSORGATEWAY_RAWDATA_HPP
