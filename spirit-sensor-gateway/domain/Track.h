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

#ifndef SPIRITSENSORGATEWAY_SPIRITTRACK_H
#define SPIRITSENSORGATEWAY_SPIRITTRACK_H

#include "spirit-sensor-gateway/common/ConstantFunction.h"

namespace SpiritProtocol {

    namespace DefaultValues{
        namespace Track {
            Acceleration const UNINITIALIZED_TRACK_ACCELERATION_VALUE = 0;
            ConfidenceLevel const UNINITIALIZED_TRACK_CONFIDENCE_VALUE = 0;
            Distance const UNINITIALIZED_TRACK_DISTANCE_VALUE = 0;
            TrackID const UNINITIALIZED_TRACK_ID_VALUE = 0;
            Speed const UNINITIALIZED_TRACK_SPEED_VALUE = 0;
            Intensity const UNINITIALIZED_TRACK_INTENSITY_VALUE = 0;
        }

    }

    class Track {
        public:
            Track();

            Track(TrackID trackID, ConfidenceLevel confidenceLevel, Intensity intensity);
            ~Track();
            bool operator==(Track const& other) const;
            bool operator!=(Track const& other) const;
            Acceleration getAcceleration() const;
            ConfidenceLevel getConfidenceLevel() const;
            Distance getDistance() const;
            TrackID getID() const;
            Intensity getIntensity() const;
            Speed getSpeed() const;
            void setAcceleration(Acceleration acceleration);
            void setDistance(Distance distance);
            void setSpeed(Speed speed);

        private:
            Acceleration acceleration;
            ConfidenceLevel confidenceLevel;
            Distance distance;
            TrackID ID;
            Intensity intensity;
            Speed speed;
    };
}

#endif //SPIRITSENSORGATEWAY_SPIRITTRACK_H
