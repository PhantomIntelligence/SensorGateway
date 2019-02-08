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

#include "SimpleControlMessage.h"

using DataModel::SimpleControlMessage;
using DataModel::Defaults::DEFAULT_SIMPLE_CONTROL_MESSAGE_CONTENT;

SimpleControlMessage::SimpleControlMessage(Content simpleDataContent,
                             ServiceTimestamps const& gatewayTimestamps) :
        content(std::move(simpleDataContent)),
        gatewayTimestamps(gatewayTimestamps) {}

SimpleControlMessage::SimpleControlMessage() : SimpleControlMessage(SimpleControlMessage::returnDefaultData()) {}

SimpleControlMessage::SimpleControlMessage(SimpleControlMessage&& other) noexcept:
        content(std::move(other.content)),
        gatewayTimestamps(std::move(other.gatewayTimestamps)) {
    other.content = DEFAULT_SIMPLE_CONTROL_MESSAGE_CONTENT;
}

SimpleControlMessage& SimpleControlMessage::operator=(SimpleControlMessage const& other)& {
    SimpleControlMessage temporary(other);
    swap(*this, temporary);
    return *this;
}

SimpleControlMessage& SimpleControlMessage::operator=(SimpleControlMessage&& other)& noexcept {
    swap(*this, other);
    return *this;
}

void SimpleControlMessage::swap(SimpleControlMessage& current, SimpleControlMessage& other) noexcept {
    std::swap(current.content, other.content);
    ServiceTimestamps::swap(current.gatewayTimestamps, other.gatewayTimestamps);
}

bool SimpleControlMessage::operator==(SimpleControlMessage const& other) const {
    bool sameContent = true;
    for (auto contentIndex = 0U; contentIndex < content.size() && sameContent; ++contentIndex) {
        sameContent = (content.at(contentIndex) == other.content.at(contentIndex));
    }
    return sameContent;
}

bool SimpleControlMessage::operator!=(SimpleControlMessage const& other) const {
    return !operator==(other);
}

void SimpleControlMessage::inverseContent() {
    auto oldContent = Content(content);
    auto size = content.size();
    if (size < 2) {
        throw std::runtime_error("Warning! Some tests may misbehave. Content size needs to be more than 2.");
    }
    for (unsigned long i = 0; i < content.size(); ++i) {
        content.at(i) = std::move(oldContent.at(size - 1 - i));
    }
}

bool SimpleControlMessage::isTheInverseOf(SimpleControlMessage const& other) const {
    auto inversedOther = SimpleControlMessage(other);
    inversedOther.inverseContent();
    return operator==(inversedOther);
}

std::string SimpleControlMessage::toString() const noexcept {
    std::string stringifiedContent;
    for (unsigned long i = 0; i < content.size(); ++i) {
        stringifiedContent += " " + content.at(i);
    }
    return stringifiedContent;
}

void SimpleControlMessage::addTimePointForGatewayWithLocation(std::string const& locationName) {
    gatewayTimestamps.addTimePointForLocation(locationName);
}

SimpleControlMessage::ServiceTimestamps const& SimpleControlMessage::getGatewayTimestamps() const noexcept {
    return gatewayTimestamps;
}

const SimpleControlMessage SimpleControlMessage::returnDefaultData() noexcept {
    return Defaults::DEFAULT_SIMPLE_CONTROL_MESSAGE;
}


