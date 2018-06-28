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

        Pixel(Pixel const& other);

        Pixel(Pixel&& other) noexcept;

        Pixel& operator=(Pixel& other)& ;

        Pixel& operator=(Pixel&& other)& noexcept;

        void swap(Pixel& current, Pixel& other) noexcept;

        bool operator==(Pixel const& other) const;

        bool operator!=(Pixel const& other) const;

        void addTrack(Track track);

        bool doesTrackExist(TrackID trackID) const;

        Track* fetchTrackByID(TrackID trackID);

        PixelID getID() const;

        std::array<Track, MAXIMUM_NUMBER_OF_TRACK_IN_AWL16_PIXEL> getTracks() const;

    private:
        uint16_t ID;
        std::array<Track, MAXIMUM_NUMBER_OF_TRACK_IN_AWL16_PIXEL> tracks;
    };
}

#endif //SPIRITSENSORGATEWAY_SPIRITPIXEL_H
