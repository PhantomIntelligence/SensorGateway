#ifndef SENSORGATEWAY_TIMETRACKINGTEST_CPP
#define SENSORGATEWAY_TIMETRACKINGTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "sensor-gateway/common/data-structure/time-tracking/TimeTracking.hpp"

class TimeTrackingTest : public ::testing::Test {
protected:

    std::string const LOCATION = "The house of the Rose";
    static size_t const NUMBER_OF_SENSOR_TIMESTAMPS = 9;
    static size_t const NUMBER_OF_GATEWAY_TIMESTAMPS = 10;

    using TimeTrackingDefinition = Metrics::TimeTrackingDefinition<NUMBER_OF_SENSOR_TIMESTAMPS, NUMBER_OF_GATEWAY_TIMESTAMPS>;
    using TimeTracking = Metrics::TimeTracking<TimeTrackingDefinition>;
};

TEST_F(TimeTrackingTest,
       given__when_getSensorTimestamps_then_returnsADefaultServiceTimestampsInstance) {
    TimeTracking timeTracking;
    auto sensorTimestamps = timeTracking.getSensorTimestamps();
    TimeTrackingDefinition::SensorTimestamps expectedTimestamps;
    ASSERT_EQ(sensorTimestamps, expectedTimestamps);
}

TEST_F(TimeTrackingTest,
       given__when_getGatewayTimestamps_then_returnsADefaultServiceTimestampsInstance) {
    TimeTracking timeTracking;
    auto gatewayTimestamps = timeTracking.getGatewayTimestamps();
    TimeTrackingDefinition::GatewayTimestamps expectedTimestamps;
    ASSERT_EQ(gatewayTimestamps, expectedTimestamps);
}

TEST_F(TimeTrackingTest,
       given_addedTimestampForSensor_when_getSensorTimestamps_then_returnsSensorTimestampsWithNewTimePointAndCorrectLocation) {
    TimeTracking timeTracking;
    timeTracking.addTimePointForSensorWithLocation(LOCATION);
    auto sensorTimestamps = timeTracking.getSensorTimestamps();
    auto firstLocationName = sensorTimestamps.getTimePoints()[0].location;
    ASSERT_EQ(firstLocationName, LOCATION);
}

TEST_F(TimeTrackingTest,
       given_addedTimestampForGateway_when_getGatewayTimestamps_then_returnsGatewayTimestampsWithNewTimePointAndCorrectLocation) {
    TimeTracking timeTracking;
    timeTracking.addTimePointForGatewayWithLocation(LOCATION);
    auto gatewayTimestamps = timeTracking.getGatewayTimestamps();
    auto firstLocationName = gatewayTimestamps.getTimePoints()[0].location;
    ASSERT_EQ(firstLocationName, LOCATION);
}

#endif //SENSORGATEWAY_TIMETRACKINGTEST_CPP
