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

#ifndef SENSORGATEWAY_PIXEL_H
#define SENSORGATEWAY_PIXEL_H

#include "Track.h"

using DataFlow::Track;
using Sensor::AWL::_16::NUMBER_OF_TRACKS_IN_PIXEL;

namespace DataFlow {
    using TracksArray = std::array<Track, NUMBER_OF_TRACKS_IN_PIXEL>;
}

namespace DataFlow {

    class Pixel {

    public:

        explicit Pixel(PixelId pixelId, TracksArray tracks, int currentNumberOfTracks);

        Pixel();

        ~Pixel() = default;

        Pixel(Pixel const& other);

        Pixel(Pixel&& other) noexcept;

        Pixel& operator=(Pixel const& other)&;

        Pixel& operator=(Pixel&& other)& noexcept;

        static void swap(Pixel& current, Pixel& other) noexcept;

        bool operator==(Pixel const& other) const;

        bool operator!=(Pixel const& other) const;

        void addTrack(Track&& track);

        bool doesTrackExist(TrackId const& trackId);

        Track* fetchTrackById(TrackId const& trackId);

        TracksArray* getTracks();

        static Pixel const& returnDefaultData() noexcept;

        PixelId id;

        int getCurrentNumberOfTracksInPixel() const;

    private:

        TracksArray tracks;

        int currentNumberOfTracksInPixel;

        void validateNotFull() const;
    };
}

namespace Defaults {
    namespace Pixel {
        using DataFlow::Pixel;
        using DataFlow::PixelId;
        using DataFlow::TracksArray;

        /**
         * @warning Modifying these values without updating the FlatBuffers file WILL create a huge performance drop!!!
         * If a value is modified here, be sure its homologous value in the communication protocol schema file is too.
         * @see https://github.com/PhantomIntelligence/GatewayProtocol.git
         */
        PixelId const UNDEFINED_ID = 65535;
        PixelId const DEFAULT_ID = UNDEFINED_ID;
        TracksArray const DEFAULT_TRACKS_ARRAY = TracksArray();
        int const DEFAULT_CURRENT_NUMBER_OF_TRACKS = 0;
        Pixel const DEFAULT_PIXEL = Pixel(DEFAULT_ID, DEFAULT_TRACKS_ARRAY, DEFAULT_CURRENT_NUMBER_OF_TRACKS);
    }
}

#endif //SENSORGATEWAY_PIXEL_H
