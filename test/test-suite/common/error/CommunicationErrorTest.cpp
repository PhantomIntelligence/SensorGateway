#ifndef SENSORGATEWAY_COMMUNICATIONERRORTEST_CPP
#define SENSORGATEWAY_COMMUNICATIONERRORTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "sensor-gateway/common/error/CommunicationError.h"

using ErrorHandling::CommunicationError;
using ErrorHandling::Severity;
using ErrorHandling::Category;

class CommunicationErrorTest : public ::testing::Test {

protected:
    CommunicationErrorTest();

    virtual ~CommunicationErrorTest();

    CommunicationError createArbitraryCommunicationErrorWithSeverity(Severity severity) const noexcept {
        CommunicationError communicationError(ARBITRARY_ORIGIN,
                                              ARBITRARY_CATEGORY,
                                              severity,
                                              ARBITRARY_ERROR_CODE,
                                              ARBITRARY_ERROR_MESSAGE
        );
        return communicationError;
    }

    CommunicationError createArbitraryCommunicationErrorWithCategory(Category category) const noexcept {
        CommunicationError communicationError(ARBITRARY_ORIGIN,
                                              category,
                                              ARBITRARY_SEVERITY,
                                              ARBITRARY_ERROR_CODE,
                                              ARBITRARY_ERROR_MESSAGE
        );
        return communicationError;
    }

    std::string const ARBITRARY_ORIGIN = "Origin";
    CommunicationError::ErrorCode const ARBITRARY_ERROR_CODE = -1;
    std::string const ARBITRARY_ERROR_MESSAGE = "Help! I need somebody!";
    Category const ARBITRARY_CATEGORY = Category::UNRECOGNIZED_ERROR;
    Severity const ARBITRARY_SEVERITY = Severity::DEBUG;
};

CommunicationErrorTest::CommunicationErrorTest() {
}

CommunicationErrorTest::~CommunicationErrorTest() {}


TEST_F(CommunicationErrorTest, given_anErrorWithEmergencySeverityLevel_when_askedIfIsFatal_then_returnsTrue) {
    auto communicationError = createArbitraryCommunicationErrorWithSeverity(Severity::EMERGENCY);
    auto isFatal = communicationError.isFatal();
    ASSERT_TRUE(isFatal);
}

TEST_F(CommunicationErrorTest, given_anErrorWithAlertSeverityLevel_when_askedIfIsFatal_then_returnsFalse) {
    auto communicationError = createArbitraryCommunicationErrorWithSeverity(Severity::ALERT);
    auto isFatal = communicationError.isFatal();
    ASSERT_FALSE(isFatal);
}

TEST_F(CommunicationErrorTest, given_anErrorWithCriticalSeverityLevel_when_askedIfIsFatal_then_returnsFalse) {
    auto communicationError = createArbitraryCommunicationErrorWithSeverity(Severity::CRITICAL);
    auto isFatal = communicationError.isFatal();
    ASSERT_FALSE(isFatal);
}

TEST_F(CommunicationErrorTest, given_anErrorWithErrorSeverityLevel_when_askedIfIsFatal_then_returnsFalse) {
    auto communicationError = createArbitraryCommunicationErrorWithSeverity(Severity::ERROR);
    auto isFatal = communicationError.isFatal();
    ASSERT_FALSE(isFatal);
}

TEST_F(CommunicationErrorTest, given_anErrorWithWarningSeverityLevel_when_askedIfIsFatal_then_returnsFalse) {
    auto communicationError = createArbitraryCommunicationErrorWithSeverity(Severity::WARNING);
    auto isFatal = communicationError.isFatal();
    ASSERT_FALSE(isFatal);
}

TEST_F(CommunicationErrorTest, given_anErrorWithNoticeSeverityLevel_when_askedIfIsFatal_then_returnsFalse) {
    auto communicationError = createArbitraryCommunicationErrorWithSeverity(Severity::NOTICE);
    auto isFatal = communicationError.isFatal();
    ASSERT_FALSE(isFatal);
}

TEST_F(CommunicationErrorTest, given_anErrorWithInfoSeverityLevel_when_askedIfIsFatal_then_returnsFalse) {
    auto communicationError = createArbitraryCommunicationErrorWithSeverity(Severity::INFO);
    auto isFatal = communicationError.isFatal();
    ASSERT_FALSE(isFatal);
}

TEST_F(CommunicationErrorTest, given_anErrorWithDebugSeverityLevel_when_askedIfIsFatal_then_returnsFalse) {
    auto communicationError = createArbitraryCommunicationErrorWithSeverity(Severity::DEBUG);
    auto isFatal = communicationError.isFatal();
    ASSERT_FALSE(isFatal);
}

TEST_F(CommunicationErrorTest,
       given_anErrorWithEmergencySeverityLevel_when_askedIfCloseConnectionIsRequired_then_returnsTrue) {
    auto communicationError = createArbitraryCommunicationErrorWithSeverity(Severity::EMERGENCY);
    auto closeConnectionIsRequired = communicationError.isCloseConnectionRequired();
    ASSERT_TRUE(closeConnectionIsRequired);
}

TEST_F(CommunicationErrorTest,
       given_anErrorWithConnectionErrorCategory_when_askedIfCloseConnectionIsRequired_then_returnsTrue) {
    auto communicationError = createArbitraryCommunicationErrorWithCategory(Category::CONNECTION_ERROR);
    auto closeConnectionIsRequired = communicationError.isCloseConnectionRequired();
    ASSERT_TRUE(closeConnectionIsRequired);
}

TEST_F(CommunicationErrorTest,
       given_anErrorWithCommunicationErrorCategory_when_askedIfCloseConnectionIsRequired_then_returnsFalse) {
    auto communicationError = createArbitraryCommunicationErrorWithCategory(Category::COMMUNICATION_ERROR);
    auto closeConnectionIsRequired = communicationError.isCloseConnectionRequired();
    ASSERT_FALSE(closeConnectionIsRequired);
}

TEST_F(CommunicationErrorTest,
       given_anErrorWithUnhandledErrorCategory_when_askedIfCloseConnectionIsRequired_then_returnsTrue) {
    auto communicationError = createArbitraryCommunicationErrorWithCategory(Category::UNHANDLED_ERROR);
    auto closeConnectionIsRequired = communicationError.isCloseConnectionRequired();
    ASSERT_TRUE(closeConnectionIsRequired);
}

TEST_F(CommunicationErrorTest,
       given_anErrorWithExternalErrorCategory_when_askedIfCloseConnectionIsRequired_then_returnsTrue) {
    auto communicationError = createArbitraryCommunicationErrorWithCategory(Category::EXTERNAL_ERROR);
    auto closeConnectionIsRequired = communicationError.isCloseConnectionRequired();
    ASSERT_TRUE(closeConnectionIsRequired);
}

TEST_F(CommunicationErrorTest,
       given_anErrorWithUnrecognizedErrorCategory_when_askedIfCloseConnectionIsRequired_then_returnsTrue) {
    auto communicationError = createArbitraryCommunicationErrorWithCategory(Category::UNRECOGNIZED_ERROR);
    auto closeConnectionIsRequired = communicationError.isCloseConnectionRequired();
    ASSERT_TRUE(closeConnectionIsRequired);
}

TEST_F(CommunicationErrorTest,
       given_anErrorWithEmergencySeverityLevel_when_askedIfOpenConnectionIsRequired_then_returnsFalse) {
    auto communicationError = createArbitraryCommunicationErrorWithSeverity(Severity::EMERGENCY);
    auto openConnectionIsRequired = communicationError.isOpenConnectionRequired();
    ASSERT_FALSE(openConnectionIsRequired);
}

TEST_F(CommunicationErrorTest,
       given_anErrorWithConnectionErrorCategory_when_askedIfOpenConnectionIsRequired_then_returnsTrue) {
    auto communicationError = createArbitraryCommunicationErrorWithCategory(Category::CONNECTION_ERROR);
    auto openConnectionIsRequired = communicationError.isOpenConnectionRequired();
    ASSERT_TRUE(openConnectionIsRequired);
}

TEST_F(CommunicationErrorTest,
       given_anErrorWithCommunicationErrorCategory_when_askedIfOpenConnectionIsRequired_then_returnsFalse) {
    auto communicationError = createArbitraryCommunicationErrorWithCategory(Category::COMMUNICATION_ERROR);
    auto openConnectionIsRequired = communicationError.isOpenConnectionRequired();
    ASSERT_FALSE(openConnectionIsRequired);
}

TEST_F(CommunicationErrorTest,
       given_anErrorWithUnhandledErrorCategory_when_askedIfOpenConnectionIsRequired_then_returnsTrue) {
    auto communicationError = createArbitraryCommunicationErrorWithCategory(Category::UNHANDLED_ERROR);
    auto openConnectionIsRequired = communicationError.isOpenConnectionRequired();
    ASSERT_TRUE(openConnectionIsRequired);
}

TEST_F(CommunicationErrorTest,
       given_anErrorWithExternalErrorCategory_when_askedIfOpenConnectionIsRequired_then_returnsTrue) {
    auto communicationError = createArbitraryCommunicationErrorWithCategory(Category::EXTERNAL_ERROR);
    auto openConnectionIsRequired = communicationError.isOpenConnectionRequired();
    ASSERT_TRUE(openConnectionIsRequired);
}

TEST_F(CommunicationErrorTest,
       given_anErrorWithUnrecognizedErrorCategory_when_askedIfOpenConnectionIsRequired_then_returnsTrue) {
    auto communicationError = createArbitraryCommunicationErrorWithCategory(Category::UNRECOGNIZED_ERROR);
    auto openConnectionIsRequired = communicationError.isOpenConnectionRequired();
    ASSERT_TRUE(openConnectionIsRequired);
}

TEST_F(CommunicationErrorTest,
       given_anErrorWithEmergencySeverityLevel_when_askedIfResumeCommunicationIsRequired_then_returnsFalse) {
    auto communicationError = createArbitraryCommunicationErrorWithSeverity(Severity::EMERGENCY);
    auto closeConnectionIsRequired = communicationError.isResumeCommunicationRequired();
    ASSERT_FALSE(closeConnectionIsRequired);
}
TEST_F(CommunicationErrorTest, given_anErrorWithAlertSeverityLevel_when_askedIfResumeCommunicationIsRequired_then_returnsTrue) {
    auto communicationError = createArbitraryCommunicationErrorWithSeverity(Severity::ALERT);
    auto isResumeCommunicationRequired = communicationError.isResumeCommunicationRequired();
    ASSERT_FALSE(isResumeCommunicationRequired);
}

TEST_F(CommunicationErrorTest, given_anErrorWithCriticalSeverityLevel_when_askedIfResumeCommunicationIsRequired_then_returnsTrue) {
    auto communicationError = createArbitraryCommunicationErrorWithSeverity(Severity::CRITICAL);
    auto isResumeCommunicationRequired = communicationError.isResumeCommunicationRequired();
    ASSERT_FALSE(isResumeCommunicationRequired);
}

TEST_F(CommunicationErrorTest, given_anErrorWithErrorSeverityLevel_when_askedIfResumeCommunicationIsRequired_then_returnsTrue) {
    auto communicationError = createArbitraryCommunicationErrorWithSeverity(Severity::ERROR);
    auto isResumeCommunicationRequired = communicationError.isResumeCommunicationRequired();
    ASSERT_FALSE(isResumeCommunicationRequired);
}

TEST_F(CommunicationErrorTest, given_anErrorWithWarningSeverityLevel_when_askedIfResumeCommunicationIsRequired_then_returnsTrue) {
    auto communicationError = createArbitraryCommunicationErrorWithSeverity(Severity::WARNING);
    auto isResumeCommunicationRequired = communicationError.isResumeCommunicationRequired();
    ASSERT_FALSE(isResumeCommunicationRequired);
}

TEST_F(CommunicationErrorTest, given_anErrorWithNoticeSeverityLevel_when_askedIfResumeCommunicationIsRequired_then_returnsTrue) {
    auto communicationError = createArbitraryCommunicationErrorWithSeverity(Severity::NOTICE);
    auto isResumeCommunicationRequired = communicationError.isResumeCommunicationRequired();
    ASSERT_FALSE(isResumeCommunicationRequired);
}

TEST_F(CommunicationErrorTest, given_anErrorWithInfoSeverityLevel_when_askedIfResumeCommunicationIsRequired_then_returnsTrue) {
    auto communicationError = createArbitraryCommunicationErrorWithSeverity(Severity::INFO);
    auto isResumeCommunicationRequired = communicationError.isResumeCommunicationRequired();
    ASSERT_FALSE(isResumeCommunicationRequired);
}

TEST_F(CommunicationErrorTest, given_anErrorWithDebugSeverityLevel_when_askedIfResumeCommunicationIsRequired_then_returnsTrue) {
    auto communicationError = createArbitraryCommunicationErrorWithSeverity(Severity::DEBUG);
    auto isResumeCommunicationRequired = communicationError.isResumeCommunicationRequired();
    ASSERT_FALSE(isResumeCommunicationRequired);
}

#endif //SENSORGATEWAY_COMMUNICATIONERRORTEST_CPP
