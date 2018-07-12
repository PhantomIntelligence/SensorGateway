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
using DataFlow::FrameID;
using Defaults::Frame::DEFAULT_FRAME;
using Defaults::Frame::DEFAULT_FRAME_ID;
using Defaults::Frame::DEFAULT_PIXELS_ARRAY;
using Defaults::Frame::DEFAULT_SYSTEM_ID;
using DataFlow::PixelsArray;
using DataFlow::SystemID;

Frame::Frame(FrameID frameID, SystemID systemID, PixelsArray pixels) :
        frameID(frameID), systemID(systemID), pixels(pixels) {};

Frame::Frame() : Frame(Frame::returnDefaultData()) {};

Frame::Frame(Frame const& other) :
        Frame(other.frameID, other.systemID, other.pixels) {};

Frame::Frame(Frame&& other) noexcept:
        frameID(std::move(other.frameID)),
        systemID(std::move(systemID)),
        pixels(std::move(other.pixels)) {};

Frame& Frame::operator=(Frame const& other)& {
    Frame temporary(std::move(other));
    swap(*this, temporary);
    return *this;
};

Frame& Frame::operator=(Frame&& other)& noexcept {
    swap(*this, other);
    return *this;
};

void Frame::swap(Frame& current, Frame& other) noexcept {
    std::swap(current.frameID, other.frameID);
    std::swap(current.systemID, other.systemID);
    std::swap(current.pixels, other.pixels);
};

bool Frame::operator==(Frame const& other) const {
    auto sameFrameID = (frameID == other.frameID);
    auto sameSystemID = (systemID == other.systemID);
    auto samePixels = (pixels == other.pixels);
    auto framesAreEqual = (sameFrameID && sameSystemID && samePixels);
    return framesAreEqual;
}

bool Frame::operator!=(Frame const& other) const {
    return !(operator==(other));
}

void Frame::addTrackToPixelWithID(PixelID const& pixelID, Track&& trackToAdd) {
    updatePixelID(pixelID);
    pixels[pixelID].addTrack(std::forward<Track>(trackToAdd));
}

FrameID const& Frame::getFrameID() const {
    return frameID;
};

SystemID const& Frame::getSystemID() const {
    return systemID;
};

PixelsArray* Frame::getPixels() {
    return &pixels;
};

void Frame::setFrameID(FrameID const& frameID) {
    this->frameID = frameID;
}

void Frame::setSystemID(SystemID const& systemID) {
    this->systemID = systemID;
}

Frame const& Frame::returnDefaultData() noexcept {
    return DEFAULT_FRAME;
}

void Frame::updatePixelID(PixelID const& pixelID) {
    pixels[pixelID].ID = pixelID;
}
