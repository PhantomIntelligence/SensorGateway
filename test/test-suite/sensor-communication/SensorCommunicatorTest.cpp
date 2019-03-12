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

#ifndef SENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP
#define SENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP

#include <gtest/gtest.h>
#include "test/utilities/assertion/ErrorAssertion.hpp"

#include "test/utilities/mock/ArbitraryDataSinkMock.hpp"
#include "test/utilities/mock/ErrorThrowingSensorCommunicationStrategyMock.hpp"
#include "test/utilities/mock/Function.hpp"
#include "test/utilities/mock/LoopBackSensorCommunicationStrategyMock.hpp"

#include "sensor-gateway/sensor-communication/SensorCommunicator.hpp"

using RealisticMessage = Sensor::Test::RealisticImplementation::Structures::Message;
using RealisticRawData = Sensor::Test::RealisticImplementation::Structures::RawData;

using RealisticMessageSinkMock = Mock::ArbitraryDataSinkMock<RealisticMessage>;
using RealisticRawDataSinkMock = Mock::ArbitraryDataSinkMock<RealisticRawData>;

using RealisticMessageList = RealisticMessageSinkMock::DataList;
using RealisticRawDataList = RealisticRawDataSinkMock::DataList;

using RealisticMessageSensorCommunicator = SensorAccessLinkElement::SensorCommunicator<Sensor::Test::RealisticImplementation::Structures>;
using TestFunctions::DataTestUtil;


namespace SensorCommunicatorTestMock {

    class SensorCommunicationStrategy
            : public SensorCommunication::SensorCommunicationStrategy<Sensor::Test::RealisticImplementation::Structures> {

    protected:

        using super = SensorCommunication::SensorCommunicationStrategy<Sensor::Test::RealisticImplementation::Structures>;

        using MockFunctionSendRequest = Mock::Function<StringLiteral<decltype("mock->sendRequest"_ToString)>, Mock::VoidType, Request>;

    public:
        using Request = typename super::Request;

        SensorCommunicationStrategy() :
                openConnectionCalled(false),
                closeConnectionCalled(false),
                fetchMessagesCalled(false),
                fetchRawDataCyclesCalled(false),
                sendCommandCalled(false),
                fetchMessagesInvokedBeforeFetchRawDataCycles(false),
                fetchRawDataCyclesInvokedBeforeSendRequest(false),
                hasToReturnSpecificMessages(false),
                hasToReturnSpecificRawDataCycles(false) {
        }

        ~SensorCommunicationStrategy() noexcept override = default;

        SensorCommunicationStrategy(SensorCommunicationStrategy const& other) = delete;

        SensorCommunicationStrategy(SensorCommunicationStrategy&& other) noexcept = delete;

        SensorCommunicationStrategy& operator=(SensorCommunicationStrategy const& other)& = delete;

        SensorCommunicationStrategy&
        operator=(SensorCommunicationStrategy&& other)& noexcept = delete;

        void returnThisMessageSequenceWhenFetchMessagesIsCalled(RealisticMessageList&& dataToReturn) {
            hasToReturnSpecificMessages = true;
            this->messagesToReturn = std::forward<RealisticMessageList>(dataToReturn);
        }

        void returnThisRawDataCyclesSequenceWhenFetchRawDataCyclesIsCalled(RealisticRawDataList&& dataToReturn) {
            hasToReturnSpecificRawDataCycles = true;
            this->rawDataCyclesToReturn = std::forward<RealisticRawDataList>(dataToReturn);
        }

        super::Messages fetchMessages() override {
            acknowledgeFetchMessagesHasBeenCalled();

            super::Messages messages;
            messages.fill(Message::returnDefaultData());

            if (hasToReturnSpecificMessages) {
                for (auto i = 0u; i < messages.size() && !messagesToReturn.empty(); ++i) {
                    Message rawDataCycle = messagesToReturn.front();
                    messagesToReturn.pop_front();
                    messages[i] = rawDataCycle;
                }
                if (messagesToReturn.empty()) {
                    // We don't have to return any more data
                    hasToReturnSpecificMessages = false;
                }
            }

            return messages;
        }

        super::RawDataCycles fetchRawDataCycles() override {
            acknowledgeFetchRawDataCyclesHasBeenCalled();

            super::RawDataCycles rawDataCycles;
            rawDataCycles.fill(RawData::returnDefaultData());

            if (hasToReturnSpecificRawDataCycles) {
                for (auto i = 0u; i < rawDataCycles.size() && !rawDataCyclesToReturn.empty(); ++i) {
                    RealisticRawData rawDataCycle = rawDataCyclesToReturn.front();
                    rawDataCyclesToReturn.pop_front();
                    rawDataCycles[i] = rawDataCycle;
                }
                if (rawDataCyclesToReturn.empty()) {
                    // We don't have to return any more data
                    hasToReturnSpecificRawDataCycles = false;
                }
            }

            return rawDataCycles;
        }

        void openConnection() override {
            openConnectionCalled.store(true);
        }

        void closeConnection() override {
            closeConnectionCalled.store(true);
        }

        void sendRequest(Request&& request) override {
            mockFunctionSendRequest.invokeVoidReturn(std::forward<Request>(request));
        }

        bool hasOpenConnectionBeenCalled() const {
            return openConnectionCalled.load();
        }

        bool hasCloseConnectionBeenCalled() const {
            return closeConnectionCalled.load();
        }

        bool hasFetchMessagesBeenInvoked() const {
            return fetchMessagesCalled.load();
        }

        bool hasFetchRawDataCyclesBeenInvoked() const {
            return fetchRawDataCyclesCalled.load();
        }

        bool hasSendRequestBeenInvoked() const {
            return mockFunctionSendRequest.hasBeenInvoked();
        }

        bool hasSendRequestBeenInvokedWith(Request const& request) const {
            return mockFunctionSendRequest.hasBeenInvokedWith(request);
        };


        bool hasMessageAndRawDataCallSequenceBeenRespected() const {
            bool sequenceRespected =
                    hasFetchMessagesBeenInvoked() &&
                    hasFetchRawDataCyclesBeenInvoked() &&
                    hasSendRequestBeenInvoked() &&
                    hasFetchMessagesCalledBeforeFetchRawDataCyclesBeenCalled() &&
                    hasFetchRawDataCyclesBeenInvokedBeforeSendRequest();
            return sequenceRespected;
        };

        void waitUntilFetchMessagesInvocation() {
            if (!hasFetchMessagesBeenInvoked()) {
                fetchMessagesCalledAcknowledgement.get_future().wait();
            }
        }

        void waitUntilFetchRawDataCyclesInvocation() {
            if (!hasFetchRawDataCyclesBeenInvoked()) {
                fetchRawDataCyclesCalledAcknowledgement.get_future().wait();
            }
        }

        void waitUntilSendRequestInvocation() {
            mockFunctionSendRequest.waitUntilInvocation();
        }


    protected:
        AtomicFlag openConnectionCalled;
        AtomicFlag closeConnectionCalled;

    private:

        void acknowledgeFetchMessagesHasBeenCalled() {
            LockGuard sequenceGuard(callSequenceMutex);
            LockGuard guard(fetchMessagesAckMutex);
            if (!hasFetchMessagesBeenInvoked()) {
                fetchMessagesCalled.store(true);
                fetchMessagesCalledAcknowledgement.set_value(true);
                if (!hasFetchRawDataCyclesBeenInvoked()) {
                    fetchMessagesInvokedBeforeFetchRawDataCycles.store(true);
                }
            }
        }

        void acknowledgeFetchRawDataCyclesHasBeenCalled() {
            LockGuard sequenceGuard(callSequenceMutex);
            LockGuard guard(fetchRawDataCyclesAckMutex);
            if (!hasFetchRawDataCyclesBeenInvoked()) {
                fetchRawDataCyclesCalled.store(true);
                fetchRawDataCyclesCalledAcknowledgement.set_value(true);
                if (!hasSendRequestBeenInvoked()) {
                    fetchRawDataCyclesInvokedBeforeSendRequest.store(true);
                }
            }
        }

        bool hasFetchMessagesCalledBeforeFetchRawDataCyclesBeenCalled() const {
            return fetchMessagesInvokedBeforeFetchRawDataCycles.load();
        }

        bool hasFetchRawDataCyclesBeenInvokedBeforeSendRequest() const {
            return fetchRawDataCyclesInvokedBeforeSendRequest.load();
        }

        AtomicFlag fetchMessagesCalled;
        AtomicFlag fetchRawDataCyclesCalled;
        AtomicFlag sendCommandCalled;

        Mutex callSequenceMutex;
        AtomicFlag fetchMessagesInvokedBeforeFetchRawDataCycles;
        AtomicFlag fetchRawDataCyclesInvokedBeforeSendRequest;

        Mutex fetchMessagesAckMutex;
        mutable BooleanPromise fetchMessagesCalledAcknowledgement;

        Mutex fetchRawDataCyclesAckMutex;
        mutable BooleanPromise fetchRawDataCyclesCalledAcknowledgement;

        bool hasToReturnSpecificMessages;
        bool hasToReturnSpecificRawDataCycles;
        RealisticMessageList messagesToReturn;
        RealisticRawDataList rawDataCyclesToReturn;

        MockFunctionSendRequest mockFunctionSendRequest;
    };

}

class SensorCommunicatorTest : public ::testing::Test {

public:

    using Error = ErrorHandling::SensorAccessLinkError;
    using ErrorSinkMock = Mock::ArbitraryDataSinkMock<Error>;
    using ErrorProcessingScheduler = DataFlow::DataProcessingScheduler<Error, ErrorSinkMock, ONLY_ONE_PRODUCER>;
    using ThrowingSensorCommunicationStrategyMock = Mock::ErrorThrowingSensorCommunicationStrategyMock<Sensor::Test::RealisticImplementation::Structures>;

protected:

    SensorCommunicatorTest() = default;

    virtual ~SensorCommunicatorTest() = default;

    RealisticMessageList createASequenceOfDifferentMessagesOfSize(uint64_t numberOfMessagesToCreate) const noexcept;

    RealisticRawDataList
    createASequenceOfDifferentRawDataCyclesOfSize(uint64_t numberOfRawDataCyclesToCreate) const noexcept;

    RealisticMessageList fetchMessageProducedBySensorCommunicatorExecution(
            RealisticMessageList&& messages, uint8_t numberOfMessagesToReceive);

    RealisticRawDataList fetchRawDataProducedBySensorCommunicatorExecution(
            RealisticRawDataList&& rawDataCycles, uint8_t numberOfRawDataToReceive);

    ::testing::AssertionResult openConnectionIsCalledAfterStrategyHasThrown(
            ThrowingSensorCommunicationStrategyMock* throwingStrategy) {
        RealisticMessageSensorCommunicator sensorCommunicator(throwingStrategy);

        sensorCommunicator.start();
        throwingStrategy->waitUntilOpenConnectionCallIsMadeAfterErrorIsThrown();

        auto openConnectionCalled = throwingStrategy->hasOpenConnectionBeenCalled();

        sensorCommunicator.terminateAndJoin();
        if (openConnectionCalled) {
            return ::testing::AssertionSuccess();
        } else {
            return ::testing::AssertionFailure()
                    << "\nExpected a call to \"openConnection\" during the error handling\n";
        }
    }

    ::testing::AssertionResult openConnectionIsCalledAfterStrategyHasThrownAfterClose(
            ThrowingSensorCommunicationStrategyMock* throwingStrategy) {
        RealisticMessageSensorCommunicator sensorCommunicator(throwingStrategy);

        sensorCommunicator.terminateAndJoin();
        throwingStrategy->waitUntilOpenConnectionCallIsMadeAfterErrorIsThrown();

        auto openConnectionCalled = throwingStrategy->hasOpenConnectionBeenCalled();

        if (openConnectionCalled) {
            return ::testing::AssertionSuccess();
        } else {
            return ::testing::AssertionFailure()
                    << "\nExpected a call to \"openConnection\" during the error handling\n";
        }
    }

    ::testing::AssertionResult closeConnectionIsCalledAfterStrategyHasThrown(
            ThrowingSensorCommunicationStrategyMock* throwingStrategy) {
        RealisticMessageSensorCommunicator sensorCommunicator(throwingStrategy);

        sensorCommunicator.start();
        throwingStrategy->waitUntilCloseConnectionCallIsMadeAfterErrorIsThrown();

        auto closeConnectionCalled = throwingStrategy->hasCloseConnectionBeenCalled();

        sensorCommunicator.terminateAndJoin();
        if (closeConnectionCalled) {
            return ::testing::AssertionSuccess();
        } else {
            return ::testing::AssertionFailure()
                    << "\nExpected a call to \"closeConnection\" during the error handling\n";
        }
    }

    ::testing::AssertionResult closeConnectionIsCalledAfterStrategyHasThrownAfterClose(
            ThrowingSensorCommunicationStrategyMock* throwingStrategy) {
        RealisticMessageSensorCommunicator sensorCommunicator(throwingStrategy);

        sensorCommunicator.terminateAndJoin();
        throwingStrategy->waitUntilCloseConnectionCallIsMadeAfterErrorIsThrown();

        auto closeConnectionCalled = throwingStrategy->hasCloseConnectionBeenCalled();

        if (closeConnectionCalled) {
            return ::testing::AssertionSuccess();
        } else {
            return ::testing::AssertionFailure()
                    << "\nExpected a call to \"closeConnection\" during the error handling\n";
        }
    }

//    template<typename Communicator, typename Strategy>
    ::testing::AssertionResult expectedErrorGetsPublishedAfterThrow(
            ThrowingSensorCommunicationStrategyMock* throwingStrategy,
            ErrorHandling::SensorAccessLinkError&& expectedError) {
        auto numberOfErrorToReceive = 1;
        ErrorSinkMock sink(numberOfErrorToReceive);
        ErrorProcessingScheduler scheduler(&sink);
        RealisticMessageSensorCommunicator sensorCommunicator(throwingStrategy);

        sensorCommunicator.linkConsumer(&scheduler);

        sensorCommunicator.start();
        sink.waitConsumptionToBeReached();

        sensorCommunicator.terminateAndJoin();
        scheduler.terminateAndJoin();

        auto producedErrors = sink.getConsumedData();

        bool expectedErrorPublished = true;
        auto lastParsedError = ErrorHandling::SensorAccessLinkError::returnDefaultData();
        for (auto t = 0; t < numberOfErrorToReceive && expectedErrorPublished; ++t) {
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

    ::testing::AssertionResult expectedErrorGetsPublishedAfterThrowAtClose(
            ThrowingSensorCommunicationStrategyMock* throwingStrategy,
            ErrorHandling::SensorAccessLinkError&& expectedError) {
        auto numberOfErrorToReceive = 1;
        ErrorSinkMock sink(numberOfErrorToReceive);
        ErrorProcessingScheduler scheduler(&sink);
        RealisticMessageSensorCommunicator sensorCommunicator(throwingStrategy);

        sensorCommunicator.linkConsumer(&scheduler);

        std::this_thread::yield(); // Ensure the test work properly
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        sensorCommunicator.terminateAndJoin();

        sink.waitConsumptionToBeReached();
        scheduler.terminateAndJoin();

        auto producedErrors = sink.getConsumedData();

        bool expectedErrorPublished = true;
        auto lastParsedError = ErrorHandling::SensorAccessLinkError::returnDefaultData();
        for (auto t = 0; t < numberOfErrorToReceive && expectedErrorPublished; ++t) {
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

TEST_F(SensorCommunicatorTest, given__when_start_then_callsOpenConnectionInStrategy) {
    SensorCommunicatorTestMock::SensorCommunicationStrategy mockStrategy;
    RealisticMessageSensorCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.start();

    sensorCommunicator.terminateAndJoin();
    auto openConnectionCalled = mockStrategy.hasOpenConnectionBeenCalled();
    ASSERT_TRUE(openConnectionCalled);
}

TEST_F(SensorCommunicatorTest, given__when_terminateAndJoin_then_callsCloseConnectionInStrategy) {
    SensorCommunicatorTestMock::SensorCommunicationStrategy mockStrategy;
    RealisticMessageSensorCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.terminateAndJoin();

    auto closeConnectionCalled = mockStrategy.hasCloseConnectionBeenCalled();
    ASSERT_TRUE(closeConnectionCalled);
}

TEST_F(SensorCommunicatorTest, given__when_start_then_callsFetchMessagesInStrategy) {
    SensorCommunicatorTestMock::SensorCommunicationStrategy mockStrategy;
    RealisticMessageSensorCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.start();

    mockStrategy.waitUntilFetchMessagesInvocation();

    auto fetchMessagesCalled = mockStrategy.hasFetchMessagesBeenInvoked();
    sensorCommunicator.terminateAndJoin();
    ASSERT_TRUE(fetchMessagesCalled);
}

TEST_F(SensorCommunicatorTest, given__when_start_then_callsFetchRawDataCyclesInStrategy) {
    SensorCommunicatorTestMock::SensorCommunicationStrategy mockStrategy;
    RealisticMessageSensorCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.start();

    mockStrategy.waitUntilFetchRawDataCyclesInvocation();

    auto fetchRawDataCyclesCalled = mockStrategy.hasFetchRawDataCyclesBeenInvoked();
    sensorCommunicator.terminateAndJoin();
    ASSERT_TRUE(fetchRawDataCyclesCalled);
}

TEST_F(SensorCommunicatorTest, given__when_start_then_callsFetchMessagesBeforeFetchRawDataAndFetchRawDataCalledBeforeSendRequest) {
    SensorCommunicatorTestMock::SensorCommunicationStrategy mockStrategy;
    RealisticMessageSensorCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.start();

    auto request = TestFunctions::DataTestUtil::createRandomRealisticSensorRequest();
    sensorCommunicator.consume(std::move(request));

    mockStrategy.waitUntilSendRequestInvocation();

    auto sequenceRespected = mockStrategy.hasMessageAndRawDataCallSequenceBeenRespected();
    sensorCommunicator.terminateAndJoin();
    ASSERT_TRUE(sequenceRespected);
}

TEST_F(SensorCommunicatorTest,
       given_aSensorRequestMessage_when_consumeRequest_then_sendsTheRequestToTheStrategy) {
    auto request = DataTestUtil::createRandomRealisticSensorRequest();
    auto copy = decltype(DataTestUtil::createRandomRealisticSensorRequest())(request);

    SensorCommunicatorTestMock::SensorCommunicationStrategy mockStrategy;
    RealisticMessageSensorCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.start();
    sensorCommunicator.consume(std::move(request));

    mockStrategy.waitUntilSendRequestInvocation();

    auto strategyReceivedRequest = mockStrategy.hasSendRequestBeenInvokedWith(copy);

    sensorCommunicator.terminateAndJoin();

    ASSERT_TRUE(strategyReceivedRequest);
}

using RealisticMessageProcessingScheduler = DataFlow::DataProcessingScheduler<RealisticMessage, RealisticMessageSinkMock, ONLY_ONE_PRODUCER>;

RealisticMessageList SensorCommunicatorTest::fetchMessageProducedBySensorCommunicatorExecution(
        RealisticMessageList&& messages, uint8_t numberOfMessagesToReceive) {
    RealisticMessageSinkMock sink(numberOfMessagesToReceive);
    RealisticMessageProcessingScheduler scheduler(&sink);

    SensorCommunicatorTestMock::SensorCommunicationStrategy mockStrategy;
    mockStrategy.returnThisMessageSequenceWhenFetchMessagesIsCalled(std::forward<RealisticMessageList>(messages));
    RealisticMessageSensorCommunicator sensorCommunicator(&mockStrategy);
    sensorCommunicator.linkConsumer(&scheduler);

    sensorCommunicator.start();

    sink.waitConsumptionToBeReached();

    sensorCommunicator.terminateAndJoin();
    scheduler.terminateAndJoin();

    RealisticMessageList producedMessages = sink.getConsumedData();

    return producedMessages;
}

TEST_F(SensorCommunicatorTest, given_aSequenceOfOneIncomingMessage_when_start_then_willProduceThisData) {
    auto numberOfMessages = 1u;
    auto messages = createASequenceOfDifferentMessagesOfSize(numberOfMessages);
    RealisticMessage expectedMessage = RealisticMessage(messages.front());

    auto resultingMessage = fetchMessageProducedBySensorCommunicatorExecution(
            std::move(messages), numberOfMessages).front();

    ASSERT_EQ(expectedMessage, resultingMessage);
}

TEST_F(SensorCommunicatorTest,
       given_aSequenceOfSeveralIncomingMessages_when_start_then_willProduceTheseMessagesInTheSameOrderTheyAreRead) {
    auto numberOfMessages = 5u;
    auto messages = createASequenceOfDifferentMessagesOfSize(numberOfMessages);
    RealisticMessageList expectedMessages = messages;

    auto resultedMessages = fetchMessageProducedBySensorCommunicatorExecution(
            std::move(messages), numberOfMessages);

    for (auto t = 0; t < numberOfMessages; ++t) {
        ASSERT_EQ(expectedMessages.front(), resultedMessages.front());
        expectedMessages.pop_front();
        resultedMessages.pop_front();
    }
}

TEST_F(SensorCommunicatorTest,
       given_aStrategyThatReturnsDefaultAndNonDefaultMessages_when_start_then_willNotProduceDefaultMessages) {
    auto numberOfDefaultMessages = 5u;
    auto numberOfMessages = 2u;
    auto messages = createASequenceOfDifferentMessagesOfSize(numberOfMessages);
    RealisticMessageList expectedMessages = messages;
    RealisticMessageList messageToProduce;
    for (auto defaultMessageIndex = 0; defaultMessageIndex < numberOfDefaultMessages; ++defaultMessageIndex) {
        messageToProduce.push_back(RealisticMessage::returnDefaultData());
    }
    for (auto realMessageIndex = 0; realMessageIndex < numberOfMessages; ++realMessageIndex) {
        messageToProduce.push_back(messages.front());
        messages.pop_front();
    }

    auto resultedMessages = fetchMessageProducedBySensorCommunicatorExecution(
            std::move(messageToProduce), numberOfMessages);

    for (auto t = 0; t < numberOfMessages; ++t) {
        ASSERT_EQ(expectedMessages.front(), resultedMessages.front());
        expectedMessages.pop_front();
        resultedMessages.pop_front();
    }
}

RealisticMessageList
SensorCommunicatorTest::createASequenceOfDifferentMessagesOfSize(uint64_t numberOfMessagesToCreate) const noexcept {
    RealisticMessageList messages;
    for (uint8_t offset = 0; offset < numberOfMessagesToCreate; ++offset) {
        auto message = TestFunctions::DataTestUtil::createRandomRealisticMessageWithEmptyTimestamps();
        messages.push_back(message);
    }
    return messages;
}

using RealisticRawDataProcessingScheduler = DataFlow::DataProcessingScheduler<RealisticRawData, RealisticRawDataSinkMock, ONLY_ONE_PRODUCER>;

RealisticRawDataList SensorCommunicatorTest::fetchRawDataProducedBySensorCommunicatorExecution(
        RealisticRawDataList&& rawDataCycles, uint8_t numberOfRawDataToReceive) {
    RealisticRawDataSinkMock sink(numberOfRawDataToReceive);
    RealisticRawDataProcessingScheduler scheduler(&sink);

    SensorCommunicatorTestMock::SensorCommunicationStrategy mockStrategy;
    mockStrategy.returnThisRawDataCyclesSequenceWhenFetchRawDataCyclesIsCalled(
            std::forward<RealisticRawDataList>(rawDataCycles));
    RealisticMessageSensorCommunicator sensorCommunicator(&mockStrategy);
    sensorCommunicator.linkConsumer(&scheduler);

    sensorCommunicator.start();

    sink.waitConsumptionToBeReached();

    sensorCommunicator.terminateAndJoin();
    scheduler.terminateAndJoin();

    RealisticRawDataList producedRawData = sink.getConsumedData();

    return producedRawData;
}

TEST_F(SensorCommunicatorTest, given_aSequenceOfOneIncomingRawData_when_start_then_willProduceThisData) {
    auto numberOfRawData = 1u;
    auto rawDataCycles = createASequenceOfDifferentRawDataCyclesOfSize(numberOfRawData);
    auto expectedRawData = RealisticRawData(rawDataCycles.front());

    auto resultingRawData = fetchRawDataProducedBySensorCommunicatorExecution(
            std::move(rawDataCycles), numberOfRawData).front();

    ASSERT_EQ(expectedRawData, resultingRawData);
}

TEST_F(SensorCommunicatorTest,
       given_aSequenceOfSeveralIncomingRawDataCycles_when_start_then_willProduceTheseRawDataCyclesInTheSameOrderTheyAreRead) {
    auto numberOfRawData = 5u;
    auto rawDataCycles = createASequenceOfDifferentRawDataCyclesOfSize(numberOfRawData);
    auto expectedRawDataCycles = rawDataCycles;

    auto resultingRawDataCycles = fetchRawDataProducedBySensorCommunicatorExecution(
            std::move(rawDataCycles), numberOfRawData);

    for (auto t = 0; t < numberOfRawData; ++t) {
        ASSERT_EQ(expectedRawDataCycles.front(), resultingRawDataCycles.front());
        expectedRawDataCycles.pop_front();
        resultingRawDataCycles.pop_front();
    }
}

TEST_F(SensorCommunicatorTest,
       given_aStrategyThatReturnsDefaultAndNonDefaultRawData_when_start_then_willNotProduceDefaultRawData) {
    auto numberOfDefaultRawDataCycles = 5u;
    auto numberOfRawDataCycles = 2u;
    auto rawDataCycles = createASequenceOfDifferentRawDataCyclesOfSize(numberOfRawDataCycles);
    RealisticRawDataList expectedRawDataCycles = rawDataCycles;
    RealisticRawDataList messageToProduce;
    for (auto defaultRawDataIndex = 0; defaultRawDataIndex < numberOfDefaultRawDataCycles; ++defaultRawDataIndex) {
        messageToProduce.push_back(RealisticRawData::returnDefaultData());
    }
    for (auto realRawDataIndex = 0; realRawDataIndex < numberOfRawDataCycles; ++realRawDataIndex) {
        messageToProduce.push_back(rawDataCycles.front());
        rawDataCycles.pop_front();
    }

    auto resultedRawDataCycles = fetchRawDataProducedBySensorCommunicatorExecution(
            std::move(messageToProduce), numberOfRawDataCycles);

    for (auto t = 0; t < numberOfRawDataCycles; ++t) {
        ASSERT_EQ(expectedRawDataCycles.front(), resultedRawDataCycles.front());
        expectedRawDataCycles.pop_front();
        resultedRawDataCycles.pop_front();
    }
}

RealisticRawDataList SensorCommunicatorTest::createASequenceOfDifferentRawDataCyclesOfSize(
        uint64_t numberOfRawDataCyclesToCreate) const noexcept {
    RealisticRawDataList rawDataCycles;
    for (auto i = 0; i < numberOfRawDataCyclesToCreate; ++i) {
        auto rawData = TestFunctions::DataTestUtil::createRandomRealisticRawData();
        rawDataCycles.push_back(rawData);
    }
    return rawDataCycles;
}

TEST_F(SensorCommunicatorTest,
       given_anErrorThatRequiresToCloseThrownOnOpenConnectionCall_when_isCaught_then_callsCloseConnectionInStrategy) {
    ThrowingSensorCommunicationStrategyMock throwingMockStrategy;
    throwingMockStrategy.throwCloseConnectionRequiredErrorWhenOpenConnectionIsCalled();
    ASSERT_TRUE(closeConnectionIsCalledAfterStrategyHasThrown(&throwingMockStrategy));
}

TEST_F(SensorCommunicatorTest,
       given_anErrorThatRequiresToOpenThrownOnOpenConnectionCall_when_isCaught_then_callsOpenConnectionInStrategy) {
    ThrowingSensorCommunicationStrategyMock throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenOpenConnectionIsCalled();
    ASSERT_TRUE(openConnectionIsCalledAfterStrategyHasThrown(&throwingMockStrategy));
}

TEST_F(SensorCommunicatorTest,
       given_anErrorThatRequiresToCloseThrownOnCloseConnectionCall_when_isCaught_then_callsCloseConnectionInStrategy) {
    ThrowingSensorCommunicationStrategyMock throwingMockStrategy;
    throwingMockStrategy.throwCloseConnectionRequiredErrorWhenCloseConnectionIsCalled();
    ASSERT_TRUE(closeConnectionIsCalledAfterStrategyHasThrownAfterClose(&throwingMockStrategy));
}

TEST_F(SensorCommunicatorTest,
       given_anErrorThatRequiresToOpenThrownOnCloseConnectionCall_when_isCaught_then_callsOpenConnectionInStrategy) {
    ThrowingSensorCommunicationStrategyMock throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenCloseConnectionIsCalled();
    ASSERT_TRUE(openConnectionIsCalledAfterStrategyHasThrownAfterClose(&throwingMockStrategy));
}

TEST_F(SensorCommunicatorTest,
       given_anErrorThatRequiresToCloseThrownOnFetchMessagesCall_when_isCaught_then_callsCloseConnectionInStrategy) {
    ThrowingSensorCommunicationStrategyMock throwingMockStrategy;
    throwingMockStrategy.throwCloseConnectionRequiredErrorWhenFetchMessagesIsCalled();
    ASSERT_TRUE(closeConnectionIsCalledAfterStrategyHasThrown(&throwingMockStrategy));
}

TEST_F(SensorCommunicatorTest,
       given_anErrorThatRequiresToOpenThrownOnFetchMessagesCall_when_isCaught_then_callsOpenConnectionInStrategy) {
    ThrowingSensorCommunicationStrategyMock throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenFetchMessagesIsCalled();
    ASSERT_TRUE(openConnectionIsCalledAfterStrategyHasThrown(&throwingMockStrategy));
}

TEST_F(SensorCommunicatorTest,
       given_anErrorThatRequiresToCloseThrownOnFetchRawDataCyclesCall_when_isCaught_then_callsCloseConnectionInStrategy) {
    ThrowingSensorCommunicationStrategyMock throwingMockStrategy;
    throwingMockStrategy.throwCloseConnectionRequiredErrorWhenFetchRawDataCyclesIsCalled();
    ASSERT_TRUE(closeConnectionIsCalledAfterStrategyHasThrown(&throwingMockStrategy));
}

TEST_F(SensorCommunicatorTest,
       given_anErrorThatRequiresToOpenThrownOnFetchRawDataCyclesCall_when_isCaught_then_callsOpenConnectionInStrategy) {
    ThrowingSensorCommunicationStrategyMock throwingMockStrategy;
    throwingMockStrategy.throwOpenConnectionRequiredErrorWhenFetchRawDataCyclesIsCalled();
    ASSERT_TRUE(openConnectionIsCalledAfterStrategyHasThrown(&throwingMockStrategy));
}

TEST_F(SensorCommunicatorTest,
       given_anStrategyThrowingErrorOnOpenConnectionCall_when_errorIsCaught_then_producesTheErrorWithCorrectOrigin) {
    ThrowingSensorCommunicationStrategyMock throwingMockStrategy;
    throwingMockStrategy.throwErrorWhenOpenConnectionIsCalled();
    Error expectedError = throwingMockStrategy.expectedErrorWhenOpenConnectionIsCalled();
    expectedError = formatStrategyErrorWithCorrectOrigin(expectedError,
                                                         ErrorHandling::Origin::SENSOR_COMMUNICATOR_OPEN_CONNECTION);
    ASSERT_TRUE(expectedErrorGetsPublishedAfterThrow(&throwingMockStrategy, std::move(expectedError)));
}

TEST_F(SensorCommunicatorTest,
       given_aStrategyThrowingErrorOnCloseConnectionCall_when_errorIsCaught_then_producesTheErrorWithCorrectOrigin) {
    ThrowingSensorCommunicationStrategyMock throwingMockStrategy;
    throwingMockStrategy.throwErrorWhenCloseConnectionIsCalled();
    Error expectedError = throwingMockStrategy.expectedErrorWhenCloseConnectionIsCalled();
    expectedError = formatStrategyErrorWithCorrectOrigin(expectedError,
                                                         ErrorHandling::Origin::SENSOR_COMMUNICATOR_CLOSE_CONNECTION);
    ASSERT_TRUE(expectedErrorGetsPublishedAfterThrowAtClose(&throwingMockStrategy, std::move(expectedError)));
}

TEST_F(SensorCommunicatorTest,
       given_aStrategyThrowingErrorOnFetchMessagesCall_when_errorIsCaught_then_producesTheErrorWithCorrectOrigin) {
    ThrowingSensorCommunicationStrategyMock throwingMockStrategy;
    throwingMockStrategy.throwErrorWhenFetchMessagesIsCalled();
    Error expectedError = throwingMockStrategy.expectedErrorWhenFetchMessagesIsCalled();
    expectedError = formatStrategyErrorWithCorrectOrigin(expectedError,
                                                         ErrorHandling::Origin::SENSOR_COMMUNICATOR_HANDLE_MESSAGE);
    ASSERT_TRUE(expectedErrorGetsPublishedAfterThrow(&throwingMockStrategy, std::move(expectedError)));
}

TEST_F(SensorCommunicatorTest,
       given_aStrategyThrowingErrorOnFetchRawDataCyclesCall_when_errorIsCaught_then_producesTheErrorWithCorrectOrigin) {
    ThrowingSensorCommunicationStrategyMock throwingMockStrategy;
    throwingMockStrategy.throwErrorWhenFetchRawDataCyclesIsCalled();
    Error expectedError = throwingMockStrategy.expectedErrorWhenFetchRawDataCyclesIsCalled();
    expectedError = formatStrategyErrorWithCorrectOrigin(expectedError,
                                                         ErrorHandling::Origin::SENSOR_COMMUNICATOR_HANDLE_RAWDATA);
    ASSERT_TRUE(expectedErrorGetsPublishedAfterThrow(&throwingMockStrategy, std::move(expectedError)));
}

#endif //SENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP
