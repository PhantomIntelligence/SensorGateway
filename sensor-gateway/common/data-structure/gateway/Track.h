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

        explicit Track(TrackId trackId, Distance distance, Intensity intensity, Speed speed, Acceleration acceleration,
                       ConfidenceLevel confidenceLevel);

        Track();

        ~Track() = default;

        Track(Track const& other);

        Track(Track&& other) noexcept;

        Track& operator=(Track const& other)&;

        Track& operator=(Track&& other)& noexcept;

        static void swap(Track& current, Track& other) noexcept;

        bool operator==(Track const& other) const;

        bool operator!=(Track const& other) const;

        static Track const& returnDefaultData() noexcept;

        TrackId id;
        Distance distance;
        Intensity intensity;
        Speed speed;
        Acceleration acceleration;
        ConfidenceLevel confidenceLevel;
    };

    namespace Defaults {
        namespace Track {

            /**
             * @warning Carelessly modifying these values WILL cause a huge performance drop.
             * If a value is modified here, be sure its homologous value in the communication protocol schema file is too.
             * @see https://github.com/PhantomIntelligence/GatewayProtocol.git
             */
            TrackId const UNDEFINED_ID = 0;
            TrackId const DEFAULT_ID = UNDEFINED_ID;
            Distance const DEFAULT_DISTANCE = 0;
            Intensity const DEFAULT_INTENSITY = 0;
            Speed const DEFAULT_SPEED = 0;
            Acceleration const DEFAULT_ACCELERATION = 0;
            ConfidenceLevel const DEFAULT_CONFIDENCE_LEVEL = 0;
            typename DataFlow::Track const DEFAULT_TRACK = typename DataFlow::Track(DEFAULT_ID,
                                              DEFAULT_DISTANCE,
                                              DEFAULT_INTENSITY,
                                              DEFAULT_SPEED,
                                              DEFAULT_ACCELERATION,
                                              DEFAULT_CONFIDENCE_LEVEL);
        }
    }
}


#endif //SENSORGATEWAY_TRACK_H
