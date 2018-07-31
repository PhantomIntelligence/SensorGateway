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


#include "ProcessedData.h"
#include <utility>
#include <iostream>

using ExampleDataModel::NativeData;
using ExampleDataModel::ProcessedData;
using ExampleDataModel::ProcessedComplement;
using Defaults::EMPTY_SAMPLE;

ProcessedData::ProcessedData(NativeData const& nativeData, ProcessedComplement const& complement) :
        NativeData(nativeData),
        discreteDetections(complement.discreteDetection),
        samplingDistance(complement.samplingDistance) {}


ProcessedData::ProcessedData(ProcessedData const& other) :
        NativeData(other),
        discreteDetections(other.discreteDetections),
        samplingDistance(other.samplingDistance) {
}

ProcessedData::ProcessedData(ProcessedData&& other) noexcept:
        NativeData(std::move(other)),
        discreteDetections(std::move(other.discreteDetections)),
        samplingDistance(std::move(other.samplingDistance)) {
    other.discreteDetections = EMPTY_SAMPLE;
    other.samplingDistance = ZERO;
}


void ProcessedData::swap(ProcessedData& current, ProcessedData& other) noexcept {
    NativeData::swap(current, other);
    std::swap(current.discreteDetections, other.discreteDetections);
    std::swap(current.samplingDistance, other.samplingDistance);
}

Sample const& ProcessedData::getDiscreteDetections() const {
    return discreteDetections;
}

uint16_t const& ProcessedData::getSamplingDistance() const {
    return samplingDistance;
}

bool ProcessedData::operator==(ProcessedData const& other) const {
    auto sameNativeData = static_cast<NativeData>(*this).operator==(static_cast<NativeData>(other));
    auto sameDiscreteDetection = getDiscreteDetections() == other.getDiscreteDetections();
    auto sameSamplingDistance = getSamplingDistance() == other.getSamplingDistance();

    bool equal = (sameNativeData && sameDiscreteDetection && sameSamplingDistance);
    return equal;
}

bool ProcessedData::operator!=(ProcessedData const& other) const {
    return !operator==(other);
}

ProcessedData const& ProcessedData::returnDefaultData() noexcept {
    return Defaults::DEFAULT_PROCESSED_DATA;
}

