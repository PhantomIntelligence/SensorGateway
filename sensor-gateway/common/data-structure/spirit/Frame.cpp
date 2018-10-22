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

#include "Frame.h"

using DataFlow::Frame;
using DataFlow::MessageId;
using Defaults::Frame::DEFAULT_FRAME;
using Defaults::Frame::DEFAULT_MESSAGE_ID;
using Defaults::Frame::DEFAULT_PIXELS_ARRAY;
using Defaults::Frame::DEFAULT_SENSOR_ID;
using DataFlow::PixelsArray;
using DataFlow::SensorId;

Frame::Frame(MessageId frameId, SensorId sensorId, PixelsArray pixels) :
        messageId(frameId), sensorId(sensorId), pixels(std::move(pixels)) {};

Frame::Frame() : Frame(Frame::returnDefaultData()) {};

Frame::Frame(Frame const& other) :
        Frame(other.messageId, other.sensorId, other.pixels) {};

Frame::Frame(Frame&& other) noexcept:
        messageId(other.messageId),
        sensorId(other.sensorId),
        pixels(other.pixels) {
    other.messageId = DEFAULT_MESSAGE_ID;
    other.sensorId = DEFAULT_SENSOR_ID;
    other.pixels = DEFAULT_PIXELS_ARRAY;
};

Frame& Frame::operator=(Frame const& other)& {
    Frame temporary(other);
    swap(*this, temporary);
    return *this;
};

Frame& Frame::operator=(Frame&& other)& noexcept {
    swap(*this, other);
    return *this;
};

void Frame::swap(Frame& current, Frame& other) noexcept {
    std::swap(current.messageId, other.messageId);
    std::swap(current.sensorId, other.sensorId);
    std::swap(current.pixels, other.pixels);
};

bool Frame::operator==(Frame const& other) const {
    auto sameFrameId = (messageId == other.messageId);
    auto sameSystemId = (sensorId == other.sensorId);
    auto samePixels = (pixels == other.pixels);
    auto framesAreEqual = (sameFrameId && sameSystemId && samePixels);
    return framesAreEqual;
}

bool Frame::operator!=(Frame const& other) const {
    return !(operator==(other));
}

Frame const& Frame::returnDefaultData() noexcept {
    return DEFAULT_FRAME;
}

void Frame::addTrackToPixelWithId(PixelId const& pixelId, Track&& trackToAdd) {
    updatePixelId(pixelId);
    pixels[pixelId].addTrack(std::forward<Track>(trackToAdd));
}

std::string const Frame::getSensorIdentifier() const noexcept {
    std::ostringstream sensorIdentifierStream;
    sensorIdentifierStream << "(" << sensorId << ")";
    std::string sensorIdentifier = sensorIdentifierStream.str();
    return sensorIdentifier;
}

PixelsArray* Frame::getPixels() {
    return &pixels;
}

void Frame::updatePixelId(PixelId const& pixelId) {
    pixels[pixelId].id = pixelId;
}
