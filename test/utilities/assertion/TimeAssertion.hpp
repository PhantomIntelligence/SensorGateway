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

#ifndef SENSORGATEWAY_TIMEASSERTION_HPP
#define SENSORGATEWAY_TIMEASSERTION_HPP

#include <gtest/gtest.h>
#include "sensor-gateway/common/ConstantFunctionsDefinition.h"
#include <typeinfo>

namespace {
    DurationInNanoseconds const TWO_HUNDRED_FIFTY_NANO_SECONDS = DurationInNanoseconds(250);
    DurationInNanoseconds const FIVE_HUNDRED_NANO_SECONDS = DurationInNanoseconds(500);
}

namespace Assert {
    static ::testing::AssertionResult
    firstOneComesBeforeTheSecondOne(HighResolutionTimePoint const& first, HighResolutionTimePoint const& second) {
        if (first >= second) {
            return ::testing::AssertionFailure()
                    << "\n Received time order is not respected... \n"
                    << "\t" << first.time_since_epoch().count()
                    << " does not come before "
                    << second.time_since_epoch().count()
                    << ".\n";
        } else {
            return ::testing::AssertionSuccess();
        }
    }

    static ::testing::AssertionResult
    timeWithinMicrosecondDelta(HighResolutionTimePoint const& lhs, HighResolutionTimePoint const& rhs,
                               DurationInNanoseconds const& maximalDelta) {
        DurationInNanoseconds absoluteValuedDelta;
        if (lhs > rhs) {
            absoluteValuedDelta = lhs - rhs;
        } else {
            absoluteValuedDelta = rhs - lhs;
        }

        if (absoluteValuedDelta.count() > maximalDelta.count()) {
            return ::testing::AssertionFailure()
                    << "\n DurationInNanoseconds is bigger than expected."
                    << "\n\tActual duration (absolute value) : " << absoluteValuedDelta.count() << " us"
                    << "\n\tMaximal expected duration        : " << maximalDelta.count() << " us"
                    << "\n";
        } else {
            return ::testing::AssertionSuccess();
        }
    }
}

#endif //SENSORGATEWAY_TIMEASSERTION_HPP
