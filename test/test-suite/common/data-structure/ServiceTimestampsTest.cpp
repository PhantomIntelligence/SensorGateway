#ifndef SENSORGATEWAY_SERVICETIMESTAMPSTEST_CPP
#define SENSORGATEWAY_SERVICETIMESTAMPSTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <test/utilities/data-model/DataModelFixture.h>
#include <test/utilities/assertion/TimeAssertion.hpp>
#include "sensor-gateway/common/data-structure/time-tracking/ServiceTimestamps.hpp"

class ServiceTimestampsTest : public ::testing::Test {

protected:

    static size_t const NUMBER_OF_TIME_POINTS = 19;

    std::array<std::string, NUMBER_OF_TIME_POINTS> LOCATIONS;

    using ServiceTimestamps = Metrics::ServiceTimestamps<NUMBER_OF_TIME_POINTS>;

    void SetUp() override {
        static auto const LOCATION_STRING_LENGTH = 99;
        for (int locationIndex = 0; locationIndex < NUMBER_OF_TIME_POINTS; ++locationIndex) {
            LOCATIONS[locationIndex] = TestFunctions::DataTestUtil::createRandomStringOfLength(LOCATION_STRING_LENGTH);
        }
    }

    void fillUpServiceTimestamps(ServiceTimestamps* serviceTimestamps) noexcept {
        for (int timePointIndex = 0; timePointIndex < NUMBER_OF_TIME_POINTS; ++timePointIndex) {
            serviceTimestamps->addTimePointForLocation(LOCATIONS[timePointIndex]);
        }
    }
};

TEST_F(ServiceTimestampsTest,
       given__when_getCurrentNumberOfTimePoints_then_returnsZero) {
    ServiceTimestamps serviceTimestamps;

    auto currentNumberOfTimePoints = serviceTimestamps.getCurrentNumberOfTimePoints();

    auto expectedNumberOfTimePoints = 0;
    ASSERT_EQ(currentNumberOfTimePoints, expectedNumberOfTimePoints);
}

TEST_F(ServiceTimestampsTest,
       given_oneAddedTimePointWithALocation_when_getCurrentNumberOfTimePoints_then_returnsOne) {
    ServiceTimestamps serviceTimestamps;
    serviceTimestamps.addTimePointForLocation(LOCATIONS[0]);

    auto currentNumberOfTimePoints = serviceTimestamps.getCurrentNumberOfTimePoints();

    auto expectedNumberOfTimePoints = 1;
    ASSERT_EQ(currentNumberOfTimePoints, expectedNumberOfTimePoints);
}

TEST_F(ServiceTimestampsTest,
       given_oneAddedTimePointWithALocation_when_getTimePoints_then_containsTheLocation) {
    auto expectedLocationName = LOCATIONS[0];
    ServiceTimestamps serviceTimestamps;
    serviceTimestamps.addTimePointForLocation(expectedLocationName);

    auto timePoint = serviceTimestamps.getTimePoints()[0];
    auto actualLocationName = timePoint.location;
    ASSERT_EQ(actualLocationName, expectedLocationName);
}

TEST_F(ServiceTimestampsTest,
       given_oneAddedTimePointWithALocation_when_getTimePoints_then_containsACloseEnoughTimestampFromNow) {
    ServiceTimestamps serviceTimestamps;
    serviceTimestamps.addTimePointForLocation(LOCATIONS[0]);
    auto now = HighResolutionClock::now();

    auto timePoint = serviceTimestamps.getTimePoints()[0];
    auto timePointTimestamp = timePoint.timestamp;

    ASSERT_TRUE(Assert::timeWithinMicrosecondDelta(timePointTimestamp, now, TWO_HUNDRED_FIFTY_NANO_SECONDS));
}

TEST_F(ServiceTimestampsTest,
       given_multipleLocations_when_addTimePoints_then_theTimePointsAreCreatedWithAdequateLocation) {
    ServiceTimestamps serviceTimestamps;
    fillUpServiceTimestamps(&serviceTimestamps);

    auto timePoints = serviceTimestamps.getTimePoints();
    for (int timePointIndex = 0; timePointIndex < NUMBER_OF_TIME_POINTS; ++timePointIndex) {
        ASSERT_EQ(LOCATIONS[timePointIndex], timePoints[timePointIndex].location);
    }

}

TEST_F(ServiceTimestampsTest,
       given_twoAddedTimePointsWithLocations_when_getTimePoints_then_theSecondOneHasALaterTimestampThanTheFirst) {
    ServiceTimestamps serviceTimestamps;
    serviceTimestamps.addTimePointForLocation(LOCATIONS[0]);
    serviceTimestamps.addTimePointForLocation(LOCATIONS[1]);

    auto timePoints = serviceTimestamps.getTimePoints();
    auto firstTimestamp = timePoints[0].timestamp;
    auto secondTimestamp = timePoints[1].timestamp;

    ASSERT_TRUE(Assert::firstOneComesBeforeTheSecondOne(firstTimestamp, secondTimestamp));
}

TEST_F(ServiceTimestampsTest,
       given_aFullCapacityServiceTimestamps_when_addATimePoint_then_throwsAnException) {
    ServiceTimestamps serviceTimestamps;
    fillUpServiceTimestamps(&serviceTimestamps);

    ASSERT_THROW(serviceTimestamps.addTimePointForLocation(LOCATIONS[0]), std::runtime_error);
}

#endif //SENSORGATEWAY_SERVICETIMESTAMPSTEST_CPP
