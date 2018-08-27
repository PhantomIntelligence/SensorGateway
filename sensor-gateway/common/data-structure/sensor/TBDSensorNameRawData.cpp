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

#include "TBDSensorNameRawData.h"

using DataFlow::TBDSensorNameRawData;

TBDSensorNameRawData::TBDSensorNameRawData(TBDSensorNameRawData::Content content) noexcept:
        content(content) {}

TBDSensorNameRawData::TBDSensorNameRawData() noexcept :
        TBDSensorNameRawData(TBDSensorNameRawData::returnDefaultData()) {}

TBDSensorNameRawData::~TBDSensorNameRawData() noexcept = default;

TBDSensorNameRawData::TBDSensorNameRawData(TBDSensorNameRawData const& other) :
        TBDSensorNameRawData(other.content) {}

TBDSensorNameRawData::TBDSensorNameRawData(TBDSensorNameRawData&& other) noexcept :
        content(other.content) {}

TBDSensorNameRawData&
TBDSensorNameRawData::operator=(TBDSensorNameRawData const& other)& {
    TBDSensorNameRawData temporary(other);
    swap(*this, temporary);
    return *this;
}

TBDSensorNameRawData&
TBDSensorNameRawData::operator=(TBDSensorNameRawData&& other)& noexcept {
    swap(*this, other);
    return *this;
}

void TBDSensorNameRawData::swap(TBDSensorNameRawData& current, TBDSensorNameRawData& other) noexcept {
    std::swap(current.content, other.content);
}

bool DataFlow::TBDSensorNameRawData::operator==(TBDSensorNameRawData const& other) const {
    auto sameContent = (content == other.content);
    auto messagesAreEqual = (sameContent);
    return messagesAreEqual;
}

bool DataFlow::TBDSensorNameRawData::operator!=(TBDSensorNameRawData const& other) const {
    return !operator==(other);
}

TBDSensorNameRawData const& TBDSensorNameRawData::returnDefaultData() noexcept {
    return Defaults::DEFAULT_TBD_SENSOR_NAME_RAW_DATA;
}
