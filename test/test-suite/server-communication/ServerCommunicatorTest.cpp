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
#include <list>

#include "sensor-gateway/server-communication/ServerCommunicator.hpp"
#include "test/utilities/data-model/DataModelFixture.h"
#include "test/utilities/mock/ArbitraryDataSinkMock.hpp"
#include "test/utilities/mock/CatchingRequestHandlerMock.hpp"
#include "test/utilities/mock/CatchingServerCommunicationStrategyMock.hpp"
#include "test/utilities/mock/ErrorThrowingServerCommunicationStrategyMock.hpp"
#include "test/utilities/mock/RequestResponseSpecializedServerCommunicationStrategyMock.hpp"
#include "test/utilities/assertion/TimeAssertion.hpp"

using TestFunctions::DataTestUtil;

class ServerCommunicatorTest : public ::testing::Test {

public:

    using Error = ErrorHandling::SensorAccessLinkError;
    using ErrorSinkMock = Mock::ArbitraryDataSinkMock<Error>;
    using ErrorProcessingScheduler = DataFlow::DataProcessingScheduler<Error, ErrorSinkMock, 1>;

    using DataStructures = Sensor::Test::Simple::Structures;
    using ParameterListForThisTestCase = DataStructures::Parameters;
    using MockServerCommunicatorStrategy = Mock::CatchingServerCommunicationStrategyMock<DataStructures>;
    using ThrowingServerCommunicationStrategy = Mock::ErrorThrowingServerCommunicationStrategyMock<DataStructures>;
    using RequestResponseServerCommunicationStrategy = Mock::RequestResponseSpecializedServerCommunicationStrategyMock<DataStructures>;

protected:

    using ServerCommunicator = SensorAccessLinkElement::ServerCommunicator<DataStructures>;
    using MockRequestHandler = Mock::CatchingRequestHandlerMock<DataStructures, DataStructures>;
    using Message = Sensor::Test::Simple::Structures::Message;
    using RawData = Sensor::Test::Simple::Structures::RawData;

    std::string const SERVER_ADDRESS = "I like trains";
    MockRequestHandler defaultRequestHandlerMock;

    using GetParameterValueRequest = ServerCommunication::RequestTypes::GetParameterValue;
    using HandleGetParameterValueRequest = std::function<void(GetParameterValueRequest&&)>;

    ServerCommunicatorTest() = default;

    virtual ~ServerCommunicatorTest() = default;

    void assertRequestHandlerReceivesExpectedNumberOfValidRequestsWithStrategy(MockRequestHandler* mockRequestHandler,
                                                                               ServerCommunicator* serverCommunicator) {

        serverCommunicator->openConnection(SERVER_ADDRESS);
        mockRequestHandler->waitForExpectedNumberOfGetParameterValueRequest();
        serverCommunicator->terminateAndJoin();

        auto hasReceivedExpectedNumberOfCall = mockRequestHandler->hasReceivedExpectedNumberOfGetParameterValueRequest();

        ASSERT_TRUE(hasReceivedExpectedNumberOfCall);
    }

    auto bindHandlerGetParameterValueTo(MockRequestHandler* handlerMock) -> HandleGetParameterValueRequest {
        return std::bind(&MockRequestHandler::handleGetParameterValueRequest,
                         handlerMock, std::placeholders::_1);
    }

    auto ignoreHandleGetParameterValue() -> HandleGetParameterValueRequest {
        return bindHandlerGetParameterValueTo(&defaultRequestHandlerMock);
    }

    auto recreateGetParameterValueRequestFromName(
            std::string const& content) const -> std::tuple<GetParameterValueRequest, bool, bool> const {
        GetParameterValueRequest getParameterRequest = Assemble::ServerRequest::getParameterValueRequest(content);
        bool willCreateError = false;
        try {
            Assemble::ServerRequest::ensureParameterIsAvailable<ParameterListForThisTestCase>(content);
        } catch (ErrorHandling::SensorAccessLinkError& e) {
            willCreateError = true;
        }
        bool hasNonDefaultContent = getParameterRequest != GetParameterValueRequest::returnDefaultData();
        return std::make_tuple(getParameterRequest, hasNonDefaultContent, willCreateError);
    };

    Error formatStrategyErrorWithCorrectOrigin(Error strategyIssuedError, std::string origin) const noexcept {
        Error formattedError(origin + ErrorHandling::Message::SEPARATOR + strategyIssuedError.getOrigin(),
                             strategyIssuedError.getCategory(),
                             strategyIssuedError.getSeverity(),
                             strategyIssuedError.getErrorCode(),
                             strategyIssuedError.getMessage());
        return formattedError;
    }

    ::testing::AssertionResult expectedErrorHasBeenThrown(ErrorSinkMock* sink, Error expectedError) const noexcept {

        auto producedErrors = sink->getConsumedData();
        bool expectedErrorPublished = true;
        auto lastParsedError = ErrorHandling::SensorAccessLinkError::returnDefaultData();
        for (auto t = 0; t < producedErrors.size() && expectedErrorPublished; ++t) {
            lastParsedError = producedErrors.front();
            expectedErrorPublished = lastParsedError == expectedError;
            producedErrors.pop_front();
        }
        if (expectedErrorPublished) {
            return ::testing::AssertionSuccess();
        } else {
            return ::testing::AssertionFailure()
                    << "\nExpected : \n"
                    << expectedError.fetchDetailedMessage()
                    << "\nGot : \n"
                    << lastParsedError.fetchDetailedMessage()
                    << "\n";
        }
    }

};

TEST_F(ServerCommunicatorTest, given__when_connect_then_callsOpenConnectionInStrategy) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy,
                                          ignoreHandleGetParameterValue());

    serverCommunicator.openConnection(SERVER_ADDRESS);
    auto strategyHasBeenCalled = mockStrategy.hasOpenConnectionBeenCalled();
    serverCommunicator.terminateAndJoin();

    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest, given_aMessageToSend_when_consume_then_callsSendMessageInStrategy) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy,
                                          ignoreHandleGetParameterValue());
    auto message = DataTestUtil::createRandomSimpleMessageWithEmptyTimestamps();

    serverCommunicator.consume(std::move(message));

    auto strategyHasBeenCalled = mockStrategy.hasSendMessageBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest, given_aMessageToSend_when_consume_then_callsSendMessageInStrategyWithTheMessage) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy,
                                          ignoreHandleGetParameterValue());
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
    ServerCommunicator serverCommunicator(&mockStrategy,
                                          ignoreHandleGetParameterValue());
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
    ServerCommunicator serverCommunicator(&mockStrategy,
                                          ignoreHandleGetParameterValue());
    auto rawData = DataTestUtil::createRandomSimpleRawData();

    serverCommunicator.consume(std::move(rawData));

    auto strategyHasBeenCalled = mockStrategy.hasSendRawDataBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest, given_aRawDataCycleToSend_when_consume_then_callsSendRawDataInStrategyWithTheRawData) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy,
                                          ignoreHandleGetParameterValue());
    auto rawData = DataTestUtil::createRandomSimpleRawData();
    auto copy = RawData(rawData);

    serverCommunicator.consume(std::move(rawData));

    auto receivedRawData = mockStrategy.getFirstSentRawData();
    ASSERT_EQ(copy, receivedRawData);
}

TEST_F(ServerCommunicatorTest, given__when_terminateAndJoin_then_callsCloseConnectionInStrategy) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy,
                                          ignoreHandleGetParameterValue());

    serverCommunicator.terminateAndJoin();

    auto strategyHasBeenCalled = mockStrategy.hasCloseConnectionBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToCloseThrownOnOpenConnectionCall_when_isCaught_then_callsCloseConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwCloseConnectionRequiredErrorWhenOpenConnectionIsCalled();

    ServerCommunicator serverCommunicator(&throwingMockStrategy,
                                          ignoreHandleGetParameterValue());
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

    ServerCommunicator serverCommunicator(&throwingMockStrategy,
                                          ignoreHandleGetParameterValue());
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

    ServerCommunicator serverCommunicator(&throwingMockStrategy,
                                          ignoreHandleGetParameterValue());
    serverCommunicator.terminateAndJoin();
    throwingMockStrategy.waitUntilCloseConnectionCallIsMadeAfterErrorIsThrown();

    auto closeConnectionCalled = throwingMockStrategy.hasCloseConnectionBeenCalledAfterThrowingFunction();

    ASSERT_TRUE(closeConnectionCalled);
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToOpenThrownOnCloseConnectionCall_when_isCaught_then_callsOpenConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenCloseConnectionIsCalled();

    ServerCommunicator serverCommunicator(&throwingMockStrategy,
                                          ignoreHandleGetParameterValue());
    serverCommunicator.terminateAndJoin();
    throwingMockStrategy.waitUntilOpenConnectionCallIsMadeAfterErrorIsThrown();

    auto openConnectionCalled = throwingMockStrategy.hasOpenConnectionBeenCalledAfterThrowingFunction();

    ASSERT_TRUE(openConnectionCalled);
}

TEST_F(ServerCommunicatorTest,
       given_anErrorThatRequiresToCloseThrownOnSendMessageCall_when_isCaught_then_callsCloseConnectionInStrategy) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwCloseConnectionRequiredErrorWhenSendMessageIsCalled();

    ServerCommunicator serverCommunicator(&throwingMockStrategy,
                                          ignoreHandleGetParameterValue());
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

    ServerCommunicator serverCommunicator(&throwingMockStrategy,
                                          ignoreHandleGetParameterValue());
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

    ServerCommunicator serverCommunicator(&throwingMockStrategy,
                                          ignoreHandleGetParameterValue());
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

    ServerCommunicator serverCommunicator(&throwingMockStrategy,
                                          ignoreHandleGetParameterValue());
    auto rawData = DataTestUtil::createRandomSimpleRawData();
    serverCommunicator.consume(std::move(rawData));
    throwingMockStrategy.waitUntilOpenConnectionCallIsMadeAfterErrorIsThrown();

    auto openConnectionCalled = throwingMockStrategy.hasOpenConnectionBeenCalledAfterThrowingFunction();

    ASSERT_TRUE(openConnectionCalled);
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
    ServerCommunicator serverCommunicator(&throwingMockStrategy,
                                          ignoreHandleGetParameterValue());

    serverCommunicator.linkConsumer(&scheduler);

    serverCommunicator.openConnection(SERVER_ADDRESS);
    sink.waitConsumptionToBeReached();

    scheduler.terminateAndJoin();

    ASSERT_TRUE(expectedErrorHasBeenThrown(&sink, expectedError));
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
    ServerCommunicator serverCommunicator(&throwingMockStrategy,
                                          ignoreHandleGetParameterValue());

    serverCommunicator.linkConsumer(&scheduler);

    serverCommunicator.terminateAndJoin();
    sink.waitConsumptionToBeReached();

    scheduler.terminateAndJoin();

    ASSERT_TRUE(expectedErrorHasBeenThrown(&sink, expectedError));
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
    ServerCommunicator serverCommunicator(&throwingMockStrategy,
                                          ignoreHandleGetParameterValue());

    serverCommunicator.linkConsumer(&scheduler);

    auto message = DataTestUtil::createRandomSimpleMessageWithEmptyTimestamps();
    serverCommunicator.consume(std::move(message));
    sink.waitConsumptionToBeReached();

    scheduler.terminateAndJoin();

    ASSERT_TRUE(expectedErrorHasBeenThrown(&sink, expectedError));
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
    ServerCommunicator serverCommunicator(&throwingMockStrategy,
                                          ignoreHandleGetParameterValue());

    serverCommunicator.linkConsumer(&scheduler);

    auto rawData = DataTestUtil::createRandomSimpleRawData();
    serverCommunicator.consume(std::move(rawData));
    sink.waitConsumptionToBeReached();

    scheduler.terminateAndJoin();

    ASSERT_TRUE(expectedErrorHasBeenThrown(&sink, expectedError));
}

TEST_F(ServerCommunicatorTest, given__when_isServerConnected_then_returnFalse) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy,
                                          ignoreHandleGetParameterValue());

    auto serverConnected = serverCommunicator.isServerConnected();

    ASSERT_FALSE(serverConnected);
}

TEST_F(ServerCommunicatorTest,
       given_aConnectedServer_when_isServerConnected_then_returnTrueAfterStrategyHasBeenCalled) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy,
                                          ignoreHandleGetParameterValue());

    serverCommunicator.openConnection(SERVER_ADDRESS);

    bool strategyHasBeenCalled = false;
    do {
        strategyHasBeenCalled = mockStrategy.hasOpenConnectionBeenCalled();
    } while (!strategyHasBeenCalled);

    auto serverConnected = serverCommunicator.isServerConnected();
    serverCommunicator.terminateAndJoin();

    ASSERT_TRUE(serverConnected);
}

TEST_F(ServerCommunicatorTest, given_anOpennedConnection_when_terminateAndJoin_then_serverIsNoLonguerConnected) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy,
                                          ignoreHandleGetParameterValue());

    serverCommunicator.openConnection(SERVER_ADDRESS);
    serverCommunicator.terminateAndJoin();

    auto serverConnected = serverCommunicator.isServerConnected();

    ASSERT_FALSE(serverConnected);
}


TEST_F(ServerCommunicatorTest,
       given_aThrowingStrategyOnConnection_when_isServerConnected_then_returnFalseAfterStrategyHasBeenCalled) {
    ThrowingServerCommunicationStrategy throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenOpenConnectionIsCalled();

    ServerCommunicator serverCommunicator(&throwingMockStrategy,
                                          ignoreHandleGetParameterValue());
    serverCommunicator.openConnection(SERVER_ADDRESS);
    throwingMockStrategy.waitUntilOpenConnectionCallIsMadeAfterErrorIsThrown();

    auto serverConnected = serverCommunicator.isServerConnected();
    serverCommunicator.terminateAndJoin();

    ASSERT_FALSE(serverConnected);
}

TEST_F(ServerCommunicatorTest,
       given_aConnectedServer_when_openConnection_then_callsFetchGetParameterValueContentsInStrategy) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy,
                                          ignoreHandleGetParameterValue());

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
    ServerCommunicator serverCommunicator(&throwingMockStrategy,
                                          ignoreHandleGetParameterValue());

    serverCommunicator.linkConsumer(&scheduler);

    serverCommunicator.openConnection(SERVER_ADDRESS);
    sink.waitConsumptionToBeReached();

    serverCommunicator.terminateAndJoin();
    scheduler.terminateAndJoin();

    ASSERT_TRUE(expectedErrorHasBeenThrown(&sink, expectedError));
}

TEST_F(ServerCommunicatorTest,
       given_aServerStrategyThatReturnsOneValidGetParameterValueContent_when_openConnection_then_sendsTheRequestHandleOneGetParameterValidRequest) {
    auto expectedNumberOfRequest = 1u;

    MockRequestHandler mockRequestHandler;
    mockRequestHandler.setExpectedNumberOfGetParameterValueRequest(expectedNumberOfRequest);
    RequestResponseServerCommunicationStrategy requestResponseStrategy;
    requestResponseStrategy.setNumberOfUniqueValidGetParameterValueContentsToReturn(expectedNumberOfRequest);
    ServerCommunicator serverCommunicator(&requestResponseStrategy,
                                          bindHandlerGetParameterValueTo(&mockRequestHandler));

    assertRequestHandlerReceivesExpectedNumberOfValidRequestsWithStrategy(&mockRequestHandler,
                                                                          &serverCommunicator);
    serverCommunicator.terminateAndJoin();
}

TEST_F(ServerCommunicatorTest,
       given_aServerStrategyThatReturnsANumberOfUniqueValidGetParameterValueContent_when_openConnection_then_sendsTheRequestHandleAllRequest) {
    auto expectedNumberOfRequest = 3u; // Warning: do not expect a bigger *unique* number than the number of available parameter names

    MockRequestHandler mockRequestHandler;
    mockRequestHandler.setExpectedNumberOfGetParameterValueRequest(expectedNumberOfRequest);
    RequestResponseServerCommunicationStrategy requestResponseStrategy;
    requestResponseStrategy.setNumberOfUniqueValidGetParameterValueContentsToReturn(expectedNumberOfRequest);

    ServerCommunicator serverCommunicator(&requestResponseStrategy,
                                          bindHandlerGetParameterValueTo(&mockRequestHandler));

    assertRequestHandlerReceivesExpectedNumberOfValidRequestsWithStrategy(&mockRequestHandler,
                                                                          &serverCommunicator);
    serverCommunicator.terminateAndJoin();
}

//TEST_F(ServerCommunicatorTest,
//       given_aServerStrategyThatReturnsANumberOfUniqueValidGetParameterValueContent_when_openConnection_then_sendsTheRequestHandleAllRequest) {
//    auto expectedNumberOfRequest = 3u; // Warning: do not expect a bigger *unique* number than the number of available parameter names
//    RequestResponseServerCommunicationStrategy requestResponseStrategy;
//    requestResponseStrategy.setNumberOfUniqueValidGetParameterValueContentsToReturn(expectedNumberOfRequest);
//    MockRequestHandler mockRequestHandler;
//    mockRequestHandler.setExpectedNumberOfGetParameterValueRequest(expectedNumberOfRequest);
//
//    ServerCommunicator serverCommunicator(&requestResponseStrategy,
//                                          bindHandlerGetParameterValueTo(&mockRequestHandler));
//
//    serverCommunicator.openConnection(SERVER_ADDRESS);
//    mockRequestHandler.waitForExpectedNumberOfGetParameterValueRequest();
//    serverCommunicator.terminateAndJoin();
//
//    auto expectedRequestContent = requestResponseStrategy.getReturnedGetParameterValueRequest();
//    std::list<GetParameterValueRequest> getParameterValueRequests;
//    auto requestCount = 0u;
//    bool requestHasNonDefaultContent = false;
//    bool requestWillCreateError = false;
//    GetParameterValueRequest getParameterValueRequest;
//
//    std::tie(getParameterValueRequest, requestHasNonDefaultContent, requestWillCreateError) = recreateGetParameterValueRequestFromName(expectedRequestContent[requestCount++]);
//
//    while (requestHasNonDefaultContent) {
//        getParameterValueRequests.emplace_back(std::move(getParameterValueRequest));
//        std::tie(getParameterValueRequest, requestHasNonDefaultContent, requestWillCreateError) = recreateGetParameterValueRequestFromName(expectedRequestContent[requestCount++]);
//    };
//    auto numberOfRequestSentToHandler = getParameterValueRequests.size();
//
//    ASSERT_TRUE(hasReceivedExpectedNumberOfCall);
//}

#endif //SENSORGATEWAY_SERVERCOMMUNICATORTEST_CPP

