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

using DataFlow::Acceleration;
using DataFlow::ConfidenceLevel;
using DataFlow::Distance;
using DataFlow::TrackID;
using DataFlow::Speed;
using DataFlow::Intensity;
using DataFlow::Track;
using Defaults::Track::DEFAULT_ID;
using Defaults::Track::DEFAULT_ACCELERATION;
using Defaults::Track::DEFAULT_SPEED;
using Defaults::Track::DEFAULT_DISTANCE;
using Defaults::Track::DEFAULT_CONFIDENCE;
using Defaults::Track::DEFAULT_INTENSITY;
using Defaults::Track::DEFAULT_TRACK;


Track::Track(TrackID trackID, ConfidenceLevel confidenceLevel, Intensity intensity) : ID(trackID),
                                                                                      confidenceLevel(confidenceLevel),
                                                                                      intensity(intensity),
                                                                                      acceleration(DEFAULT_ACCELERATION),
                                                                                      distance(DEFAULT_DISTANCE),
                                                                                      speed(DEFAULT_SPEED) {

};

Track::Track(TrackID trackID, ConfidenceLevel confidenceLevel, Intensity intensity,
             Acceleration acceleration, Distance distance, Speed speed) : ID(trackID),
                                                                          confidenceLevel(confidenceLevel),
                                                                          intensity(intensity),
                                                                          acceleration(acceleration),
                                                                          distance(distance),
                                                                          speed(speed) {

};

Track::Track() : ID(DEFAULT_ID),
                 confidenceLevel(DEFAULT_CONFIDENCE),
                 intensity(DEFAULT_INTENSITY),
                 acceleration(DEFAULT_ACCELERATION),
                 distance(DEFAULT_DISTANCE),
                 speed(DEFAULT_SPEED) {
};


Track::Track(Track const& other) : Track(other.ID, other.confidenceLevel, other.intensity,
                                         other.acceleration, other.distance, other.speed) {
};

Track::Track(Track&& other) noexcept: ID(std::move(other.ID)),
                                      confidenceLevel(std::move(other.confidenceLevel)),
                                      intensity(std::move(other.intensity)),
                                      acceleration(std::move(other.acceleration)),
                                      distance(std::move(other.distance)),
                                      speed(std::move(other.speed)) {
};

Track& Track::operator=(Track const& other)& {
    Track temporary(std::move(other));
    swap(*this, temporary);
    return *this;
};

Track& Track::operator=(Track&& other)& noexcept {
    swap(*this, other);
    return *this;
};

void Track::swap(Track& current, Track& other) noexcept{
    std::swap(current.ID, other.ID);
    std::swap(current.confidenceLevel, other.confidenceLevel);
    std::swap(current.intensity, other.intensity);
    std::swap(current.acceleration, other.acceleration);
    std::swap(current.distance, other.distance);
    std::swap(current.speed, other.speed);
};

bool Track::operator==(Track const& other) const {
    auto sameTrackID = (ID == other.ID);
    auto sameConfidenceLevel = (confidenceLevel == other.confidenceLevel);
    auto sameIntensity = (intensity == other.intensity);
    auto sameAcceleration = (acceleration == other.acceleration);
    auto sameDistance = (distance == other.distance);
    auto sameSpeed = (speed == other.speed);
    bool tracksAreEqual = (sameAcceleration &&
                           sameConfidenceLevel &&
                           sameDistance &&
                           sameTrackID &&
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

Track const& Track::returnDefaultData() noexcept {
    return DEFAULT_TRACK;
}

