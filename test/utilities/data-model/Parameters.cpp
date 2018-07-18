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
#include "Parameters.h"
#include <cassert>


using ExampleDataModel::Parameters;

Parameters::Parameters(uint16_t const& FWHM, uint16_t const& samplingTime) :
        FWHM(FWHM),
        samplingTime(samplingTime) {
}

Parameters::Parameters(Parameters const& other) :
        FWHM(other.FWHM),
        samplingTime(other.samplingTime) {}

Parameters::Parameters(Parameters&& other) noexcept:
        FWHM(std::move(other.FWHM)),
        samplingTime(std::move(other.samplingTime)) {
    other.FWHM = ZERO;
    other.samplingTime = ZERO;
}

void Parameters::swap(Parameters& current, Parameters& other) noexcept {
    std::swap(current.FWHM, other.FWHM);
    std::swap(current.samplingTime, other.samplingTime);
}

uint16_t const& Parameters::getFWHM() const {
    assert(FWHM != 0);
    return FWHM;
}


uint16_t const& Parameters::getSamplingTime() const {
    assert(samplingTime != 0);
    return samplingTime;
}

bool Parameters::operator==(Parameters const& other) const {
    auto sameFWHM = getFWHM() == other.getFWHM();
    auto sameSamplingDistance = getSamplingTime() == other.getSamplingTime();
    return (sameFWHM && sameSamplingDistance);
}

bool Parameters::operator!=(Parameters const& other) const {
    return !operator==(other);
}

Parameters const Parameters::returnDefaultData() noexcept {
    return Defaults::DEFAULT_PARAMETERS;
}
