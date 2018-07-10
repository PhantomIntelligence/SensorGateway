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

using namespace DataFlow;
using namespace Defaults::Pixel;

Pixel::Pixel(PixelID pixelID) : ID(pixelID),tracks(DEFAULT_TRACKS_ARRAY) {};

Pixel::Pixel() {

};

Pixel::~Pixel() {
};

bool Pixel::operator==(Pixel const& other) const {
    auto sameID = (ID == other.ID);
    auto sameTracks = true;
    for (auto i = 0; i < NUMBER_OF_TRACKS_IN_AWL16_PIXEL && sameTracks && sameID; ++i) {
        sameTracks = (tracks[i] == other.tracks[i]);
    }
    return (sameID && sameTracks);
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
    for (auto i = 0; i < NUMBER_OF_TRACKS_IN_AWL16_PIXEL; ++i) {
        if (tracks[i].getID() == trackID) {
            return &tracks[i];
        }
    }
    return nullptr;
}

PixelID Pixel::getID() const {
    return ID;
}

std::array<Track, NUMBER_OF_TRACKS_IN_AWL16_PIXEL>* Pixel::getTracks() {
    return &tracks;
}

void Pixel::validateNotFull() const {
    if (numberOfTracksInPixel >= NUMBER_OF_TRACKS_IN_AWL16_PIXEL) {
        throw std::runtime_error(ExceptionMessage::PIXEL_TRACK_ARRAY_ILLEGAL_STORE_FULL);
    }

}

Pixel const Pixel::returnDefaultData() noexcept {
    return DEFAULT_PIXEL;
}





