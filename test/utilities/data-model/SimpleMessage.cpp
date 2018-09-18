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

#include "SimpleMessage.h"

using DataModel::SimpleMessage;
using DataModel::Defaults::DEFAULT_SIMPLE_MESSAGE_CONTENT;

SimpleMessage::SimpleMessage(Content simpleDataContent) :
        content(std::move(simpleDataContent)) {}

SimpleMessage::SimpleMessage() : SimpleMessage(SimpleMessage::returnDefaultData()) {}

SimpleMessage::SimpleMessage(SimpleMessage&& other) noexcept:
        content(std::move(other.content)) {
    other.content = DEFAULT_SIMPLE_MESSAGE_CONTENT;
}

SimpleMessage& SimpleMessage::operator=(SimpleMessage const& other)& {
    SimpleMessage temporary(other);
    swap(*this, temporary);
    return *this;
}

SimpleMessage& SimpleMessage::operator=(SimpleMessage&& other)& noexcept {
    swap(*this, other);
    return *this;
}

void SimpleMessage::swap(SimpleMessage& current, SimpleMessage& other) noexcept {
    std::swap(current.content, other.content);
}

bool SimpleMessage::operator==(SimpleMessage const& other) const {
    bool sameContent = true;
    for (auto contentIndex = 0U; contentIndex < content.size() && sameContent; ++contentIndex) {
        sameContent = (content.at(contentIndex) == other.content.at(contentIndex));
    }
    return sameContent;
}

bool SimpleMessage::operator!=(SimpleMessage const& other) const {
    return !operator==(other);
}

void SimpleMessage::inverseContent() {
    auto oldContent = Content(content);
    auto size = content.size();
    if (size < 2) {
        throw std::runtime_error("Warning! Some tests may misbehave. Content size needs to be more than 2.");
    }
    for (unsigned long i = 0; i < content.size(); ++i) {
        content.at(i) = std::move(oldContent.at(size - 1 - i));
    }
}

bool SimpleMessage::isTheInverseOf(SimpleMessage const& other) const {
    auto inversedOther = SimpleMessage(other);
    inversedOther.inverseContent();
    return operator==(inversedOther);
}

std::string SimpleMessage::toString() const noexcept {
    std::string stringifiedContent;
    for (unsigned long i = 0; i < content.size(); ++i) {
        stringifiedContent += " " + content.at(i);
    }
    return stringifiedContent;
}

const SimpleMessage SimpleMessage::returnDefaultData() noexcept {
    return Defaults::DEFAULT_SIMPLE_MESSAGE;
}


