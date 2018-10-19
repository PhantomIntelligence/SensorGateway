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

#ifndef SENSORGATEWAY_TRACK_H
#define SENSORGATEWAY_TRACK_H

#include "sensor-gateway/common/data-flow/DataProcessingScheduler.hpp"

namespace DataFlow {

    class Track {

    public:

        explicit Track(TrackId trackId, ConfidenceLevel confidenceLevel, Intensity intensity,
                       Acceleration acceleration, Distance distance, Speed speed);

        Track();

        ~Track() = default;

        Track(Track const& other);

        Track(Track&& other) noexcept;

        Track& operator=(Track const& other)& ;

        Track& operator=(Track&& other)& noexcept;

        static void swap(Track& current, Track& other) noexcept;

        bool operator==(Track const& other) const;

        bool operator!=(Track const& other) const;

        static Track const& returnDefaultData() noexcept;

        TrackId id;
        ConfidenceLevel confidenceLevel;
        Intensity intensity;
        Acceleration acceleration;
        Distance distance;
        Speed speed;
    };
}

namespace Defaults {
    namespace Track {
        using DataFlow::Track;
        using DataFlow::TrackId;
        using DataFlow::ConfidenceLevel;
        using DataFlow::Intensity;
        using DataFlow::Acceleration;
        using DataFlow::Distance;
        using DataFlow::Speed;
        TrackId const UNDEFINED_ID = std::numeric_limits<TrackId>::infinity();
        TrackId const DEFAULT_ID = UNDEFINED_ID;
        ConfidenceLevel const DEFAULT_CONFIDENCE_LEVEL = 0;
        Intensity const DEFAULT_INTENSITY = 0;
        Acceleration const DEFAULT_ACCELERATION = 0;
        Distance const DEFAULT_DISTANCE = 0;
        Speed const DEFAULT_SPEED = 0;
        Track const DEFAULT_TRACK = Track(DEFAULT_ID, DEFAULT_CONFIDENCE_LEVEL, DEFAULT_INTENSITY, DEFAULT_ACCELERATION,
                                          DEFAULT_DISTANCE, DEFAULT_SPEED);
    }
}

#endif //SENSORGATEWAY_TRACK_H
