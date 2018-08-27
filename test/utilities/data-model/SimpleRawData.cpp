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

#include "SimpleRawData.h"

using DataModel::SimpleRawData;
using DataModel::Defaults::DEFAULT_SIMPLE_RAW_DATA_CONTENT;

SimpleRawData::SimpleRawData(RawDataContent::Data content) noexcept :
        content(content) {}

SimpleRawData::SimpleRawData() noexcept: SimpleRawData(SimpleRawData::returnDefaultData()) {
}

SimpleRawData::~SimpleRawData() noexcept = default;

SimpleRawData::SimpleRawData(SimpleRawData&& other) noexcept :
        content(other.content) {
    other.content = DEFAULT_SIMPLE_RAW_DATA_CONTENT;
}

SimpleRawData& SimpleRawData::operator=(SimpleRawData const& other)& {
    SimpleRawData temporary(other);
    swap(*this, temporary);
    return *this;
}

SimpleRawData& SimpleRawData::operator=(SimpleRawData&& other)& noexcept {
    swap(*this, other);
    return *this;
}

void SimpleRawData::swap(SimpleRawData& current, SimpleRawData& other) noexcept {
    std::swap(current.content, other.content);
}

bool SimpleRawData::operator==(SimpleRawData const& other) const {
    bool sameContent = true;
    for (auto contentIndex = 0U; contentIndex < content.size() && sameContent; ++contentIndex) {
        sameContent = (content.at(contentIndex) == other.content.at(contentIndex));
    }
    return sameContent;
}

bool SimpleRawData::operator!=(SimpleRawData const& other) const {
    return !operator==(other);
}

void SimpleRawData::inverseContent() {
    auto oldContent = RawDataContent::Data(content);
    auto size = content.size();
    if (size < 2) {
        throw std::runtime_error("Warning! Some tests may misbehave. Content size needs to be more than 2.");
    }
    for (unsigned long i = 0; i < content.size(); ++i) {
        content.at(i) = oldContent.at(size - 1 - i);
    }
}

bool SimpleRawData::isTheInverseOf(SimpleRawData const& other) const {
    auto inversedRawData = SimpleRawData(other);
    inversedRawData.inverseContent();
    return operator==(inversedRawData);
}

std::string SimpleRawData::toString() const noexcept {
    std::string stringifiedContent;
    for (auto& contentUnit: content) {
        stringifiedContent += " " + std::to_string(contentUnit);
    }
    return stringifiedContent;
}

SimpleRawData const SimpleRawData::returnDefaultData() noexcept {
    return Defaults::DEFAULT_SIMPLE_RAW_DATA;
}
