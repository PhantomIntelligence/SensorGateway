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

#include "AWLMessage.h"

using DataFlow::AWLMessage;

AWLMessage::AWLMessage(int64_t id, uint64_t timestamp, uint32_t length, AWL::DataArray data) :
        id(id),
        timestamp(timestamp),
        length(length),
        data(data) {
}

AWLMessage::AWLMessage(AWLMessage const& other) :
        AWLMessage(other.id, other.timestamp, other.length, other.data) {
}

AWLMessage::AWLMessage(AWLMessage&& other) noexcept :
        id(std::move(other.id)),
        timestamp(std::move(other.timestamp)),
        length(std::move(other.length)),
        data(std::move(other.data)) {

}

AWLMessage& AWLMessage::operator=(AWLMessage const& other)& {
    AWLMessage temporary(std::move(other));
    swap(*this, temporary);
    return *this;
}

AWLMessage& AWLMessage::operator=(AWLMessage&& other)& noexcept {
    swap(*this, other);
    return *this;
}

void AWLMessage::swap(AWLMessage& current, AWLMessage& other) noexcept {
    std::swap(current.id, other.id);
    std::swap(current.length, other.length);
    std::swap(current.timestamp, other.timestamp);
    std::swap(current.data, other.data);
}

bool DataFlow::AWLMessage::operator == (AWLMessage const& other) const  {
    auto sameId = (id == other.id);
    auto sameTimestamp = (timestamp == other.timestamp);
    auto sameLength = (length == other.length);
    auto sameData = (data == other.data);
    auto messagesAreEqual = (sameId && sameTimestamp && sameLength && sameData);
    return messagesAreEqual;
}

bool DataFlow::AWLMessage::operator != (AWLMessage const& other) const  {
    return !operator==(other);
}

AWLMessage const& AWLMessage::returnDefaultData() noexcept {
    return Defaults::DEFAULT_AWL_MESSAGE;
}
