/**
	Copyright 2014-2019 Phantom Intelligence Inc.

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

#ifndef SENSORGATEWAY_SERVERCOMMUNICATORTEST_CPP
#define SENSORGATEWAY_SERVERCOMMUNICATORTEST_CPP

#include <gtest/gtest.h>

#include "sensor-gateway/server-communication/ServerCommunicator.hpp"
#include "test/utilities/data-model/DataModelFixture.h"
#include "test/utilities/assertion/ErrorAssertion.hpp"
#include "test/utilities/mock/CatchingRequestHandlerMock.hpp"
#include "test/utilities/mock/CatchingServerCommunicationStrategyMock.hpp"
#include "test/utilities/mock/ErrorThrowingServerCommunicationStrategyMock.hpp"
#include "test/utilities/mock/RequestResponseSpecializedServerCommunicationStrategyMock.hpp"
#include "test/utilities/assertion/TimeAssertion.hpp"

using TestFunctions::DataTestUtil;

class ServerCommunicatorTest : public ::testing::Test {

public:

    using RequestAssembler = Assemble::ServerRequestAssembler;
    using ResponseAssembler = Assemble::ServerResponseAssembler;

    using DataStructures = Sensor::Test::Simple::Structures;
    using ParameterListForThisTestCase = DataStructures::Parameters;

    using MockServerCommunicatorStrategy = Mock::CatchingServerCommunicationStrategyMock<DataStructures>;
    using ThrowingServerCommunicationStrategy = Mock::ErrorThrowingServerCommunicationStrategyMock<DataStructures>;
    using RequestResponseServerCommunicationStrategy = Mock::RequestResponseSpecializedServerCommunicationStrategyMock<DataStructures>;

protected:

    using ServerCommunicator = SensorAccessLinkElement::ServerCommunicator<DataStructures>;
    using ServerRequestHandlingCallBacks = typename ServerCommunicator::RequestHandlingCallBacks;
    using MockRequestHandler = Mock::CatchingRequestHandlerMock<DataStructures, DataStructures>;
    using Message = Sensor::Test::Simple::Structures::Message;
    using RawData = Sensor::Test::Simple::Structures::RawData;

    std::string const SERVER_ADDRESS = "I like trains";
    MockRequestHandler defaultRequestHandlerMock;

    using GetParameterValueRequest = ServerCommunication::RequestTypes::GetParameterValue;
    using HandleGetAllParameterNamesRequest = std::function<void()>;
    using HandleGetParameterValueRequest = std::function<void(GetParameterValueRequest&&)>;
    using HandleCalibrationRequest = std::function<void()>;
    using HandleClearCalibrationRequest = std::function<void()>;
    using NoAction = StringLiteral<decltype(""_ToString)>;

    ServerCommunicatorTest() = default;

    virtual ~ServerCommunicatorTest() = default;

    auto givenAnErrorMessageResponse() const {
        auto badRequest = Given::anInvalidGetParameterValueRequest<ParameterListForThisTestCase>();
        badRequest.markAsBadRequest();
        auto badRequestResponse = ResponseAssembler::createErrorMessageResponseFromRequest<NoAction>(
                std::move(badRequest));
        return badRequestResponse;
    }

    auto givenASuccessMessageResponse() const {
        auto request = Given::aValidGetParameterValueRequest<ParameterListForThisTestCase>();
        auto successResponse = ResponseAssembler::createSuccessMessageResponseFromRequest<NoAction>(std::move(request));
        return successResponse;
    }

    auto givenAParameterErrorResponse() const {
        auto validRequest = Given::aValidGetParameterValueRequest<ParameterListForThisTestCase>();
        validRequest.markAsErrorCause();
        ParameterListForThisTestCase availableParameters;
        auto metadata = availableParameters.getMetadataFor((validRequest.payloadToString()));
        auto parameterErrorResponse = Assemble::ServerResponseAssembler::createParameterErrorResponse(metadata,
                                                                                                      std::move(
                                                                                                              validRequest));
        return parameterErrorResponse;
    }

    void
    assertRequestHandlerReceivesExpectedNumberOfValidRequestsWithStrategy(MockRequestHandler* mockRequestHandler,
                                                                          ServerCommunicator* serverCommunicator) {
        serverCommunicator->openConnection(SERVER_ADDRESS);
        mockRequestHandler->waitForExpectedNumberOfGetParameterValueRequest();
        serverCommunicator->terminateAndJoin();

        auto hasReceivedExpectedNumberOfCall = mockRequestHandler
                ->hasReceivedExpectedNumberOfGetParameterValueRequest();

        ASSERT_TRUE(hasReceivedExpectedNumberOfCall);
    }

    auto bindHandlerGetAllParameterNamesTo(MockRequestHandler* handlerMock) -> HandleGetAllParameterNamesRequest {
        return std::bind(&MockRequestHandler::handleGetAllParameterNamesRequest, handlerMock);
    }

    auto bindHandlerGetParameterValueTo(MockRequestHandler* handlerMock) -> HandleGetParameterValueRequest {
        return std::bind(&MockRequestHandler::handleGetParameterValueRequest,
                         handlerMock, std::placeholders::_1);
    }

    auto bindHandlerCalibrationTo(MockRequestHandler* handlerMock) -> HandleCalibrationRequest {
        return std::bind(&MockRequestHandler::handleCalibrationRequest, handlerMock);
    }

    auto bindHandlerClearCalibrationTo(MockRequestHandler* handlerMock) -> HandleClearCalibrationRequest {
        return std::bind(&MockRequestHandler::handleClearCalibrationRequest, handlerMock);
    }

    auto ignoreHandleGetAllParameterNames() -> HandleGetAllParameterNamesRequest {
        return bindHandlerGetAllParameterNamesTo(&defaultRequestHandlerMock);
    }

    auto ignoreHandleGetParameterValue() -> HandleGetParameterValueRequest {
        return bindHandlerGetParameterValueTo(&defaultRequestHandlerMock);
    }

    auto ignoreHandleCalibration() -> HandleCalibrationRequest {
        return bindHandlerCalibrationTo(&defaultRequestHandlerMock);
    }

    auto ignoreHandleClearCalibration() -> HandleClearCalibrationRequest {
        return bindHandlerClearCalibrationTo(&defaultRequestHandlerMock);
    }

    template<typename Response>
    testing::AssertionResult
    closeConnectionIsCalledAfterStrategyThrowsOnSendResponse(ThrowingServerCommunicationStrategy* throwingStrategy,
                                                             Response&& response) noexcept {

        ServerRequestHandlingCallBacks handlingCallBacks = ServerCommunicator::RequestCallBackStore::createCallBacks(
                ignoreHandleGetAllParameterNames(),
                ignoreHandleGetParameterValue(),
                ignoreHandleCalibration(),
                ignoreHandleClearCalibration()

        );
        ServerCommunicator serverCommunicator(throwingStrategy, &handlingCallBacks);

        serverCommunicator.sendResponse(std::forward<Response>(response));
        throwingStrategy->waitUntilCloseConnectionCallIsMadeAfterErrorIsThrown();

        auto closeConnectionCalled = throwingStrategy->hasCloseConnectionBeenCalledAfterThrowingFunction();
        if (closeConnectionCalled) {
            return ::testing::AssertionSuccess();
        } else {
            return ::testing::AssertionFailure()
                    << "\nExpected closeConnection to be call in strategy as a reaction to the thrown error in `sendResponse`: \n"
                    << "\nMake sure the error thrown is correctly formed (see SensorAccessLinkError), as it dictates further handling\n"
                    << "\n";
        }
    }

    template<typename Response>
    testing::AssertionResult
    openConnectionIsCalledAfterStrategyThrowsOnSendResponse(ThrowingServerCommunicationStrategy* throwingStrategy,
                                                            Response&& response) noexcept {

        ServerRequestHandlingCallBacks handlingCallBacks =
                ServerCommunicator::RequestCallBackStore::createCallBacks(
                        ignoreHandleGetAllParameterNames(),
                        ignoreHandleGetParameterValue(),
                        ignoreHandleCalibration(),
                        ignoreHandleClearCalibration()

                );
        ServerCommunicator serverCommunicator(throwingStrategy, &handlingCallBacks);

        serverCommunicator.sendResponse(std::forward<Response>(response));
        throwingStrategy->waitUntilOpenConnectionCallIsMadeAfterErrorIsThrown();

        auto closeConnectionCalled = throwingStrategy->hasOpenConnectionBeenCalledAfterThrowingFunction();
        if (closeConnectionCalled) {
            return ::testing::AssertionSuccess();
        } else {
            return ::testing::AssertionFailure()
                    << "\nExpected openConnection to be call in strategy as a reaction to the thrown error in `sendResponse`: \n"
                    << "\nMake sure the error thrown is correctly formed (see SensorAccessLinkError), as it dictates further handling\n"
                    << "\n";
        }
    }
};

TEST_F(ServerCommunicatorTest, given__when_connect_then_callsOpenConnectionInStrategy) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks);

    serverCommunicator.openConnection(SERVER_ADDRESS);
    auto strategyHasBeenCalled = mockStrategy.hasOpenConnectionBeenCalled();
    serverCommunicator.terminateAndJoin();

    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest, given_aMessageToSend_when_consume_then_callsSendMessageInStrategy) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks);
    auto message = DataTestUtil::createRandomSimpleMessageWithEmptyTimestamps();

    serverCommunicator.consume(std::move(message));

    auto strategyHasBeenCalled = mockStrategy.hasSendMessageBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest, given_aMessageToSend_when_consume_then_callsSendMessageInStrategyWithTheMessage) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks);
    auto message = DataTestUtil::createRandomSimpleMessageWithEmptyTimestamps();
    auto copy = Message(message);

    serverCommunicator.consume(std::move(message));
    auto receivedMessage = mockStrategy.getFirstSentMessage();
    ASSERT_EQ(copy, receivedMessage);
}

/**
 * @metric-test : timestamps
 */
TEST_F(ServerCommunicatorTest, given_aMessageToSend_when_consume_then_sendsItAfterHavingTimestampedIt) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks);
    auto message = DataTestUtil::createRandomSimpleMessageWithEmptyTimestamps();

    auto now = HighResolutionClock::now();
    serverCommunicator.consume(std::move(message));

    auto receivedMessage = mockStrategy.getFirstSentMessage();

    auto timePoint = receivedMessage.getGatewayTimestamps().getTimePoints()[0];
    auto timePointLocation = timePoint.location;
    auto timePointTimestamp = timePoint.timestamp;

    auto sameLocation = timePointLocation == Metrics::LocationNames::SERVER_COMMUNICATOR_SENDING;
    ASSERT_TRUE(sameLocation);
    ASSERT_TRUE(Assert::timeWithinMicrosecondDelta(timePointTimestamp, now, FIVE_HUNDRED_NANO_SECONDS));
}

TEST_F(ServerCommunicatorTest, given_aRawDataCycleToSend_when_consume_then_callsSendRawDataInStrategy) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks);

    auto rawData = DataTestUtil::createRandomSimpleRawData();

    serverCommunicator.consume(std::move(rawData));

    auto strategyHasBeenCalled = mockStrategy.hasSendRawDataBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest,
       given_aRawDataCycleToSend_when_consume_then_callsSendRawDataInStrategyWithTheRawData) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks);
    auto rawData = DataTestUtil::createRandomSimpleRawData();
    auto copy = RawData(rawData);

    serverCommunicator.consume(std::move(rawData));

    auto receivedRawData = mockStrategy.getFirstSentRawData();
    ASSERT_EQ(copy, receivedRawData);
}

TEST_F(ServerCommunicatorTest, given__when_terminateAndJoin_then_callsCloseConnectionInStrategy) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks);

    serverCommunicator.terminateAndJoin();

    auto strategyHasBeenCalled = mockStrategy.hasCloseConnectionBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToCloseThrownOnOpenConnectionCall_when_isCaught_then_callsCloseConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwCloseConnectionRequiredErrorWhenOpenConnectionIsCalled();

    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&throwingMockStrategy, &handlingCallBacks);
    serverCommunicator.openConnection(SERVER_ADDRESS);
    throwingMockStrategy.waitUntilCloseConnectionCallIsMadeAfterErrorIsThrown();

    auto closeConnectionCalled = throwingMockStrategy.hasCloseConnectionBeenCalledAfterThrowingFunction();
    serverCommunicator.terminateAndJoin();

    ASSERT_TRUE(closeConnectionCalled);
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToOpenThrownOnOpenConnectionCall_when_isCaught_then_callsOpenConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenOpenConnectionIsCalled();

    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&throwingMockStrategy, &handlingCallBacks);
    serverCommunicator.openConnection(SERVER_ADDRESS);
    throwingMockStrategy.waitUntilOpenConnectionCallIsMadeAfterErrorIsThrown();

    auto openConnectionCalled = throwingMockStrategy.hasOpenConnectionBeenCalledAfterThrowingFunction();
    serverCommunicator.terminateAndJoin();

    ASSERT_TRUE(openConnectionCalled);
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToCloseThrownOnCloseConnectionCall_when_isCaught_then_callsCloseConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwCloseConnectionRequiredErrorWhenCloseConnectionIsCalled();

    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&throwingMockStrategy, &handlingCallBacks);
    serverCommunicator.terminateAndJoin();
    throwingMockStrategy.waitUntilCloseConnectionCallIsMadeAfterErrorIsThrown();

    auto closeConnectionCalled = throwingMockStrategy.hasCloseConnectionBeenCalledAfterThrowingFunction();

    ASSERT_TRUE(closeConnectionCalled);
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToOpenThrownOnCloseConnectionCall_when_isCaught_then_callsOpenConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenCloseConnectionIsCalled();

    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&throwingMockStrategy, &handlingCallBacks);
    serverCommunicator.terminateAndJoin();
    throwingMockStrategy.waitUntilOpenConnectionCallIsMadeAfterErrorIsThrown();

    auto openConnectionCalled = throwingMockStrategy.hasOpenConnectionBeenCalledAfterThrowingFunction();

    ASSERT_TRUE(openConnectionCalled);
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToCloseThrownOnSendMessageCall_when_isCaught_then_callsCloseConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwCloseConnectionRequiredErrorWhenSendMessageIsCalled();

    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&throwingMockStrategy, &handlingCallBacks);
    auto message = DataTestUtil::createRandomSimpleMessageWithEmptyTimestamps();
    serverCommunicator.consume(std::move(message));
    throwingMockStrategy.waitUntilCloseConnectionCallIsMadeAfterErrorIsThrown();

    auto closeConnectionCalled = throwingMockStrategy.hasCloseConnectionBeenCalledAfterThrowingFunction();

    ASSERT_TRUE(closeConnectionCalled);
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToOpenThrownOnSendMessageCall_when_isCaught_then_callsOpenConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenSendMessageIsCalled();

    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&throwingMockStrategy, &handlingCallBacks);
    auto message = DataTestUtil::createRandomSimpleMessageWithEmptyTimestamps();
    serverCommunicator.consume(std::move(message));
    throwingMockStrategy.waitUntilOpenConnectionCallIsMadeAfterErrorIsThrown();

    auto openConnectionCalled = throwingMockStrategy.hasOpenConnectionBeenCalledAfterThrowingFunction();

    ASSERT_TRUE(openConnectionCalled);
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToCloseThrownOnSendRawDataCall_when_isCaught_then_callsCloseConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwCloseConnectionRequiredErrorWhenSendRawDataIsCalled();

    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&throwingMockStrategy, &handlingCallBacks);
    auto rawData = DataTestUtil::createRandomSimpleRawData();
    serverCommunicator.consume(std::move(rawData));
    throwingMockStrategy.waitUntilCloseConnectionCallIsMadeAfterErrorIsThrown();

    auto closeConnectionCalled = throwingMockStrategy.hasCloseConnectionBeenCalledAfterThrowingFunction();

    ASSERT_TRUE(closeConnectionCalled);
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToOpenThrownOnSendRawDataCall_when_isCaught_then_callsOpenConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenSendRawDataIsCalled();

    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&throwingMockStrategy, &handlingCallBacks);
    auto rawData = DataTestUtil::createRandomSimpleRawData();
    serverCommunicator.consume(std::move(rawData));
    throwingMockStrategy.waitUntilOpenConnectionCallIsMadeAfterErrorIsThrown();

    auto openConnectionCalled = throwingMockStrategy.hasOpenConnectionBeenCalledAfterThrowingFunction();

    ASSERT_TRUE(openConnectionCalled);
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToCloseThrownOnSendAllParameterMetadataResponseCall_when_isCaught_then_callsCloseConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwCloseConnectionRequiredErrorWhenSendAllParameterMetadataResponseIsCalled();
    auto response = Given::anAllParameterMetadataResponse<ParameterListForThisTestCase>();

    ASSERT_TRUE(closeConnectionIsCalledAfterStrategyThrowsOnSendResponse(&throwingMockStrategy, std::move(response)));
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToOpenThrownOnSendAllParameterMetadataResponseCall_when_isCaught_then_callsOpenConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenSendAllParameterMetadataResponseIsCalled();
    auto response = Given::anAllParameterMetadataResponse<ParameterListForThisTestCase>();

    ASSERT_TRUE(openConnectionIsCalledAfterStrategyThrowsOnSendResponse(&throwingMockStrategy, std::move(response)));
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToCloseThrownOnSendUnsignedIntegerParameterResponseCall_when_isCaught_then_callsCloseConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwCloseConnectionRequiredErrorWhenSendUnsignedIntegerParameterValueResponseIsCalled();
    auto response = Given::anUnsignedIntegerParameterValueResponse(1u);

    ASSERT_TRUE(closeConnectionIsCalledAfterStrategyThrowsOnSendResponse(&throwingMockStrategy, std::move(response)));
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToOpenThrownOnSendUnsignedIntegerParameterResponseCall_when_isCaught_then_callsOpenConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenSendUnsignedIntegerParameterValueResponseIsCalled();
    auto response = Given::anUnsignedIntegerParameterValueResponse(16u);

    ASSERT_TRUE(openConnectionIsCalledAfterStrategyThrowsOnSendResponse(&throwingMockStrategy, std::move(response)));
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToCloseThrownOnSendSignedIntegerParameterResponseCall_when_isCaught_then_callsCloseConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwCloseConnectionRequiredErrorWhenSendSignedIntegerParameterValueResponseIsCalled();
    auto response = Given::aSignedIntegerParameterValueResponse(-1);

    ASSERT_TRUE(closeConnectionIsCalledAfterStrategyThrowsOnSendResponse(&throwingMockStrategy, std::move(response)));
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToOpenThrownOnSendSignedIntegerParameterResponseCall_when_isCaught_then_callsOpenConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenSendSignedIntegerParameterValueResponseIsCalled();
    auto response = Given::aSignedIntegerParameterValueResponse(-1);

    ASSERT_TRUE(openConnectionIsCalledAfterStrategyThrowsOnSendResponse(&throwingMockStrategy, std::move(response)));
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToCloseThrownOnSendRealNumberParameterResponseCall_when_isCaught_then_callsCloseConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwCloseConnectionRequiredErrorWhenSendRealNumberParameterValueResponseIsCalled();
    auto response = Given::aRealNumberParameterValueResponse(3.2);

    ASSERT_TRUE(closeConnectionIsCalledAfterStrategyThrowsOnSendResponse(&throwingMockStrategy, std::move(response)));
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToOpenThrownOnSendRealNumberParameterResponseCall_when_isCaught_then_callsOpenConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenSendRealNumberParameterValueResponseIsCalled();
    auto response = Given::aRealNumberParameterValueResponse(3.1415926535);

    ASSERT_TRUE(openConnectionIsCalledAfterStrategyThrowsOnSendResponse(&throwingMockStrategy, std::move(response)));
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToCloseThrownOnSendBooleanParameterResponseCall_when_isCaught_then_callsCloseConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwCloseConnectionRequiredErrorWhenSendBooleanParameterValueResponseIsCalled();
    auto response = Given::aBooleanParameterValueResponse(false);

    ASSERT_TRUE(closeConnectionIsCalledAfterStrategyThrowsOnSendResponse(&throwingMockStrategy, std::move(response)));
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToOpenThrownOnSendBooleanParameterResponseCall_when_isCaught_then_callsOpenConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenSendBooleanParameterValueResponseIsCalled();
    auto response = Given::aBooleanParameterValueResponse(true);

    ASSERT_TRUE(openConnectionIsCalledAfterStrategyThrowsOnSendResponse(&throwingMockStrategy, std::move(response)));
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToCloseThrownOnSendParameterErrorResponseCall_when_isCaught_then_callsCloseConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwCloseConnectionRequiredErrorWhenSendParameterErrorResponseIsCalled();
    auto response = givenAParameterErrorResponse();

    ASSERT_TRUE(closeConnectionIsCalledAfterStrategyThrowsOnSendResponse(&throwingMockStrategy, std::move(response)));
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToOpenThrownOnSendParameterErrorResponseCall_when_isCaught_then_callsOpenConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenSendParameterErrorResponseIsCalled();
    auto response = givenAParameterErrorResponse();

    ASSERT_TRUE(openConnectionIsCalledAfterStrategyThrowsOnSendResponse(&throwingMockStrategy, std::move(response)));
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToCloseThrownOnSendSuccessMessageResponseCall_when_isCaught_then_callsCloseConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwCloseConnectionRequiredErrorWhenSendSuccessMessageResponseIsCalled();
    auto response = givenASuccessMessageResponse();

    ASSERT_TRUE(closeConnectionIsCalledAfterStrategyThrowsOnSendResponse(&throwingMockStrategy, std::move(response)));
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToOpenThrownOnSendSuccessMessageResponseCall_when_isCaught_then_callsOpenConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenSendSuccessMessageResponseIsCalled();
    auto response = givenASuccessMessageResponse();

    ASSERT_TRUE(openConnectionIsCalledAfterStrategyThrowsOnSendResponse(&throwingMockStrategy, std::move(response)));
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToCloseThrownOnSendErrorMessageResponseCall_when_isCaught_then_callsCloseConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwCloseConnectionRequiredErrorWhenSendErrorMessageResponseIsCalled();
    auto response = givenAnErrorMessageResponse();

    ASSERT_TRUE(closeConnectionIsCalledAfterStrategyThrowsOnSendResponse(&throwingMockStrategy, std::move(response)));
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToOpenThrownOnSendErrorMessageResponseCall_when_isCaught_then_callsOpenConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenSendErrorMessageResponseIsCalled();
    auto response = givenAnErrorMessageResponse();

    ASSERT_TRUE(openConnectionIsCalledAfterStrategyThrowsOnSendResponse(&throwingMockStrategy, std::move(response)));
}

TEST_F(ServerCommunicatorTest,
       given_aStrategyThrowingErrorOnOpenConnectionCall_when_errorIsCaught_then_producesTheErrorWithCorrectOrigin) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwErrorWhenOpenConnectionIsCalled();
    Error expectedError = throwingMockStrategy.expectedErrorWhenOpenConnectionIsCalled();
    expectedError = formatStrategyErrorWithCorrectOrigin(expectedError,
                                                         ErrorHandling::Origin::SERVER_COMMUNICATOR_OPEN_CONNECTION);

    auto numberOfErrorToReceive = 1;
    ErrorSinkMock sink(numberOfErrorToReceive);
    ErrorProcessingScheduler scheduler(&sink);
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&throwingMockStrategy, &handlingCallBacks);

    serverCommunicator.linkConsumer(&scheduler);

    serverCommunicator.openConnection(SERVER_ADDRESS);
    sink.waitConsumptionToBeReached();

    scheduler.terminateAndJoin();

    ASSERT_TRUE(Assert::errorHasBeenThrown(&sink, expectedError));
}

TEST_F(ServerCommunicatorTest,
       given_aStrategyThrowingErrorOnCloseConnectionCall_when_errorIsCaught_then_producesTheErrorWithCorrectOrigin) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwErrorWhenCloseConnectionIsCalled();
    Error expectedError = throwingMockStrategy.expectedErrorWhenCloseConnectionIsCalled();
    expectedError = formatStrategyErrorWithCorrectOrigin(expectedError,
                                                         ErrorHandling::Origin::SERVER_COMMUNICATOR_CLOSE_CONNECTION);
    auto numberOfErrorToReceive = 1;
    ErrorSinkMock sink(numberOfErrorToReceive);
    ErrorProcessingScheduler scheduler(&sink);
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&throwingMockStrategy, &handlingCallBacks);

    serverCommunicator.linkConsumer(&scheduler);

    serverCommunicator.terminateAndJoin();
    sink.waitConsumptionToBeReached();

    scheduler.terminateAndJoin();

    ASSERT_TRUE(Assert::errorHasBeenThrown(&sink, expectedError));
}

TEST_F(ServerCommunicatorTest,
       given_aStrategyThrowingErrorOnSendMessagesCall_when_errorIsCaught_then_producesTheErrorWithCorrectOrigin) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwErrorWhenSendMessageIsCalled();
    Error expectedError = throwingMockStrategy.expectedErrorWhenSendMessageIsCalled();
    expectedError = formatStrategyErrorWithCorrectOrigin(expectedError,
                                                         ErrorHandling::Origin::SERVER_COMMUNICATOR_SEND_MESSAGE);
    auto numberOfErrorToReceive = 1;
    ErrorSinkMock sink(numberOfErrorToReceive);
    ErrorProcessingScheduler scheduler(&sink);
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&throwingMockStrategy, &handlingCallBacks );

    serverCommunicator.linkConsumer(&scheduler);

    auto message = DataTestUtil::createRandomSimpleMessageWithEmptyTimestamps();
    serverCommunicator.consume(std::move(message));
    sink.waitConsumptionToBeReached();

    scheduler.terminateAndJoin();

    ASSERT_TRUE(Assert::errorHasBeenThrown(&sink, expectedError));
}

TEST_F(ServerCommunicatorTest,
       given_aStrategyThrowingErrorOnSendRawDataCyclesCall_when_errorIsCaught_then_producesTheErrorWithCorrectOrigin) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwErrorWhenSendRawDataIsCalled();
    Error expectedError = throwingMockStrategy.expectedErrorWhenSendRawDataIsCalled();
    expectedError = formatStrategyErrorWithCorrectOrigin(expectedError,
                                                         ErrorHandling::Origin::SERVER_COMMUNICATOR_SEND_RAWDATA);
    auto numberOfErrorToReceive = 1;
    ErrorSinkMock sink(numberOfErrorToReceive);
    ErrorProcessingScheduler scheduler(&sink);
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&throwingMockStrategy, &handlingCallBacks );

    serverCommunicator.linkConsumer(&scheduler);

    auto rawData = DataTestUtil::createRandomSimpleRawData();
    serverCommunicator.consume(std::move(rawData));
    sink.waitConsumptionToBeReached();

    scheduler.terminateAndJoin();

    ASSERT_TRUE(Assert::errorHasBeenThrown(&sink, expectedError));
}

TEST_F(ServerCommunicatorTest,
       given_aStrategyThrowingErrorOnSendResponseCall_when_errorIsCaught_then_producesTheErrorWithCorrectOrigin) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwErrorWhenSendErrorMessageResponseIsCalled();
    Error expectedError = throwingMockStrategy.expectedErrorWhenSendResponseIsCalled();
    expectedError = formatStrategyErrorWithCorrectOrigin(expectedError,
                                                         ErrorHandling::Origin::SERVER_COMMUNICATOR_SEND_RESPONSE);
    auto numberOfErrorToReceive = 1;
    ErrorSinkMock sink(numberOfErrorToReceive);
    ErrorProcessingScheduler scheduler(&sink);
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&throwingMockStrategy, &handlingCallBacks );

    serverCommunicator.linkConsumer(&scheduler);

    auto response = givenAnErrorMessageResponse();
    serverCommunicator.sendResponse(std::move(response));
    sink.waitConsumptionToBeReached();

    scheduler.terminateAndJoin();

    ASSERT_TRUE(Assert::errorHasBeenThrown(&sink, expectedError));
}

TEST_F(ServerCommunicatorTest, given__when_isServerConnected_then_returnFalse) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks );

    auto serverConnected = serverCommunicator.isServerConnected();

    ASSERT_FALSE(serverConnected);
}

TEST_F(ServerCommunicatorTest,
       given_aConnectedServer_when_isServerConnected_then_returnTrueAfterStrategyHasBeenCalled) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks );

    serverCommunicator.openConnection(SERVER_ADDRESS);

    bool strategyHasBeenCalled = false;
    do {
        strategyHasBeenCalled = mockStrategy.hasOpenConnectionBeenCalled();
    }
    while (!strategyHasBeenCalled);

    auto serverConnected = serverCommunicator.isServerConnected();
    serverCommunicator.terminateAndJoin();

    ASSERT_TRUE(serverConnected);
}

TEST_F(ServerCommunicatorTest, given_anOpennedConnection_when_terminateAndJoin_then_serverIsNoLonguerConnected) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks );

    serverCommunicator.openConnection(SERVER_ADDRESS);
    serverCommunicator.terminateAndJoin();

    auto serverConnected = serverCommunicator.isServerConnected();

    ASSERT_FALSE(serverConnected);
}


TEST_F(ServerCommunicatorTest,
       given_aThrowingStrategyOnConnection_when_isServerConnected_then_returnFalseAfterStrategyHasBeenCalled) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenOpenConnectionIsCalled();

    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&throwingMockStrategy, &handlingCallBacks );
    serverCommunicator.openConnection(SERVER_ADDRESS);
    throwingMockStrategy.waitUntilOpenConnectionCallIsMadeAfterErrorIsThrown();

    auto serverConnected = serverCommunicator.isServerConnected();
    serverCommunicator.terminateAndJoin();

    ASSERT_FALSE(serverConnected);
}

TEST_F(ServerCommunicatorTest,
       given_aServerStrategyThatReturnFalseOnHasReceivedGetAllParameterNamesRequest_when_openConnection_then_sendsTheRequestHandleOneGetAllParameterNamesRequest) {
    auto expectedNumberOfRequest = 0u;

    MockRequestHandler mockRequestHandler;
    mockRequestHandler.setExpectedNumberOfGetAllParameterNamesRequest(expectedNumberOfRequest);
    RequestResponseServerCommunicationStrategy requestResponseStrategy;
    requestResponseStrategy.onHasReceivedGetAllParameterNamesRequestReturn(false);
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    bindHandlerGetAllParameterNamesTo(&mockRequestHandler),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&requestResponseStrategy, &handlingCallBacks );

    serverCommunicator.openConnection(SERVER_ADDRESS);
    sleepForTenthOfASecond();

    auto hasReceivedExpectedNumberOfCall = mockRequestHandler.hasReceivedExpectedNumberOfGetAllParameterNamesRequest();

    serverCommunicator.terminateAndJoin();
    ASSERT_TRUE(hasReceivedExpectedNumberOfCall);
}

TEST_F(ServerCommunicatorTest,
       given_aServerStrategyThatReturnTrueOnHasReceivedGetAllParameterNamesRequest_when_openConnection_then_sendsTheRequestHandleOneGetAllParameterNamesRequest) {
    auto expectedNumberOfRequest = 1u;

    MockRequestHandler mockRequestHandler;
    mockRequestHandler.setExpectedNumberOfGetAllParameterNamesRequest(expectedNumberOfRequest);
    RequestResponseServerCommunicationStrategy requestResponseStrategy;
    requestResponseStrategy.onHasReceivedGetAllParameterNamesRequestReturn(true);
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    bindHandlerGetAllParameterNamesTo(&mockRequestHandler),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&requestResponseStrategy, &handlingCallBacks );

    serverCommunicator.openConnection(SERVER_ADDRESS);
    mockRequestHandler.waitForExpectedNumberOfGetAllParameterNamesRequest();

    auto hasReceivedExpectedNumberOfCall = mockRequestHandler.hasReceivedExpectedNumberOfGetAllParameterNamesRequest();

    serverCommunicator.terminateAndJoin();
    ASSERT_TRUE(hasReceivedExpectedNumberOfCall);
}

TEST_F(ServerCommunicatorTest,
       given_aConnectedServer_when_openConnection_then_callsFetchGetParameterValueContentsInStrategy) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks );

    serverCommunicator.openConnection(SERVER_ADDRESS);

    mockStrategy.waitUntilFetchGetParameterValueContentsIsCalled();

    serverCommunicator.terminateAndJoin();

    auto fetchGetParameterValueContentsBeenCalled = mockStrategy.hasFetchGetParameterValueContentsBeenCalled();
    ASSERT_TRUE(fetchGetParameterValueContentsBeenCalled);
}

TEST_F(ServerCommunicatorTest,
       given_aStrategyThrowingErrorOnFetchGetParameterValueContentsCall_when_errorIsCaught_then_producesTheErrorWithCorrectOrigin) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwErrorWhenFetchGetParameterValueContentsIsCalled();
    Error expectedError = throwingMockStrategy.expectedErrorWhenFetchGetParameterValueContentsIsCalled();
    expectedError = formatStrategyErrorWithCorrectOrigin(expectedError,
                                                         ErrorHandling::Origin::SERVER_COMMUNICATOR_FETCH_GET_PARAMETER_VALUE);
    auto numberOfErrorToReceive = 1;
    ErrorSinkMock sink(numberOfErrorToReceive);
    ErrorProcessingScheduler scheduler(&sink);
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&throwingMockStrategy, &handlingCallBacks );

    serverCommunicator.linkConsumer(&scheduler);

    serverCommunicator.openConnection(SERVER_ADDRESS);
    sink.waitConsumptionToBeReached();

    serverCommunicator.terminateAndJoin();
    scheduler.terminateAndJoin();

    ASSERT_TRUE(Assert::errorHasBeenThrown(&sink, expectedError));
}

TEST_F(ServerCommunicatorTest,
       given_aServerStrategyThatReturnsOneValidGetParameterValueContent_when_openConnection_then_sendsTheRequestHandleOneGetParameterValidRequest) {
    auto expectedNumberOfRequest = 1u;

    MockRequestHandler mockRequestHandler;
    mockRequestHandler.setExpectedNumberOfGetParameterValueRequest(expectedNumberOfRequest);
    RequestResponseServerCommunicationStrategy requestResponseStrategy;
    requestResponseStrategy.increaseNumberOfUniqueValidGetParameterValueContentsToReturnBy(expectedNumberOfRequest);
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    bindHandlerGetParameterValueTo(&mockRequestHandler),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&requestResponseStrategy, &handlingCallBacks );

    assertRequestHandlerReceivesExpectedNumberOfValidRequestsWithStrategy(&mockRequestHandler,
                                                                          &serverCommunicator);
    serverCommunicator.terminateAndJoin();
}

TEST_F(ServerCommunicatorTest,
       given_aServerStrategyThatReturnsANumberOfUniqueGetParameterValueContent_when_openConnection_then_sendsTheRequestHandleAllRequest) {
    auto expectedNumberOfRequest = 3u; // Warning: do not expect a bigger *unique* number than the number of available parameter names

    MockRequestHandler mockRequestHandler;
    mockRequestHandler.setExpectedNumberOfGetParameterValueRequest(expectedNumberOfRequest);
    RequestResponseServerCommunicationStrategy requestResponseStrategy;
    requestResponseStrategy.increaseNumberOfUniqueValidGetParameterValueContentsToReturnBy(expectedNumberOfRequest);

    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    bindHandlerGetParameterValueTo(&mockRequestHandler),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&requestResponseStrategy, &handlingCallBacks );

    assertRequestHandlerReceivesExpectedNumberOfValidRequestsWithStrategy(&mockRequestHandler,
                                                                          &serverCommunicator);
    serverCommunicator.terminateAndJoin();
}

TEST_F(ServerCommunicatorTest,
       given_aServerStrategyThatReturnsANumberOfDuplicateGetParameterValueContent_when_openConnection_then_sendsTheRequestHandleAllRequest) {
    auto numberOfUniqueRequest = 2u;
    auto numberOfDuplicateRequest = 3u;

    auto expectedNumberOfRequest = numberOfUniqueRequest + numberOfDuplicateRequest;

    MockRequestHandler mockRequestHandler;
    mockRequestHandler.setExpectedNumberOfGetParameterValueRequest(expectedNumberOfRequest);
    RequestResponseServerCommunicationStrategy requestResponseStrategy;
    requestResponseStrategy.increaseNumberOfUniqueValidGetParameterValueContentsToReturnBy(numberOfUniqueRequest);
    requestResponseStrategy.increaseNumberOfDuplicateValidGetParameterValueContentsToReturnBy(numberOfDuplicateRequest);

    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    bindHandlerGetParameterValueTo(&mockRequestHandler),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&requestResponseStrategy, &handlingCallBacks );

    assertRequestHandlerReceivesExpectedNumberOfValidRequestsWithStrategy(&mockRequestHandler,
                                                                          &serverCommunicator);
    serverCommunicator.terminateAndJoin();

    auto sentRequestContent = requestResponseStrategy.getReturnedGetParameterValueRequest();
    auto receivedRequests = mockRequestHandler.getReceivedGetParameterValueRequests();
    auto requestCounter = 0u;
    while (requestCounter < expectedNumberOfRequest) {
        auto expectedRequestContent = sentRequestContent.at(requestCounter);
        auto expectedRequest = Assemble::ServerRequestAssembler::getParameterValueRequest(expectedRequestContent);
        auto receivedRequest = receivedRequests.front();
        receivedRequests.pop_front();
        requestCounter++;
        ASSERT_EQ(expectedRequest, receivedRequest);
    }
}

TEST_F(ServerCommunicatorTest,
       given_anAllParameterMetadataResponse_when_sendResponse_then_asksTheStrategyToSendTheUnsignedIntegerParameterResponse) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks );
    auto response = Given::anAllParameterMetadataResponse<ParameterListForThisTestCase>();
    auto responseCopy = decltype(Given::anAllParameterMetadataResponse<ParameterListForThisTestCase>())(response);

    serverCommunicator.sendResponse(std::move(response));

    auto strategyHasBeenCalled = mockStrategy.hasSendResponseAllParameterMetadataBeenCalledWith(responseCopy);
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest,
       given_anUnsignedIntegerParameterValueResponse_when_sendResponse_then_asksTheStrategyToSendTheUnsignedIntegerParameterResponse) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks );
    auto response = Given::anUnsignedIntegerParameterValueResponse(true);
    auto responseCopy = decltype(Given::anUnsignedIntegerParameterValueResponse(true))(response);

    serverCommunicator.sendResponse(std::move(response));

    auto strategyHasBeenCalled = mockStrategy.hasSendUnsignedParameterResponseBeenCalledWith(responseCopy);
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest,
       given_aSignedIntegerParameterValueResponse_when_sendResponse_then_asksTheStrategyToSendTheSignedIntegerParameterResponse) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks );
    auto response = Given::aSignedIntegerParameterValueResponse(true);
    auto responseCopy = decltype(Given::aSignedIntegerParameterValueResponse(true))(response);

    serverCommunicator.sendResponse(std::move(response));

    auto strategyHasBeenCalled = mockStrategy.hasSendSignedParameterResponseBeenCalledWith(responseCopy);
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest,
       given_aRealNumberParameterValueResponse_when_sendResponse_then_asksTheStrategyToSendTheRealNumberParameterResponse) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks );
    auto response = Given::aRealNumberParameterValueResponse(true);
    auto responseCopy = decltype(Given::aRealNumberParameterValueResponse(true))(response);

    serverCommunicator.sendResponse(std::move(response));

    auto strategyHasBeenCalled = mockStrategy.hasSendRealNumberParameterResponseBeenCalledWith(responseCopy);
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest,
       given_aBooleanParameterValueResponse_when_sendResponse_then_asksTheStrategyToSendTheBooleanParameterResponse) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks );
    auto response = Given::aBooleanParameterValueResponse(true);
    auto responseCopy = decltype(Given::aBooleanParameterValueResponse(true))(response);

    serverCommunicator.sendResponse(std::move(response));

    auto strategyHasBeenCalled = mockStrategy.hasSendBooleanParameterResponseBeenCalledWith(responseCopy);
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest,
       given_aParameterErrorResponse_when_sendResponse_then_asksTheStrategyToSendTheParameterErrorResponse) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks );
    auto response = givenAParameterErrorResponse();
    auto responseCopy = decltype(givenAParameterErrorResponse())(response);

    serverCommunicator.sendResponse(std::move(response));

    auto strategyHasBeenCalled = mockStrategy.hasSendParameterErrorResponseBeenCalledWith(responseCopy);
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest,
       given_anSuccessMessageResponse_when_sendResponse_then_asksTheStrategyToSendTheSuccessMessageResponse) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks );
    auto response = givenASuccessMessageResponse();
    auto responseCopy = decltype(givenASuccessMessageResponse())(response);

    serverCommunicator.sendResponse(std::move(response));

    auto strategyHasBeenCalled = mockStrategy.hasSendSuccessMessageResponseBeenCalledWith(responseCopy);
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest,
       given_anErrorMessageResponse_when_sendResponse_then_asksTheStrategyToSendTheErrorMessageResponse) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&mockStrategy, &handlingCallBacks );
    auto response = givenAnErrorMessageResponse();
    auto responseCopy = decltype(givenAnErrorMessageResponse())(response);

    serverCommunicator.sendResponse(std::move(response));

    auto strategyHasBeenCalled = mockStrategy.hasSendErrorMessageResponseBeenCalledWith(responseCopy);
    ASSERT_TRUE(strategyHasBeenCalled);
}


// TODO : write test : when sendResponse(std::move(response)) -> calls strategy with output of `auto ServerResponse.formatForCommunicationOut const -> std::tuple<RequestResult, Request>`


TEST_F(ServerCommunicatorTest,
       given_aServerStrategyThatReturnFalseOnHasReceivedCalibrationRequest_when_openConnection_then_sendsTheRequestHandleOneCalibrationRequest) {
    auto expectedNumberOfRequest = 0u;

    MockRequestHandler mockRequestHandler;
    mockRequestHandler.setExpectedNumberOfCalibrationRequest(expectedNumberOfRequest);
    RequestResponseServerCommunicationStrategy requestResponseStrategy;
    requestResponseStrategy.onHasReceivedCalibrationRequestReturn(false);
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    bindHandlerCalibrationTo(&mockRequestHandler),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&requestResponseStrategy, &handlingCallBacks );

    serverCommunicator.openConnection(SERVER_ADDRESS);
    sleepForTenthOfASecond();

    auto hasReceivedExpectedNumberOfCall = mockRequestHandler.hasReceivedExpectedNumberOfCalibrationRequest();

    serverCommunicator.terminateAndJoin();
    ASSERT_TRUE(hasReceivedExpectedNumberOfCall);
}

TEST_F(ServerCommunicatorTest,
       given_aServerStrategyThatReturnTrueOnHasReceivedCalibrationRequest_when_openConnection_then_sendsTheRequestHandleOneCalibrationRequest) {
    auto expectedNumberOfRequest = 1u;

    MockRequestHandler mockRequestHandler;
    mockRequestHandler.setExpectedNumberOfCalibrationRequest(expectedNumberOfRequest);
    RequestResponseServerCommunicationStrategy requestResponseStrategy;
    requestResponseStrategy.onHasReceivedCalibrationRequestReturn(true);
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    bindHandlerCalibrationTo(&mockRequestHandler),
                    ignoreHandleClearCalibration()

            );
    ServerCommunicator serverCommunicator(&requestResponseStrategy, &handlingCallBacks );

    serverCommunicator.openConnection(SERVER_ADDRESS);
    mockRequestHandler.waitForExpectedNumberOfCalibrationRequest();

    auto hasReceivedExpectedNumberOfCall = mockRequestHandler.hasReceivedExpectedNumberOfCalibrationRequest();

    serverCommunicator.terminateAndJoin();
    ASSERT_TRUE(hasReceivedExpectedNumberOfCall);
}

TEST_F(ServerCommunicatorTest,
       given_aServerStrategyThatReturnFalseOnHasReceivedClearCalibrationRequest_when_openConnection_then_sendsTheRequestHandleOneClearCalibrationRequest) {
    auto expectedNumberOfRequest = 0u;

    MockRequestHandler mockRequestHandler;
    mockRequestHandler.setExpectedNumberOfClearCalibrationRequest(expectedNumberOfRequest);
    RequestResponseServerCommunicationStrategy requestResponseStrategy;
    requestResponseStrategy.onHasReceivedClearCalibrationRequestReturn(false);
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    bindHandlerClearCalibrationTo(&mockRequestHandler)

            );
    ServerCommunicator serverCommunicator(&requestResponseStrategy, &handlingCallBacks );

    serverCommunicator.openConnection(SERVER_ADDRESS);
    sleepForTenthOfASecond();

    auto hasReceivedExpectedNumberOfCall = mockRequestHandler.hasReceivedExpectedNumberOfClearCalibrationRequest();

    serverCommunicator.terminateAndJoin();
    ASSERT_TRUE(hasReceivedExpectedNumberOfCall);
}

TEST_F(ServerCommunicatorTest,
       given_aServerStrategyThatReturnTrueOnHasReceivedClearCalibrationRequest_when_openConnection_then_sendsTheRequestHandleOneClearCalibrationRequest) {
    auto expectedNumberOfRequest = 1u;

    MockRequestHandler mockRequestHandler;
    mockRequestHandler.setExpectedNumberOfClearCalibrationRequest(expectedNumberOfRequest);
    RequestResponseServerCommunicationStrategy requestResponseStrategy;
    requestResponseStrategy.onHasReceivedClearCalibrationRequestReturn(true);
    ServerRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    ignoreHandleGetAllParameterNames(),
                    ignoreHandleGetParameterValue(),
                    ignoreHandleCalibration(),
                    bindHandlerClearCalibrationTo(&mockRequestHandler)

            );
    ServerCommunicator serverCommunicator(&requestResponseStrategy, &handlingCallBacks );

    serverCommunicator.openConnection(SERVER_ADDRESS);
    mockRequestHandler.waitForExpectedNumberOfClearCalibrationRequest();

    auto hasReceivedExpectedNumberOfCall = mockRequestHandler.hasReceivedExpectedNumberOfClearCalibrationRequest();

    serverCommunicator.terminateAndJoin();
    ASSERT_TRUE(hasReceivedExpectedNumberOfCall);
}

#endif //SENSORGATEWAY_SERVERCOMMUNICATORTEST_CPP

