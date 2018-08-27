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

#include "AWLRawData.h"

using DataFlow::AWLRawData;

AWLRawData::AWLRawData(AWLRawData::Content content) noexcept:
        content(content) {}

AWLRawData::AWLRawData() noexcept :
        AWLRawData(AWLRawData::returnDefaultData()) {}

AWLRawData::~AWLRawData() noexcept = default;

AWLRawData::AWLRawData(AWLRawData const& other) :
        AWLRawData(other.content) {}

AWLRawData::AWLRawData(AWLRawData&& other) noexcept :
        content(other.content) {}

AWLRawData& AWLRawData::operator=(AWLRawData const& other)& {
    AWLRawData temporary(other);
    swap(*this, temporary);
    return *this;
}

AWLRawData& AWLRawData::operator=(AWLRawData&& other)& noexcept {
    swap(*this, other);
    return *this;
}

void AWLRawData::swap(AWLRawData& current, AWLRawData& other) noexcept {
    std::swap(current.content, other.content);
}

bool DataFlow::AWLRawData::operator==(AWLRawData const& other) const {
    auto sameContent = (content == other.content);
    auto messagesAreEqual = (sameContent);
    return messagesAreEqual;
}

bool DataFlow::AWLRawData::operator!=(AWLRawData const& other) const {
    return !operator==(other);
}

AWLRawData const& AWLRawData::returnDefaultData() noexcept {
    return Defaults::DEFAULT_AWL_RAW_DATA;
}
