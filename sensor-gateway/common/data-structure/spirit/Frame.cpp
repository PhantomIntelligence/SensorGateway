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
using DataFlow::FrameId;
using Defaults::Frame::DEFAULT_FRAME;
using Defaults::Frame::DEFAULT_FRAME_ID;
using Defaults::Frame::DEFAULT_PIXELS_ARRAY;
using Defaults::Frame::DEFAULT_SYSTEM_ID;
using DataFlow::PixelsArray;
using DataFlow::SystemId;

Frame::Frame(FrameId frameId, SystemId systemId, PixelsArray pixels) :
        frameId(frameId), systemId(systemId), pixels(std::move(pixels)) {};

Frame::Frame() : Frame(Frame::returnDefaultData()) {};

Frame::Frame(Frame const& other) :
        Frame(other.frameId, other.systemId, other.pixels) {};

Frame::Frame(Frame&& other) noexcept:
        frameId(other.frameId),
        systemId(other.systemId),
        pixels(other.pixels) {
    other.frameId = DEFAULT_FRAME_ID;
    other.systemId = DEFAULT_SYSTEM_ID;
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
    std::swap(current.frameId, other.frameId);
    std::swap(current.systemId, other.systemId);
    std::swap(current.pixels, other.pixels);
};

bool Frame::operator==(Frame const& other) const {
    auto sameFrameId = (frameId == other.frameId);
    auto sameSystemId = (systemId == other.systemId);
    auto samePixels = (pixels == other.pixels);
    auto framesAreEqual = (sameFrameId && sameSystemId && samePixels);
    return framesAreEqual;
}

bool Frame::operator!=(Frame const& other) const {
    return !(operator==(other));
}

void Frame::addTrackToPixelWithId(PixelId const& pixelId, Track&& trackToAdd) {
    updatePixelId(pixelId);
    pixels[pixelId].addTrack(std::forward<Track>(trackToAdd));
}

PixelsArray* Frame::getPixels() {
    return &pixels;
};

Frame const& Frame::returnDefaultData() noexcept {
    return DEFAULT_FRAME;
}

void Frame::updatePixelId(PixelId const& pixelId) {
    pixels[pixelId].id = pixelId;
}
