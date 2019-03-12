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

#include "Track.h"

using DataFlow::Track;
using DataFlow::Defaults::Track::DEFAULT_ID;
using DataFlow::Defaults::Track::DEFAULT_ACCELERATION;
using DataFlow::Defaults::Track::DEFAULT_SPEED;
using DataFlow::Defaults::Track::DEFAULT_DISTANCE;
using DataFlow::Defaults::Track::DEFAULT_CONFIDENCE_LEVEL;
using DataFlow::Defaults::Track::DEFAULT_INTENSITY;
using DataFlow::Defaults::Track::DEFAULT_TRACK;


Track::Track(TrackId trackId, Distance distance, Intensity intensity, Speed speed, Acceleration acceleration,
             ConfidenceLevel confidenceLevel) :
        id(trackId),
        confidenceLevel(confidenceLevel),
        intensity(intensity),
        acceleration(acceleration),
        distance(distance),
        speed(speed) {

}

Track::Track() : Track(Track::returnDefaultData()) {
}

Track::Track(Track const& other) : Track(other.id, other.distance, other.intensity, other.speed,
                                         other.acceleration, other.confidenceLevel) {
}

Track::Track(Track&& other) noexcept: id(other.id),
                                      distance(other.distance),
                                      intensity(other.intensity),
                                      speed(other.speed),
                                      acceleration(other.acceleration),
                                      confidenceLevel(other.confidenceLevel) {
}

Track& Track::operator=(Track const& other)& {
    Track temporary(other);
    swap(*this, temporary);
    return *this;
}

Track& Track::operator=(Track&& other)& noexcept {
    swap(*this, other);
    return *this;
}

void Track::swap(Track& current, Track& other) noexcept {
    std::swap(current.id, other.id);
    std::swap(current.confidenceLevel, other.confidenceLevel);
    std::swap(current.intensity, other.intensity);
    std::swap(current.acceleration, other.acceleration);
    std::swap(current.distance, other.distance);
    std::swap(current.speed, other.speed);
}

bool Track::operator==(Track const& other) const {
    auto sameTrackId = (id == other.id);
    auto sameDistance = (distance == other.distance);
    auto sameIntensity = (intensity == other.intensity);
    auto sameSpeed = (speed == other.speed);
    auto sameAcceleration = (acceleration == other.acceleration);
    auto sameConfidenceLevel = (confidenceLevel == other.confidenceLevel);
    bool tracksAreEqual = (sameTrackId &&
                           sameDistance &&
                           sameIntensity &&
                           sameSpeed &&
                           sameAcceleration &&
                           sameConfidenceLevel);
    return tracksAreEqual;
}

bool Track::operator!=(Track const& other) const {
    return !(operator==(other));
}

Track const& Track::returnDefaultData() noexcept {
    return DEFAULT_TRACK;
}

