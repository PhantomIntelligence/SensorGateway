#ifndef SENSORGATEWAY_SENSORACCESSLINKERRORTEST_CPP
#define SENSORGATEWAY_SENSORACCESSLINKERRORTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "sensor-gateway/common/error/SensorAccessLinkError.h"

using ErrorHandling::SensorAccessLinkError;
using ErrorHandling::Severity;
using ErrorHandling::Category;

class SensorAccessLinkErrorTest : public ::testing::Test {

protected:
    SensorAccessLinkErrorTest();

    virtual ~SensorAccessLinkErrorTest();

    SensorAccessLinkError createArbitrarySensorAccessLinkErrorWithSeverity(Severity severity) const noexcept {
        SensorAccessLinkError sensorAccessLinkError(ARBITRARY_ORIGIN,
                                              ARBITRARY_CATEGORY,
                                              severity,
                                              ARBITRARY_ERROR_CODE,
                                              ARBITRARY_ERROR_MESSAGE
        );
        return sensorAccessLinkError;
    }

    SensorAccessLinkError createArbitrarySensorAccessLinkErrorWithCategory(Category category) const noexcept {
        SensorAccessLinkError sensorAccessLinkError(ARBITRARY_ORIGIN,
                                              category,
                                              ARBITRARY_SEVERITY,
                                              ARBITRARY_ERROR_CODE,
                                              ARBITRARY_ERROR_MESSAGE
        );
        return sensorAccessLinkError;
    }

    std::string const ARBITRARY_ORIGIN = "Origin";
    ErrorHandling::ErrorCode const ARBITRARY_ERROR_CODE = ErrorHandling::GatewayErrorCode::EMPTY_CODE;
    std::string const ARBITRARY_ERROR_MESSAGE = "Help! I need somebody!";
    Category const ARBITRARY_CATEGORY = Category::UNRECOGNIZED_ERROR;
    Severity const ARBITRARY_SEVERITY = Severity::DEBUG;
};

// TODO : If error = ALARM, do no reopen
SensorAccessLinkErrorTest::SensorAccessLinkErrorTest() {
}

SensorAccessLinkErrorTest::~SensorAccessLinkErrorTest() {}

TEST_F(SensorAccessLinkErrorTest, given_anErrorWithEmergencySeverityLevel_when_askedIfIsFatalForGateway_then_returnsTrue) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::EMERGENCY);
    auto isFatalForGateway = sensorAccessLinkError.isFatalForGateway();
    ASSERT_TRUE(isFatalForGateway);
}

TEST_F(SensorAccessLinkErrorTest, given_anErrorWithAlertSeverityLevel_when_askedIfIsFatalForGateway_then_returnsFalse) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::ALERT);
    auto isFatalForGateway = sensorAccessLinkError.isFatalForGateway();
    ASSERT_FALSE(isFatalForGateway);
}

TEST_F(SensorAccessLinkErrorTest, given_anErrorWithCriticalSeverityLevel_when_askedIfIsFatalForGateway_then_returnsFalse) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::CRITICAL);
    auto isFatalForGateway = sensorAccessLinkError.isFatalForGateway();
    ASSERT_FALSE(isFatalForGateway);
}

TEST_F(SensorAccessLinkErrorTest, given_anErrorWithErrorSeverityLevel_when_askedIfIsFatalForGateway_then_returnsFalse) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::ERROR);
    auto isFatalForGateway = sensorAccessLinkError.isFatalForGateway();
    ASSERT_FALSE(isFatalForGateway);
}

TEST_F(SensorAccessLinkErrorTest, given_anErrorWithWarningSeverityLevel_when_askedIfIsFatalForGateway_then_returnsFalse) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::WARNING);
    auto isFatalForGateway = sensorAccessLinkError.isFatalForGateway();
    ASSERT_FALSE(isFatalForGateway);
}

TEST_F(SensorAccessLinkErrorTest, given_anErrorWithNoticeSeverityLevel_when_askedIfIsFatalForGateway_then_returnsFalse) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::NOTICE);
    auto isFatalForGateway = sensorAccessLinkError.isFatalForGateway();
    ASSERT_FALSE(isFatalForGateway);
}

TEST_F(SensorAccessLinkErrorTest, given_anErrorWithInfoSeverityLevel_when_askedIfIsFatalForGateway_then_returnsFalse) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::INFO);
    auto isFatalForGateway = sensorAccessLinkError.isFatalForGateway();
    ASSERT_FALSE(isFatalForGateway);
}

TEST_F(SensorAccessLinkErrorTest, given_anErrorWithDebugSeverityLevel_when_askedIfIsFatalForGateway_then_returnsFalse) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::DEBUG);
    auto isFatalForGateway = sensorAccessLinkError.isFatalForGateway();
    ASSERT_FALSE(isFatalForGateway);
}

TEST_F(SensorAccessLinkErrorTest, given_anErrorWithEmergencySeverityLevel_when_askedIfIsFatalForSensorAccess_then_returnsTrue) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::EMERGENCY);
    auto isFatalForSensorAccess = sensorAccessLinkError.isFatalForSensorAccess();
    ASSERT_TRUE(isFatalForSensorAccess);
}

TEST_F(SensorAccessLinkErrorTest, given_anErrorWithAlertSeverityLevel_when_askedIfIsFatalForSensorAccess_then_returnsTrue) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::ALERT);
    auto isFatalForSensorAccess = sensorAccessLinkError.isFatalForSensorAccess();
    ASSERT_TRUE(isFatalForSensorAccess);
}

TEST_F(SensorAccessLinkErrorTest, given_anErrorWithCriticalSeverityLevel_when_askedIfIsFatalForSensorAccess_then_returnsFalse) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::CRITICAL);
    auto isFatalForSensorAccess = sensorAccessLinkError.isFatalForSensorAccess();
    ASSERT_FALSE(isFatalForSensorAccess);
}

TEST_F(SensorAccessLinkErrorTest, given_anErrorWithErrorSeverityLevel_when_askedIfIsFatalForSensorAccess_then_returnsFalse) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::ERROR);
    auto isFatalForSensorAccess = sensorAccessLinkError.isFatalForSensorAccess();
    ASSERT_FALSE(isFatalForSensorAccess);
}

TEST_F(SensorAccessLinkErrorTest, given_anErrorWithWarningSeverityLevel_when_askedIfIsFatalForSensorAccess_then_returnsFalse) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::WARNING);
    auto isFatalForSensorAccess = sensorAccessLinkError.isFatalForSensorAccess();
    ASSERT_FALSE(isFatalForSensorAccess);
}

TEST_F(SensorAccessLinkErrorTest, given_anErrorWithNoticeSeverityLevel_when_askedIfIsFatalForSensorAccess_then_returnsFalse) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::NOTICE);
    auto isFatalForSensorAccess = sensorAccessLinkError.isFatalForSensorAccess();
    ASSERT_FALSE(isFatalForSensorAccess);
}

TEST_F(SensorAccessLinkErrorTest, given_anErrorWithInfoSeverityLevel_when_askedIfIsFatalForSensorAccess_then_returnsFalse) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::INFO);
    auto isFatalForSensorAccess = sensorAccessLinkError.isFatalForSensorAccess();
    ASSERT_FALSE(isFatalForSensorAccess);
}

TEST_F(SensorAccessLinkErrorTest, given_anErrorWithDebugSeverityLevel_when_askedIfIsFatalForSensorAccess_then_returnsFalse) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::DEBUG);
    auto isFatalForSensorAccess = sensorAccessLinkError.isFatalForSensorAccess();
    ASSERT_FALSE(isFatalForSensorAccess);
}

TEST_F(SensorAccessLinkErrorTest,
       given_anErrorWithEmergencySeverityLevel_when_askedIfCloseConnectionIsRequired_then_returnsTrue) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::EMERGENCY);
    auto closeConnectionIsRequired = sensorAccessLinkError.isCloseConnectionRequired();
    ASSERT_TRUE(closeConnectionIsRequired);
}

TEST_F(SensorAccessLinkErrorTest,
       given_anErrorWithConnectionErrorCategory_when_askedIfCloseConnectionIsRequired_then_returnsTrue) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithCategory(Category::CONNECTION_ERROR);
    auto closeConnectionIsRequired = sensorAccessLinkError.isCloseConnectionRequired();
    ASSERT_TRUE(closeConnectionIsRequired);
}

TEST_F(SensorAccessLinkErrorTest,
       given_anErrorWithSensorAccessLinkErrorCategory_when_askedIfCloseConnectionIsRequired_then_returnsFalse) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithCategory(Category::COMMUNICATION_ERROR);
    auto closeConnectionIsRequired = sensorAccessLinkError.isCloseConnectionRequired();
    ASSERT_FALSE(closeConnectionIsRequired);
}

TEST_F(SensorAccessLinkErrorTest,
       given_anErrorWithUnhandledErrorCategory_when_askedIfCloseConnectionIsRequired_then_returnsTrue) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithCategory(Category::UNHANDLED_ERROR);
    auto closeConnectionIsRequired = sensorAccessLinkError.isCloseConnectionRequired();
    ASSERT_TRUE(closeConnectionIsRequired);
}

TEST_F(SensorAccessLinkErrorTest,
       given_anErrorWithExternalErrorCategory_when_askedIfCloseConnectionIsRequired_then_returnsTrue) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithCategory(Category::EXTERNAL_ERROR);
    auto closeConnectionIsRequired = sensorAccessLinkError.isCloseConnectionRequired();
    ASSERT_TRUE(closeConnectionIsRequired);
}

TEST_F(SensorAccessLinkErrorTest,
       given_anErrorWithUnrecognizedErrorCategory_when_askedIfCloseConnectionIsRequired_then_returnsTrue) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithCategory(Category::UNRECOGNIZED_ERROR);
    auto closeConnectionIsRequired = sensorAccessLinkError.isCloseConnectionRequired();
    ASSERT_TRUE(closeConnectionIsRequired);
}

TEST_F(SensorAccessLinkErrorTest,
       given_anErrorWithEmergencySeverityLevel_when_askedIfOpenConnectionIsRequired_then_returnsFalse) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::EMERGENCY);
    auto openConnectionIsRequired = sensorAccessLinkError.isOpenConnectionRequired();
    ASSERT_FALSE(openConnectionIsRequired);
}

TEST_F(SensorAccessLinkErrorTest,
       given_anErrorWithAlertSeverityLevel_when_askedIfOpenConnectionIsRequired_then_returnsFalse) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithSeverity(Severity::ALERT);
    auto openConnectionIsRequired = sensorAccessLinkError.isOpenConnectionRequired();
    ASSERT_FALSE(openConnectionIsRequired);
}

TEST_F(SensorAccessLinkErrorTest,
       given_anErrorWithConnectionErrorCategory_when_askedIfOpenConnectionIsRequired_then_returnsTrue) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithCategory(Category::CONNECTION_ERROR);
    auto openConnectionIsRequired = sensorAccessLinkError.isOpenConnectionRequired();
    ASSERT_TRUE(openConnectionIsRequired);
}

TEST_F(SensorAccessLinkErrorTest,
       given_anErrorWithSensorAccessLinkErrorCategory_when_askedIfOpenConnectionIsRequired_then_returnsFalse) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithCategory(Category::COMMUNICATION_ERROR);
    auto openConnectionIsRequired = sensorAccessLinkError.isOpenConnectionRequired();
    ASSERT_FALSE(openConnectionIsRequired);
}

TEST_F(SensorAccessLinkErrorTest,
       given_anErrorWithUnhandledErrorCategory_when_askedIfOpenConnectionIsRequired_then_returnsTrue) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithCategory(Category::UNHANDLED_ERROR);
    auto openConnectionIsRequired = sensorAccessLinkError.isOpenConnectionRequired();
    ASSERT_TRUE(openConnectionIsRequired);
}

TEST_F(SensorAccessLinkErrorTest,
       given_anErrorWithExternalErrorCategory_when_askedIfOpenConnectionIsRequired_then_returnsTrue) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithCategory(Category::EXTERNAL_ERROR);
    auto openConnectionIsRequired = sensorAccessLinkError.isOpenConnectionRequired();
    ASSERT_TRUE(openConnectionIsRequired);
}

TEST_F(SensorAccessLinkErrorTest,
       given_anErrorWithUnrecognizedErrorCategory_when_askedIfOpenConnectionIsRequired_then_returnsTrue) {
    auto sensorAccessLinkError = createArbitrarySensorAccessLinkErrorWithCategory(Category::UNRECOGNIZED_ERROR);
    auto openConnectionIsRequired = sensorAccessLinkError.isOpenConnectionRequired();
    ASSERT_TRUE(openConnectionIsRequired);
}

#endif //SENSORGATEWAY_SENSORACCESSLINKERRORTEST_CPP
