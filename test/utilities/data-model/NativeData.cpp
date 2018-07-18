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


#include <iostream>
#include <cassert>
#include "NativeData.h"

using ExampleDataModel::NativeData;
using ExampleDataModel::NativeComplement;
using ExampleDataModel::Parameters;
using Defaults::EMPTY_NATIVE_SAMPLE;
using Defaults::DEFAULT_PARAMETERS;


NativeData::NativeData(Parameters const& parameter, NativeComplement const& complement) :
        parameters(parameter),
        distance(complement.distance),
        velocity(complement.velocity),
        intensity(complement.intensity),
        confidenceLevel(complement.confidenceLevel) {}

NativeData::NativeData(NativeData const& other) :
        parameters(other.parameters),
        distance(other.distance),
        velocity(other.velocity),
        intensity(other.intensity),
        confidenceLevel(other.confidenceLevel) {}

NativeData::NativeData(NativeData&& other) noexcept:
        parameters(std::move(other.parameters)),
        distance(std::move(other.distance)),
        velocity(std::move(other.velocity)),
        intensity(std::move(other.intensity)),
        confidenceLevel(std::move(other.confidenceLevel)) {
    other.distance = EMPTY_NATIVE_SAMPLE;
    other.intensity = EMPTY_NATIVE_SAMPLE;
    other.velocity = EMPTY_NATIVE_SAMPLE;
    other.confidenceLevel = EMPTY_NATIVE_SAMPLE;
}

void NativeData::swap(NativeData& current, NativeData& other) noexcept {
    parameters.swap(current.parameters, other.parameters);
    std::swap(current.distance, other.distance);
    std::swap(current.intensity, other.intensity);
    std::swap(current.velocity, other.velocity);
    std::swap(current.confidenceLevel, other.confidenceLevel);
}

Parameters const& NativeData::getParameters() const {
    return parameters;
}


NativeSample const& NativeData::getDistance() const {
    return distance;
}

NativeSample const& NativeData::getVelocity() const {
    return velocity;
}

NativeSample const& NativeData::getIntensity() const {
    return intensity;
}

NativeSample const& NativeData::getConfidenceLevel() const {
    return confidenceLevel;
}

bool NativeData::operator==(NativeData const& other) const {
    auto sameParameters = getParameters() == other.getParameters();
    auto sameDistance = getDistance() == other.getDistance();
    auto sameVelocity = getVelocity() == other.getVelocity();
    auto sameIntensity = getIntensity() == other.getIntensity();
    auto sameConfidenceLevel = getConfidenceLevel() == other.getConfidenceLevel();

    bool equal = (sameParameters && sameDistance && sameVelocity && sameIntensity && sameConfidenceLevel);
    return equal;
}

bool NativeData::operator!=(NativeData const& other) const {
    return !operator==(other);
}

NativeData const& NativeData::returnDefaultData() noexcept {
    return Defaults::DEFAULT_NATIVE_DATA;
}

