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

#ifndef SENSORGATEWAY_TIMETRACKING_HPP
#define SENSORGATEWAY_TIMETRACKING_HPP

#include "ServiceTimestamps.hpp"

namespace Metrics {

    /**
     * @brief This TimeTrackingDefinition struct serves to declare types used by <SensorName>TimeTracking classes
     * @tparam S the max number of time points created by a Sensor
     * @tparam G the max number of time points created by the Gateway
     */
    template<std::size_t S, std::size_t G>
    struct TimeTrackingDefinition {
        static std::size_t const NUMBER_OF_SENSOR_TIME_POINTS = S;
        static std::size_t const NUMBER_OF_GATEWAY_TIME_POINTS = G;

        using SensorTimestamps = ServiceTimestamps<NUMBER_OF_SENSOR_TIME_POINTS>;
        using GatewayTimestamps = ServiceTimestamps<NUMBER_OF_GATEWAY_TIME_POINTS>;
    };

    // TODO: For eventual debug/prod compilation, see https://en.cppreference.com/w/cpp/types/conditional
    /**
     * @warning This class is NOT thread-safe!
     */
    template<typename TimeTrackingDefinition>
    class TimeTracking {

    public:

        using SensorTimestamps = typename TimeTrackingDefinition::SensorTimestamps;
        using GatewayTimestamps = typename TimeTrackingDefinition::GatewayTimestamps;

        explicit TimeTracking(SensorTimestamps const& sensorTimestamps,
                              GatewayTimestamps const& gatewayTimestamps) :
                sensorTimestamps(sensorTimestamps),
                gatewayTimestamps(gatewayTimestamps) {}

        explicit TimeTracking() :
                TimeTracking(TimeTracking::returnDefaultData()) {}

        ~TimeTracking() noexcept = default;

        TimeTracking(TimeTracking const& other) :
                TimeTracking(other.sensorTimestamps, other.gatewayTimestamps) {}

        TimeTracking(TimeTracking&& other) noexcept :
                sensorTimestamps(std::move(sensorTimestamps)),
                gatewayTimestamps(std::move(gatewayTimestamps)) {
        }

        TimeTracking& operator=(TimeTracking const& other)& {
            TimeTracking temporary(other);
            swap(*this, temporary);
            return *this;
        }

        TimeTracking& operator=(TimeTracking&& other)& noexcept {
            swap(*this, other);
            return *this;
        }

        void swap(TimeTracking& current, TimeTracking& other) noexcept {
            std::swap(current.content, other.content);
        }

        bool operator==(TimeTracking const& other) const {
            auto sameSensorTimestamps = (sensorTimestamps == other.sensorTimestamps);
            auto sameGatewayTimestamps = (gatewayTimestamps == other.gatewayTimestamps);
            auto sameTimeTracking = (sameSensorTimestamps &&
                                     sameGatewayTimestamps);
            return sameTimeTracking;
        }

        bool operator!=(TimeTracking const& other) const {
            return !operator==(other);
        }

        static TimeTracking const& returnDefaultData() noexcept;

        SensorTimestamps const& getSensorTimestamps() noexcept {
            return sensorTimestamps;
        }

        GatewayTimestamps const& getGatewayTimestamps() noexcept {
            return gatewayTimestamps;
        }

        void addTimePointForSensorWithLocation(std::string const& locationName) {
            sensorTimestamps.addTimePointForLocation(locationName);
        }

        void addTimePointForGatewayWithLocation(std::string const& locationName) {
            gatewayTimestamps.addTimePointForLocation(locationName);
        }

    private:

        SensorTimestamps sensorTimestamps;
        GatewayTimestamps gatewayTimestamps;
    };

    namespace Defaults {
        using Metrics::TimeTracking;

        template<typename TimeTrackingDefinition>
        typename TimeTrackingDefinition::SensorTimestamps const DEFAULT_SENSOR_TIMESTAMPS;

        template<typename TimeTrackingDefinition>
        typename TimeTrackingDefinition::GatewayTimestamps const DEFAULT_GATEWAY_TIMESTAMPS;

        template<typename TimeTrackingDefinition>
        TimeTracking<TimeTrackingDefinition> const DEFAULT_TIME_TRACKING = TimeTracking<TimeTrackingDefinition>(
                DEFAULT_SENSOR_TIMESTAMPS<TimeTrackingDefinition>,
                DEFAULT_GATEWAY_TIMESTAMPS<TimeTrackingDefinition>
        );
    }

    template<typename TimeTrackingDefinition>
    TimeTracking<TimeTrackingDefinition> const& TimeTracking<TimeTrackingDefinition>::returnDefaultData() noexcept {
        return Defaults::DEFAULT_TIME_TRACKING<TimeTrackingDefinition>;
    }
}

#endif //SENSORGATEWAY_TIMETRACKING_HPP
