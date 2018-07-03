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

namespace SpiritProtocol {
    Pixel::Pixel(PixelID pixelID) : ID(pixelID) {};

    Pixel::~Pixel() {
    };

    void Pixel::addTrack(Track track) {
        tracks.insert(std::make_pair(track.getID(), track));
    }

    bool Pixel::doesTrackExist(TrackID trackID) {
        bool trackExists = false;
        if (tracks.find(trackID) != tracks.end()) {
            trackExists = true;
        }
        return trackExists;
    };

    Track* Pixel::fetchTrackByID(TrackID trackID) {
        return &tracks.at(trackID);
    }

    PixelID Pixel::getID() const {
        return ID;
    };

    std::unordered_map<TrackID, Track> Pixel::getTracks() const {
        return tracks;
    };
}

