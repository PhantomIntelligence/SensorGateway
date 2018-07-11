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

Pixel::Pixel(PixelID pixelID) : ID(pixelID),tracks(DEFAULT_TRACKS_ARRAY), numberOfTracksInPixel(0){};

Pixel::Pixel(PixelID pixelID, TracksArray tracks, int numberOfTracksInPixel): ID(pixelID),
                                                                              tracks(tracks),
                                                                              numberOfTracksInPixel(numberOfTracksInPixel){

};

Pixel::Pixel(Pixel const& other): Pixel(other.ID, other.tracks, other.numberOfTracksInPixel){

};

Pixel::Pixel(Pixel&& other) noexcept: ID(std::move(other.ID)),tracks(std::move(other.tracks)),
                                      numberOfTracksInPixel(std::move(other.numberOfTracksInPixel)){

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
    std::swap(current.numberOfTracksInPixel, other.numberOfTracksInPixel);
};

bool Pixel::operator==(Pixel const& other) const {
    auto samePixelID = (ID == other.ID);
    auto sameNumberOfTracksInPixel = (numberOfTracksInPixel == other.numberOfTracksInPixel);
    auto sameTracks = true;
    for (auto trackNumber = 0; trackNumber < NUMBER_OF_TRACKS_IN_PIXEL && sameTracks; ++trackNumber) {
        sameTracks = (tracks[trackNumber] == other.tracks[trackNumber]);
    }
    return (samePixelID && sameTracks && sameNumberOfTracksInPixel);
}

bool Pixel::operator!=(Pixel const& other) const {
    return !(operator==(other));
}

void Pixel::addTrack(Track track) {
    validateNotFull();
    tracks[numberOfTracksInPixel] = track;
    numberOfTracksInPixel++;
}

bool Pixel::doesTrackExist(TrackID trackID) {
    bool trackExists = false;
    for (auto track:*getTracks()) {
        if (track.getID() == trackID) {
            trackExists = true;
        }
    }
    return trackExists;
};

Track* Pixel::fetchTrackByID(TrackID trackID) {
    for (auto i = 0; i < NUMBER_OF_TRACKS_IN_PIXEL; ++i) {
        if (tracks[i].getID() == trackID) {
            return &tracks[i];
        }
    }
    return nullptr;
}

PixelID Pixel::getID() const {
    return ID;
}

TracksArray* Pixel::getTracks() {
    return &tracks;
}

void Pixel::validateNotFull() const {
    if (numberOfTracksInPixel >= NUMBER_OF_TRACKS_IN_PIXEL) {
        throw std::runtime_error(ExceptionMessage::PIXEL_TRACK_ARRAY_ILLEGAL_STORE_FULL);
    }
}

Pixel const& Pixel::returnDefaultData() noexcept {
    return DEFAULT_PIXEL;
}





