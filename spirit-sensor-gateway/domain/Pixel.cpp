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

#include <iostream>
#include "Pixel.h"

using DataFlow::Pixel;
using DataFlow::TracksArray;
using DataFlow::PixelID;
using Defaults::Pixel::DEFAULT_PIXEL;
using Defaults::Pixel::DEFAULT_TRACKS_ARRAY;

Pixel::Pixel() : Pixel(Pixel::returnDefaultData()) {}

Pixel::Pixel(PixelID pixelID, TracksArray tracks, int currentNumberOfTracks) :
        ID(pixelID), tracks(tracks), currentNumberOfTracksInPixel(currentNumberOfTracks) {
};

Pixel::Pixel(Pixel const& other) :
        Pixel(other.ID, other.tracks, other.currentNumberOfTracksInPixel) {

};

Pixel::Pixel(Pixel&& other) noexcept: ID(std::move(other.ID)),
                                      tracks(std::move(other.tracks)),
                                      currentNumberOfTracksInPixel(std::move(other.currentNumberOfTracksInPixel)) {

};

Pixel& Pixel::operator=(Pixel const& other)& {
    Pixel temporary(std::move(other));
    swap(*this, temporary);
    return *this;
};

Pixel& Pixel::operator=(Pixel&& other)& noexcept {
    swap(*this, other);
    return *this;
};

void Pixel::swap(Pixel& current, Pixel& other) noexcept {
    std::swap(current.ID, other.ID);
    std::swap(current.tracks, other.tracks);
    std::swap(current.currentNumberOfTracksInPixel, other.currentNumberOfTracksInPixel);
};

bool Pixel::operator==(Pixel const& other) const {
    auto samePixelID = (ID == other.ID);
    auto sameTracks = (tracks == other.tracks);
    auto sameCurrentNumberOfTracks = (currentNumberOfTracksInPixel == other.currentNumberOfTracksInPixel);
    auto pixelsAreEqual = (samePixelID && sameTracks && sameCurrentNumberOfTracks);
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

bool Pixel::doesTrackExist(TrackID const& trackID) {
    bool trackExists = false;
    for (auto track:*getTracks()) {
        if (track.ID == trackID) {
            trackExists = true;
        }
    }
    return trackExists;
};

Track* Pixel::fetchTrackByID(TrackID const& trackID) {
    for (auto i = 0; i < NUMBER_OF_TRACKS_IN_PIXEL; ++i) {
        if (tracks[i].ID == trackID) {
            return &tracks[i];
        }
    }
    return nullptr;
}

PixelID const& Pixel::getID() const {
    return ID;
}

TracksArray* Pixel::getTracks() {
    return &tracks;
}

void Pixel::validateNotFull() const {
    if (currentNumberOfTracksInPixel >= NUMBER_OF_TRACKS_IN_PIXEL) {
        throw std::runtime_error(ExceptionMessage::PIXEL_TRACK_ARRAY_ILLEGAL_STORE_FULL);
    }
}

Pixel const& Pixel::returnDefaultData() noexcept {
    return DEFAULT_PIXEL;
}





