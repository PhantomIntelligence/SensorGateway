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

#include "SimpleData.h"

using DataModel::SimpleData;
using DataModel::SimpleDataContent;
using Defaults::DEFAULT_SIMPLE_DATA_CONTENT;

SimpleData::SimpleData(SimpleDataContent simpleDataContent) :
        content(std::move(simpleDataContent)) {
}


SimpleData::SimpleData(SimpleData&& other) noexcept:
        content(std::move(other.content)) {
    other.content = DEFAULT_SIMPLE_DATA_CONTENT;
}

SimpleData& SimpleData::operator=(SimpleData const& other)& {
    SimpleData temporary(other);
    swap(*this, temporary);
    return *this;
}

SimpleData& SimpleData::operator=(SimpleData&& other)& noexcept {
    swap(*this, other);
    return *this;
}

void SimpleData::swap(SimpleData& current, SimpleData& other) noexcept {
    std::swap(current.content, other.content);
}

bool SimpleData::operator==(SimpleData const& other) const {
    bool sameContent = true;
    for (unsigned long i = 0; i < content.size() && sameContent; ++i) {
        sameContent = content.at(i) == other.content.at(i);
    }
    return sameContent;
}

bool SimpleData::operator!=(SimpleData const& other) const {
    return false;
}

void SimpleData::inverseContent() {
    auto oldContent = SimpleDataContent(content);
    auto size = content.size();
    if (size < 2) {
        throw std::runtime_error("Warning! Some tests may misbehave. Content size needs to be more than 2.");
    }
    for (unsigned long i = 0; i < content.size(); ++i) {
        content.at(i) = std::move(oldContent.at(size - 1 - i));
    }
}

bool SimpleData::isTheInverseOf(SimpleData const& other) const {
    auto inversedOther = SimpleData(other);
    inversedOther.inverseContent();
    return operator==(inversedOther);
}

std::string SimpleData::toString() const {
    std::string stringifiedData;
    for (unsigned long i = 0; i < content.size(); ++i) {
        stringifiedData += " " + content.at(i);
    }
    return stringifiedData;
}

const SimpleData SimpleData::returnDefaultData() noexcept {
    return Defaults::DEFAULT_SIMPLE_DATA;
}


