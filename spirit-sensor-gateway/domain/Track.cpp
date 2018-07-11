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

using namespace SpiritProtocol;
using namespace Defaults::Track;

Track::Track(TrackID trackID, ConfidenceLevel confidenceLevel, Intensity intensity) :
        ID(trackID),
        confidenceLevel(confidenceLevel),
        intensity(intensity),
        acceleration(DEFAULT_ACCELERATION_VALUE),
        speed(DEFAULT_SPEED_VALUE),
        distance(DEFAULT_DISTANCE_VALUE) {
};

Track::Track() : ID(DEFAULT_ID_VALUE),
                 acceleration(DEFAULT_ACCELERATION_VALUE),
                 confidenceLevel(DEFAULT_CONFIDENCE_VALUE),
                 distance(DEFAULT_DISTANCE_VALUE),
                 intensity(DEFAULT_INTENSITY_VALUE),
                 speed(DEFAULT_SPEED_VALUE) {

};

Track::~Track() {
};

bool Track::operator==(Track const& other) const {
    auto sameAcceleration = (acceleration == other.acceleration);
    auto sameConfidenceLevel = (confidenceLevel == other.confidenceLevel);
    auto sameDistance = (distance == other.distance);
    auto sameTrackId = (ID == other.ID);
    auto sameIntensity = (intensity == other.intensity);
    auto sameSpeed = (speed == other.speed);
    bool tracksAreEqual = (sameAcceleration &&
                           sameConfidenceLevel &&
                           sameDistance &&
                           sameTrackId &&
                           sameIntensity &&
                           sameSpeed);
    return tracksAreEqual;
}

bool Track::operator!=(Track const& other) const {
    return !(operator==(other));
}

Acceleration Track::getAcceleration() const {
    return acceleration;
};

ConfidenceLevel Track::getConfidenceLevel() const {
    return confidenceLevel;
};

Distance Track::getDistance() const {
    return distance;
};

TrackID Track::getID() const {
    return ID;
};

Intensity Track::getIntensity() const {
    return intensity;
};

Speed Track::getSpeed() const {
    return speed;
};

void Track::setAcceleration(Acceleration const& acceleration) {
    this->acceleration = acceleration;
};

void Track::setDistance(Distance const& distance) {
    this->distance = distance;
};

void Track::setSpeed(Speed const& speed) {
    this->speed = speed;
}

