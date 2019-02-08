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

#ifndef SENSORGATEWAY_SERVICETIMESTAMPS_HPP
#define SENSORGATEWAY_SERVICETIMESTAMPS_HPP

#include "sensor-gateway/common/error/SensorAccessLinkError.h"
#include "TimePoint.h"

namespace Metrics {

    /**
     * @warning The reading and writing of different timestamps is NOT thread-safe!
     */
    template<std::size_t N>
    class ServiceTimestamps {

    public:

        static std::size_t const MAXIMUM_SIZE = N;

        using TimePoints = std::array<Metrics::TimePoint, MAXIMUM_SIZE>;
        using Counter = uint;

        explicit ServiceTimestamps(TimePoints timePoints,
                                   Counter const& numberOfTimePoints) :
                timePoints(std::move(timePoints)),
                currentNumberOfTimePoints(numberOfTimePoints) {}

        explicit ServiceTimestamps() :
                ServiceTimestamps(ServiceTimestamps<N>::returnDefaultData()) {}

        ~ServiceTimestamps() noexcept = default;

        ServiceTimestamps(ServiceTimestamps const& other) :
                timePoints(other.timePoints),
                currentNumberOfTimePoints(other.currentNumberOfTimePoints) {}


        ServiceTimestamps(ServiceTimestamps&& other) noexcept :
                timePoints(std::move(other.timePoints)),
                currentNumberOfTimePoints(std::move(other.currentNumberOfTimePoints)) {
        }

        ServiceTimestamps& operator=(ServiceTimestamps const& other)& {
            ServiceTimestamps temporary(other);
            swap(*this, temporary);
            return *this;
        }

        ServiceTimestamps& operator=(ServiceTimestamps&& other)& noexcept {
            swap(*this, other);
            return *this;
        }

        static void swap(ServiceTimestamps& current, ServiceTimestamps& other) noexcept {
            std::swap(current.timePoints, other.timePoints);
            std::swap(current.currentNumberOfTimePoints, other.currentNumberOfTimePoints);
        }

        bool operator==(ServiceTimestamps const& other) const {
            auto sameTimePoints = (timePoints == other.timePoints);
            auto sameNumberOfTimePoints = (currentNumberOfTimePoints == other.currentNumberOfTimePoints);
            bool sameTimestamps = (
                    sameTimePoints &&
                    sameNumberOfTimePoints
            );
            return sameTimestamps;
        }

        bool operator!=(ServiceTimestamps const& other) const {
            return !operator==(other);
        }

        static ServiceTimestamps const& returnDefaultData() noexcept;

        void addTimePointForLocation(std::string const& locationName) {
            validateNotFull();
            timePoints[currentNumberOfTimePoints].timestamp = HighResolutionClock::now();
            timePoints[currentNumberOfTimePoints].location = locationName;
            ++currentNumberOfTimePoints;
        }

        TimePoints const& getTimePoints() const noexcept {
            return timePoints;
        }

        Counter const& getCurrentNumberOfTimePoints() const noexcept {
            return currentNumberOfTimePoints;
        }

    private:

        void validateNotFull() const {
            if (currentNumberOfTimePoints >= MAXIMUM_SIZE) {
                // TODO : Have a cleaner throw, elsewhere?
                throw ErrorHandling::SensorAccessLinkError(
                        "ServiceTimestamps::addTimePointForLocation",
                        ErrorHandling::Category::TIME_POINTS_ERROR,
                        ErrorHandling::Severity::ERROR,
                        ErrorHandling::GatewayErrorCode::TIME_POINTS_ALREADY_AT_MAXIMUM_CAPACITY,
                        ExceptionMessage::SERVICE_TIMESTAMPS_ILLEGAL_FULL_CAPACITY
                );
            }
        }

        TimePoints timePoints;
        Counter currentNumberOfTimePoints;
    };

    namespace Defaults {
        using Metrics::ServiceTimestamps;

        template<std::size_t N>
        typename ServiceTimestamps<N>::Counter const DEFAULT_NUMBER_OF_TIME_POINTS = 0;

        template<std::size_t N>
        typename ServiceTimestamps<N>::TimePoints const DEFAULT_TIME_POINTS;

        template<std::size_t N>
        ServiceTimestamps<N> const DEFAULT_SERVICE_TIMESTAMPS = ServiceTimestamps<N>(DEFAULT_TIME_POINTS<N>,
                                                                                     DEFAULT_NUMBER_OF_TIME_POINTS<N>);
    }

    template<std::size_t N>
    ServiceTimestamps<N> const& ServiceTimestamps<N>::returnDefaultData() noexcept {
        return Defaults::DEFAULT_SERVICE_TIMESTAMPS<N>;
    }

}

#endif //SENSORGATEWAY_SERVICETIMESTAMPS_HPP
