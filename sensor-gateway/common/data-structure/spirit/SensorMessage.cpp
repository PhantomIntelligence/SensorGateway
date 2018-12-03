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

#include "SensorMessage.h"

using DataFlow::SensorMessage;
using DataFlow::MessageId;
using Defaults::SensorMessage::DEFAULT_SENSOR_MESSAGE;
using Defaults::SensorMessage::DEFAULT_MESSAGE_ID;
using Defaults::SensorMessage::DEFAULT_PIXELS_ARRAY;
using Defaults::SensorMessage::DEFAULT_SENSOR_ID;
using DataFlow::PixelsArray;
using DataFlow::SensorId;

SensorMessage::SensorMessage(MessageId messageId, SensorId sensorId, PixelsArray pixels) :
        messageId(messageId), sensorId(sensorId), pixels(std::move(pixels)) {};

SensorMessage::SensorMessage() : SensorMessage(SensorMessage::returnDefaultData()) {};

SensorMessage::SensorMessage(SensorMessage const& other) :
        SensorMessage(other.messageId, other.sensorId, other.pixels) {};

SensorMessage::SensorMessage(SensorMessage&& other) noexcept:
        messageId(other.messageId),
        sensorId(other.sensorId),
        pixels(other.pixels) {
    other.messageId = DEFAULT_MESSAGE_ID;
    other.sensorId = DEFAULT_SENSOR_ID;
    other.pixels = DEFAULT_PIXELS_ARRAY;
};

SensorMessage& SensorMessage::operator=(SensorMessage const& other)& {
    SensorMessage temporary(other);
    swap(*this, temporary);
    return *this;
};

SensorMessage& SensorMessage::operator=(SensorMessage&& other)& noexcept {
    swap(*this, other);
    return *this;
};

void SensorMessage::swap(SensorMessage& current, SensorMessage& other) noexcept {
    std::swap(current.messageId, other.messageId);
    std::swap(current.sensorId, other.sensorId);
    std::swap(current.pixels, other.pixels);
};

bool SensorMessage::operator==(SensorMessage const& other) const {
    auto sameSensorMessageId = (messageId == other.messageId);
    auto sameSensorId = (sensorId == other.sensorId);
    auto samePixels = (pixels == other.pixels);
    auto sensorMessagesAreEqual = (sameSensorMessageId && sameSensorId && samePixels);
    return sensorMessagesAreEqual;
}

bool SensorMessage::operator!=(SensorMessage const& other) const {
    return !(operator==(other));
}

SensorMessage const& SensorMessage::returnDefaultData() noexcept {
    return DEFAULT_SENSOR_MESSAGE;
}

void SensorMessage::addTrackToPixelWithId(PixelId const& pixelId, Track&& trackToAdd) {
    updatePixelId(pixelId);
    pixels[pixelId].addTrack(std::forward<Track>(trackToAdd));
}

std::string const SensorMessage::getSensorIdentifier() const noexcept {
    std::ostringstream sensorIdentifierStream;
    sensorIdentifierStream << sensorId;
    std::string sensorIdentifier = sensorIdentifierStream.str();
    return sensorIdentifier;
}

PixelsArray* SensorMessage::getPixels() {
    return &pixels;
}

void SensorMessage::updatePixelId(PixelId const& pixelId) {
    pixels[pixelId].id = pixelId;
}
