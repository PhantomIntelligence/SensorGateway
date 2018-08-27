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

#include "GuardianRawData.h"

using DataFlow::GuardianRawData;

GuardianRawData::GuardianRawData(GuardianRawData::Content content) noexcept:
        content(content) {}

GuardianRawData::GuardianRawData() noexcept :
        GuardianRawData(GuardianRawData::returnDefaultData()) {}

GuardianRawData::~GuardianRawData() noexcept = default;

GuardianRawData::GuardianRawData(GuardianRawData const& other) :
        GuardianRawData(other.content) {}

GuardianRawData::GuardianRawData(GuardianRawData&& other) noexcept :
        content(other.content) {}

GuardianRawData&
GuardianRawData::operator=(GuardianRawData const& other)& {
    GuardianRawData temporary(other);
    swap(*this, temporary);
    return *this;
}

GuardianRawData&
GuardianRawData::operator=(GuardianRawData&& other)& noexcept {
    swap(*this, other);
    return *this;
}

void GuardianRawData::swap(GuardianRawData& current, GuardianRawData& other) noexcept {
    std::swap(current.content, other.content);
}

bool DataFlow::GuardianRawData::operator==(GuardianRawData const& other) const {
    auto sameContent = (content == other.content);
    auto messagesAreEqual = (sameContent);
    return messagesAreEqual;
}

bool DataFlow::GuardianRawData::operator!=(GuardianRawData const& other) const {
    return !operator==(other);
}

GuardianRawData const& GuardianRawData::returnDefaultData() noexcept {
    return Defaults::DEFAULT_TBD_SENSOR_NAME_RAW_DATA;
}
