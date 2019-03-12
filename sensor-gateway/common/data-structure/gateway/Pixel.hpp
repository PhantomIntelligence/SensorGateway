/**
	Copyright 2014-2019 Phantom Intelligence Inc.

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

#ifndef SENSORGATEWAY_PIXEL_HPP
#define SENSORGATEWAY_PIXEL_HPP

#include "Track.h"

namespace DataFlow {

    namespace Defaults {
        static constexpr size_t const NUMBER_OF_TRACKS_PER_PIXEL = 1;
    }

    /**
     * @tparam N The number of tracks potentially in the pixel
     */
    template<size_t N = Defaults::NUMBER_OF_TRACKS_PER_PIXEL>
    class Pixel {

    public:

        using Track = DataFlow::Track;
        static constexpr auto const numberOfTracksPerPixel = N;
        using TracksArray = std::array<Track, N>;

        explicit Pixel(PixelId const& pixelId, TracksArray tracks, int currentNumberOfTracks) :
                id(pixelId),
                tracks(std::move(tracks)),
                currentNumberOfTracksInPixel(currentNumberOfTracks) {}

        ~Pixel() noexcept {}

        Pixel() : Pixel(returnDefaultData()) {}

        Pixel(Pixel const& other) :
                Pixel(other.id,
                      other.tracks,
                      other.currentNumberOfTracksInPixel) {}

        Pixel(Pixel&& other) noexcept;

        Pixel& operator=(Pixel const& other)& {
            Pixel temporary(other);
            swap(*this, temporary);
            return *this;
        }

        Pixel& operator=(Pixel&& other)& noexcept {
            swap(*this, other);
            return *this;
        }

        static void swap(Pixel& current, Pixel& other) noexcept;

        bool operator==(Pixel const& other) const {
            auto samePixelId = (id == other.id);
            auto sameTracks = (tracks == other.tracks);
            auto sameCurrentNumberOfTracks = (currentNumberOfTracksInPixel == other.currentNumberOfTracksInPixel);
            auto pixelsAreEqual = (samePixelId && sameTracks && sameCurrentNumberOfTracks);
            return pixelsAreEqual;
        }

        bool operator!=(Pixel const& other) const {
            return !(operator==(other));
        }

        static Pixel const& returnDefaultData() noexcept;

        void addTrack(Track&& track) {
            validateNotFull();
            Track::swap(tracks[currentNumberOfTracksInPixel], track);
            currentNumberOfTracksInPixel++;
        }


        bool doesTrackExist(TrackId const& trackId) {
            bool trackExists = false;
            for (auto track:*getTracks()) {
                if (track.id == trackId) {
                    trackExists = true;
                }
            }
            return trackExists;
        }

        Track* fetchTrackById(TrackId const& trackId) {
            for (auto trackIndex = 0; trackIndex < N; ++trackIndex) {
                if (tracks[trackIndex].id == trackId) {
                    return &tracks[trackIndex];
                }
            }
            return nullptr;
        }

        TracksArray* getTracks() {
            return &tracks;
        }

        PixelId id;

        int getCurrentNumberOfTracksInPixel() const {
            return currentNumberOfTracksInPixel;
        }

    private:

        void validateNotFull() const {
            if (currentNumberOfTracksInPixel >= N) {
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

        TracksArray tracks;

        int currentNumberOfTracksInPixel;

    };

    namespace Defaults {

        using DataFlow::PixelId;

        template<size_t N>
        using TracksArray = typename Pixel<N>::TracksArray;

        /**
         * @warning Modifying these values without updating the FlatBuffers file WILL create a huge performance drop!!!
         * If a value is modified here, be sure its homologous value in the communication protocol schema file is too.
         * @see https://github.com/PhantomIntelligence/GatewayProtocol.git
         */
        PixelId const UNDEFINED_ID = 65535;
        PixelId const DEFAULT_ID = UNDEFINED_ID;

        template<size_t N>
        TracksArray<N> const DEFAULT_TRACKS_ARRAY = TracksArray<N>();

        int const DEFAULT_CURRENT_NUMBER_OF_TRACKS = 0;

        template<size_t N>
        Pixel<N> const DEFAULT_PIXEL = Pixel<N>(DEFAULT_ID, DEFAULT_TRACKS_ARRAY<N>, DEFAULT_CURRENT_NUMBER_OF_TRACKS);
    }


    template<size_t N>
    Pixel<N>::Pixel(Pixel<N>&& other) noexcept :
            id(std::move(other.id)),
            tracks(std::move(other.tracks)),
            currentNumberOfTracksInPixel(std::move(other.currentNumberOfTracksInPixel)) {
        other.id = Defaults::DEFAULT_ID;
        other.tracks = Defaults::DEFAULT_TRACKS_ARRAY<N>;
        other.currentNumberOfTracksInPixel = Defaults::DEFAULT_CURRENT_NUMBER_OF_TRACKS;
    }

    template<size_t N>
    void Pixel<N>::swap(Pixel<N>& current, Pixel<N>& other) noexcept {
        std::swap(current.id, other.id);
        std::swap(current.tracks, other.tracks);
        std::swap(current.currentNumberOfTracksInPixel, other.currentNumberOfTracksInPixel);
    }

    template<size_t N>
    Pixel<N> const& Pixel<N>::returnDefaultData() noexcept {
        return Defaults::DEFAULT_PIXEL<N>;
    }
}


#endif //SENSORGATEWAY_PIXEL_HPP
