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

#ifndef SENSORGATEWAY_TIMEPOINTS_H
#define SENSORGATEWAY_TIMEPOINTS_H

#include "sensor-gateway/common/ConstantFunctionsDefinition.h"

namespace Metrics {

    class TimePoint {

    public:

        explicit TimePoint(HighResolutionTimePoint const& timestamp, std::string location);

        explicit TimePoint();

        ~TimePoint() noexcept = default;

        TimePoint(TimePoint const& other);

        TimePoint(TimePoint&& other) noexcept;

        TimePoint& operator=(TimePoint const& other)&;

        TimePoint& operator=(TimePoint&& other)& noexcept;

        void swap(TimePoint& current, TimePoint& other) noexcept;

        bool operator==(TimePoint const& other) const;

        bool operator!=(TimePoint const& other) const;

        static TimePoint const& returnDefaultData() noexcept;

        HighResolutionTimePoint timestamp;
        std::string location;

    };

    namespace Defaults {
        using Metrics::TimePoint;

        HighResolutionTimePoint const DEFAULT_TIMESTAMP = BEGINNING_OF_TIME_ITSELF;

        std::string const DEFAULT_LOCATION_NAME;

        TimePoint const DEFAULT_TIME_POINT = TimePoint(DEFAULT_TIMESTAMP, DEFAULT_LOCATION_NAME);
    }

}

#endif //SENSORGATEWAY_TIMEPOINTS_H
