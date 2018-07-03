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

namespace SpiritProtocol {
    Pixel::Pixel(PixelID pixelID) : ID(pixelID) {};

    Pixel::~Pixel() {
    };


    Pixel::Pixel(Pixel const& other) : ID(other.ID), tracks(other.tracks) {

    }

    Pixel::Pixel(Pixel&& other) noexcept {

    }

    Pixel& Pixel::operator=(Pixel& other)& {
        swap(*this, other);
        return *this;
    }

    Pixel& Pixel::operator=(Pixel&& other)& noexcept {
        Pixel temporary(std::move(other));
        swap(*this, temporary);
        return *this;
    };

    void Pixel::swap(Pixel& current, Pixel& other) noexcept {
        std::swap(current.ID, other.ID);
        std::swap(current.tracks, other.tracks);
    }


    bool Pixel::operator==(const SpiritProtocol::Pixel& other) const {
        auto sameID = ID == other.ID;
        auto sameTracks = true;
        for (auto i = 0; i < MAXIMUM_NUMBER_OF_TRACK_IN_AWL16_PIXEL && sameTracks; ++i) {
            sameTracks = tracks[i] == other.tracks[i] && sameTracks;
        }
        return (sameID && sameTracks);
    }

    bool Pixel::operator!=(const SpiritProtocol::Pixel& other) const {
        return !(operator==(other));
    }

    void Pixel::addTrack(Track track) {
        tracks[0] = track;
    }

    bool Pixel::doesTrackExist(TrackID trackID)  {
        bool trackExists = false;
        for (auto track:*getTracksAddress()) {
            if (track.getID() == trackID) {
                trackExists = true;
            }
        }
        return trackExists;
    };

    Track* Pixel::fetchTrackByID(TrackID trackID) {
        Track* fetchedTrack;
        for (auto i = 0; i < MAXIMUM_NUMBER_OF_TRACK_IN_AWL16_PIXEL; ++i) {
            Track* track = &tracks[i];
            if (track->getID() == trackID) {
                fetchedTrack = track;
                break;
            }
        }
        return fetchedTrack;
    }

    PixelID Pixel::getID() const {
        return ID;
    }

    std::array<Track, MAXIMUM_NUMBER_OF_TRACK_IN_AWL16_PIXEL>* Pixel::getTracksAddress()  {

        return &tracks;
    };

}

