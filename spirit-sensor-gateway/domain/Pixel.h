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

#ifndef SPIRITSENSORGATEWAY_SPIRITPIXEL_H
#define SPIRITSENSORGATEWAY_SPIRITPIXEL_H

#include "Track.h"

namespace SpiritProtocol {

    class Pixel {
    public:
        Pixel(PixelID pixelID);

        ~Pixel();

        Pixel() = default;

        bool operator==(Pixel const& other) const;

        void addTrack(Track track);

        bool doesTrackExist(TrackID trackID);

        Track* fetchTrackByID(TrackID trackID);

        PixelID getID() const;

        std::array<Track, MAXIMUM_NUMBER_OF_TRACKS_IN_AWL16_PIXEL>* getTracks();

        void validateNotFull() const;

    private:
        int numberOfTracksInPixel = 0;
        PixelID ID;
        std::array<Track, MAXIMUM_NUMBER_OF_TRACKS_IN_AWL16_PIXEL> tracks;
    };
}

#endif //SPIRITSENSORGATEWAY_SPIRITPIXEL_H
