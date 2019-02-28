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

#ifndef SENSORGATEWAY_TIMEPOINTS_CPP
#define SENSORGATEWAY_TIMEPOINTS_CPP

#include "TimePoint.h"

using Metrics::TimePoint;

TimePoint::TimePoint(HighResolutionTimePoint const& timestamp,
                     std::string location) :
        timestamp(timestamp),
        location(std::move(location)) {}

TimePoint::TimePoint() : TimePoint(returnDefaultData()) {}

TimePoint::TimePoint(TimePoint const& other) :
        timestamp(other.timestamp),
        location(other.location) {}

TimePoint::TimePoint(TimePoint&& other) noexcept :
        timestamp(std::move(other.timestamp)),
        location(std::move(other.location)) {
}

TimePoint& TimePoint::operator=(TimePoint const& other)& {
    TimePoint temporary(other);
    swap(*this, temporary);
    return *this;
}

TimePoint& TimePoint::operator=(TimePoint&& other)& noexcept {
    swap(*this, other);
    return *this;
}

void TimePoint::swap(TimePoint& current, TimePoint& other) noexcept {
    std::swap(current.timestamp, other.timestamp);
    std::swap(current.location, other.location);
}

bool TimePoint::operator==(TimePoint const& other) const {
    bool sameTimestamp = timestamp == other.timestamp;
    bool sameLocation = location == other.location;
    auto sameTimePoint = (sameTimestamp &&
                          sameLocation);
    return sameTimePoint;
}

bool TimePoint::operator!=(TimePoint const& other) const {
    return !operator==(other);
}

TimePoint const& TimePoint::returnDefaultData() noexcept {
    return Defaults::DEFAULT_TIME_POINT;
}

#endif //SENSORGATEWAY_TIMEPOINTS_CPP
