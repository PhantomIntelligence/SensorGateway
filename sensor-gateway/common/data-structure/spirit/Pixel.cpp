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

#include "Pixel.h"

using DataFlow::Pixel;
using DataFlow::TracksArray;
using DataFlow::PixelId;
using Defaults::Pixel::DEFAULT_PIXEL;
using Defaults::Pixel::DEFAULT_TRACKS_ARRAY;
using Sensor::AWL::_16::NUMBER_OF_PIXELS_IN_LAYER;
using Sensor::AWL::_16::NUMBER_OF_PIXELS_IN_FRAME;
using Sensor::AWL::_16::HORIZONTAL_FIELD_OF_VIEW;
using Sensor::AWL::_16::ANGLE_RANGE;
using Sensor::AWL::_16::NUMBER_OF_LAYER;
using Sensor::AWL::_16::MULTIPLICATIVE_CONSTANT;

Pixel::Pixel(PixelId pixelId, TracksArray tracks, int currentNumberOfTracks) :
        id(pixelId), tracks(std::move(tracks)), currentNumberOfTracksInPixel(currentNumberOfTracks) {
};

Pixel::Pixel() : Pixel(Pixel::returnDefaultData()) {};

Pixel::Pixel(Pixel const& other) :
        Pixel(other.id, other.tracks, other.currentNumberOfTracksInPixel) {

};

Pixel::Pixel(Pixel&& other) noexcept: id(other.id),
                                      tracks(std::move(other.tracks)),
                                      currentNumberOfTracksInPixel(other.currentNumberOfTracksInPixel) {

};

Pixel& Pixel::operator=(Pixel const& other)& {
    Pixel temporary(other);
    swap(*this, temporary);
    return *this;
};

Pixel& Pixel::operator=(Pixel&& other)& noexcept {
    swap(*this, other);
    return *this;
};

void Pixel::swap(Pixel& current, Pixel& other) noexcept {
    std::swap(current.id, other.id);
    std::swap(current.tracks, other.tracks);
    std::swap(current.currentNumberOfTracksInPixel, other.currentNumberOfTracksInPixel);
};

bool Pixel::operator==(Pixel const& other) const {
    auto samePixelId = (id == other.id);
    auto sameTracks = (tracks == other.tracks);
    auto sameCurrentNumberOfTracks = (currentNumberOfTracksInPixel == other.currentNumberOfTracksInPixel);
    auto pixelsAreEqual = (samePixelId && sameTracks && sameCurrentNumberOfTracks);
    return pixelsAreEqual;
}

bool Pixel::operator!=(Pixel const& other) const {
    return !(operator==(other));
}

void Pixel::addTrack(Track&& track) {
    validateNotFull();
    Track::swap(tracks[currentNumberOfTracksInPixel], track);
    currentNumberOfTracksInPixel++;
}

bool Pixel::doesTrackExist(TrackId const& trackId) {
    bool trackExists = false;
    for (auto track:*getTracks()) {
        if (track.id == trackId) {
            trackExists = true;
        }
    }
    return trackExists;
};

Track* Pixel::fetchTrackById(TrackId const& trackId) {
    for (auto trackIndex = 0; trackIndex < NUMBER_OF_TRACKS_IN_PIXEL; ++trackIndex) {
        if (tracks[trackIndex].id == trackId) {
            return &tracks[trackIndex];
        }
    }
    return nullptr;
}

TracksArray* Pixel::getTracks() {
    return &tracks;
}

Pixel const& Pixel::returnDefaultData() noexcept {
    return DEFAULT_PIXEL;
}

void Pixel::validateNotFull() const {
    if (currentNumberOfTracksInPixel >= NUMBER_OF_TRACKS_IN_PIXEL) {
        // TODO : Have a cleaner throw, elsewhere?
        throw ErrorHandling::SensorAccessLinkError(
                "pixel::addTrack",
                ErrorHandling::Category::TRANSLATION_ERROR,
                ErrorHandling::Severity::ERROR,
                ErrorHandling::GatewayErrorCode::PIXEL_ALREADY_HAS_MAXIMUM_TRACK,
                ExceptionMessage::PIXEL_TRACK_ARRAY_ILLEGAL_STORE_FULL
        );
    }
}

int Pixel::getCurrentNumberOfTracksInPixel() const {
    return currentNumberOfTracksInPixel;
}




