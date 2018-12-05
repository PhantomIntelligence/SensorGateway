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

#ifndef SENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP
#define SENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP

#include <gtest/gtest.h>

#include "sensor-gateway/sensor-communication/SensorCommunicator.hpp"
#include "test/utilities/mock/ArbitraryDataSinkMock.hpp"
#include "test/utilities/mock/ErrorThrowingSensorCommunicationStrategyMock.hpp"

using SimpleMessage = Sensor::Test::Simple::Structures::Message;
using SimpleRawData = Sensor::Test::Simple::Structures::RawData;

using SimpleMessageSinkMock = Mock::ArbitraryDataSinkMock<SimpleMessage>;
using SimpleRawDataSinkMock = Mock::ArbitraryDataSinkMock<SimpleRawData>;

using SimpleMessageList = SimpleMessageSinkMock::DataList;
using SimpleRawDataList = SimpleRawDataSinkMock::DataList;

using SimpleMessageSensorCommunicator = SensorAccessLinkElement::SensorCommunicator<Sensor::Test::Simple::Structures>;
using TestFunctions::DataTestUtil;


namespace SensorCommunicatorTestMock {

    class SensorCommunicationStrategy
            : public SensorCommunication::SensorCommunicationStrategy<Sensor::Test::Simple::Structures> {

    protected:

        using super = SensorCommunication::SensorCommunicationStrategy<Sensor::Test::Simple::Structures>;

    public:

        SensorCommunicationStrategy() :
                openConnectionCalled(false),
                closeConnectionCalled(false),
                fetchMessagesCalled(false),
                fetchRawDataCyclesCalled(false),
                sendCommandCalled(false),
                fetchMessagesCalledBeforeFetchRawDataCycles(false),
                fetchRawDataCyclesCalledBeforeSendCommand(false),
                hasToReturnSpecificMessages(false),
                hasToReturnSpecificRawDataCycles(false) {
        }

        ~SensorCommunicationStrategy() noexcept override = default;

        SensorCommunicationStrategy(SensorCommunicationStrategy const& other) = delete;

        SensorCommunicationStrategy(SensorCommunicationStrategy&& other) noexcept = delete;

        SensorCommunicationStrategy& operator=(SensorCommunicationStrategy const& other)& = delete;

        SensorCommunicationStrategy&
        operator=(SensorCommunicationStrategy&& other)& noexcept = delete;

        void returnThisMessageSequenceWhenFetchMessagesIsCalled(SimpleMessageList&& dataToReturn) {
            hasToReturnSpecificMessages = true;
            this->messagesToReturn = std::forward<SimpleMessageList>(dataToReturn);
        }

        void returnThisRawDataCyclesSequenceWhenFetchRawDataCyclesIsCalled(SimpleRawDataList&& dataToReturn) {
            hasToReturnSpecificRawDataCycles = true;
            this->rawDataCyclesToReturn = std::forward<SimpleRawDataList>(dataToReturn);
        }

        super::Messages fetchMessages() override {
            acknowledgeFetchMessagesHasBeenCalled();
            if (hasToReturnSpecificMessages && !messagesToReturn.empty()) {
                SimpleMessage message = messagesToReturn.front();
                messagesToReturn.pop_front();
                super::Messages messages = {message};
                return messages;
            }

            // WARNING! This mock implementation of fetchMessages needs to be slowed down because the way gtest works. DO NOT REMOVE.
            std::this_thread::yield();

            auto message = super::Message::returnDefaultData();
            super::Messages messages = {message};
            return messages;
        }

        super::RawDataCycles fetchRawDataCycles() override {
            acknowledgeFetchRawDataCyclesHasBeenCalled();
            if (hasToReturnSpecificRawDataCycles && !rawDataCyclesToReturn.empty()) {
                SimpleRawData rawDataCycle = rawDataCyclesToReturn.front();
                rawDataCyclesToReturn.pop_front();
                super::RawDataCycles rawDataCycles = {rawDataCycle};
                return rawDataCycles;
            }
            std::this_thread::yield();

            auto rawDataCycle = super::RawData::returnDefaultData();
            super::RawDataCycles rawDataCycles = {rawDataCycle};
            return rawDataCycles;
        }

        void openConnection() override {
            openConnectionCalled.store(true);
        }

        void closeConnection() override {
            closeConnectionCalled.store(true);
        }

        bool hasOpenConnectionBeenCalled() const {
            return openConnectionCalled.load();
        }

        bool hasCloseConnectionBeenCalled() const {
            return closeConnectionCalled.load();
        }

        bool hasFetchMessagesBeenCalled() const {
            return fetchMessagesCalled.load();
        }

        bool hasFetchRawDataCyclesBeenCalled() const {
            return fetchRawDataCyclesCalled.load();
        }

        bool hasMessageAndRawDataCallSequenceBeenRespected() const {
            bool sequenceRespected =
                    hasFetchMessagesBeenCalled() &&
                    hasFetchRawDataCyclesBeenCalled() &&
                    hasFetchMessagesCalledBeforeFetchRawDataCyclesBeenCalled();
            return sequenceRespected;
        };

        void waitUntilFetchMessagesIsCalled() {
            if (!hasFetchMessagesBeenCalled()) {
                fetchMessagesCalledAcknowledgement.get_future().wait();
            }
        }

        void waitUntilFetchRawDataCyclesIsCalled() {
            if (!hasFetchRawDataCyclesBeenCalled()) {
                fetchRawDataCyclesCalledAcknowledgement.get_future().wait();
            }
        }

    protected:
        AtomicFlag openConnectionCalled;
        AtomicFlag closeConnectionCalled;

    private:

        void acknowledgeFetchMessagesHasBeenCalled() {
            LockGuard sequenceGuard(callSequenceMutex);
            LockGuard guard(fetchMessagesAckMutex);
            if (!hasFetchMessagesBeenCalled()) {
                fetchMessagesCalled.store(true);
                fetchMessagesCalledAcknowledgement.set_value(true);
                if (!hasFetchRawDataCyclesBeenCalled()) {
                    fetchMessagesCalledBeforeFetchRawDataCycles.store(true);
                }
            }
        }

        void acknowledgeFetchRawDataCyclesHasBeenCalled() {
            LockGuard sequenceGuard(callSequenceMutex);
            LockGuard guard(fetchRawDataCyclesAckMutex);
            if (!hasFetchRawDataCyclesBeenCalled()) {
                fetchRawDataCyclesCalled.store(true);
                fetchRawDataCyclesCalledAcknowledgement.set_value(true);
            }
        }

        bool hasFetchMessagesCalledBeforeFetchRawDataCyclesBeenCalled() const {
            return fetchMessagesCalledBeforeFetchRawDataCycles.load();
        }


        AtomicFlag fetchMessagesCalled;
        AtomicFlag fetchRawDataCyclesCalled;
        AtomicFlag sendCommandCalled;

        Mutex callSequenceMutex;
        AtomicFlag fetchMessagesCalledBeforeFetchRawDataCycles;
        AtomicFlag fetchRawDataCyclesCalledBeforeSendCommand;

        Mutex fetchMessagesAckMutex;
        mutable BooleanPromise fetchMessagesCalledAcknowledgement;

        Mutex fetchRawDataCyclesAckMutex;
        mutable BooleanPromise fetchRawDataCyclesCalledAcknowledgement;

        bool hasToReturnSpecificMessages;
        bool hasToReturnSpecificRawDataCycles;
        SimpleMessageList messagesToReturn;
        SimpleRawDataList rawDataCyclesToReturn;
    };

}

class SensorCommunicatorTest : public ::testing::Test {

public:

    using Error = ErrorHandling::SensorAccessLinkError;
    using ErrorSinkMock = Mock::ArbitraryDataSinkMock<Error>;
    using ErrorProcessingScheduler = DataFlow::DataProcessingScheduler<Error, ErrorSinkMock, 1>;
    using ThrowingSensorCommunicationStrategyMock = Mock::ErrorThrowingSensorCommunicationStrategyMock<Sensor::Test::Simple::Structures>;
protected:

    SensorCommunicatorTest() = default;

    virtual ~SensorCommunicatorTest() = default;

    SimpleMessageList createASequenceOfDifferentMessagesOfSize(uint64_t numberOfMessagesToCreate) const noexcept;

    SimpleRawDataList
    createASequenceOfDifferentRawDataCyclesOfSize(uint64_t numberOfRawDataCyclesToCreate) const noexcept;

    SimpleMessageList fetchMessageProducedBySensorCommunicatorExecution(
            SimpleMessageList&& messages, uint8_t numberOfMessagesToReceive);

    SimpleRawDataList fetchRawDataProducedBySensorCommunicatorExecution(
            SimpleRawDataList&& rawDataCycles, uint8_t numberOfRawDataToReceive);

    Error formatStrategyErrorWithCorrectOrigin(Error strategyIssuedError, std::string origin) const noexcept {
        Error formattedError(origin + ErrorHandling::Message::SEPARATOR + strategyIssuedError.getOrigin(),
                             strategyIssuedError.getCategory(),
                             strategyIssuedError.getSeverity(),
                             strategyIssuedError.getErrorCode(),
                             strategyIssuedError.getMessage());
        return formattedError;
    }

    ::testing::AssertionResult openConnectionIsCalledAfterStrategyHasThrown(
            ThrowingSensorCommunicationStrategyMock* throwingStrategy) {
        SimpleMessageSensorCommunicator sensorCommunicator(throwingStrategy);

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
        SimpleMessageSensorCommunicator sensorCommunicator(throwingStrategy);

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
        SimpleMessageSensorCommunicator sensorCommunicator(throwingStrategy);

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
        SimpleMessageSensorCommunicator sensorCommunicator(throwingStrategy);

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

    ::testing::AssertionResult expectedErrorGetsPublishedAfterThrow(
            ThrowingSensorCommunicationStrategyMock* throwingStrategy,
            ErrorHandling::SensorAccessLinkError&& expectedError) {
        auto numberOfErrorToReceive = 1;
        ErrorSinkMock sink(numberOfErrorToReceive);
        ErrorProcessingScheduler scheduler(&sink);
        SimpleMessageSensorCommunicator sensorCommunicator(throwingStrategy);

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
        SimpleMessageSensorCommunicator sensorCommunicator(throwingStrategy);

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
    SimpleMessageSensorCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.start();

    sensorCommunicator.terminateAndJoin();
    auto openConnectionCalled = mockStrategy.hasOpenConnectionBeenCalled();
    ASSERT_TRUE(openConnectionCalled);
}

TEST_F(SensorCommunicatorTest, given__when_terminateAndJoin_then_callsCloseConnectionInStrategy) {
    SensorCommunicatorTestMock::SensorCommunicationStrategy mockStrategy;
    SimpleMessageSensorCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.terminateAndJoin();

    auto closeConnectionCalled = mockStrategy.hasCloseConnectionBeenCalled();
    ASSERT_TRUE(closeConnectionCalled);
}

TEST_F(SensorCommunicatorTest, given__when_start_then_callsFetchMessagesInStrategy) {
    SensorCommunicatorTestMock::SensorCommunicationStrategy mockStrategy;
    SimpleMessageSensorCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.start();

    mockStrategy.waitUntilFetchMessagesIsCalled();

    auto fetchMessagesCalled = mockStrategy.hasFetchMessagesBeenCalled();
    sensorCommunicator.terminateAndJoin();
    ASSERT_TRUE(fetchMessagesCalled);
}

TEST_F(SensorCommunicatorTest, given__when_start_then_callsFetchRawDataCyclesInStrategy) {
    SensorCommunicatorTestMock::SensorCommunicationStrategy mockStrategy;
    SimpleMessageSensorCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.start();

    mockStrategy.waitUntilFetchRawDataCyclesIsCalled();

    auto fetchRawDataCyclesCalled = mockStrategy.hasFetchRawDataCyclesBeenCalled();
    sensorCommunicator.terminateAndJoin();
    ASSERT_TRUE(fetchRawDataCyclesCalled);
}

TEST_F(SensorCommunicatorTest, given__when_start_then_callsFetchMessagesBeforeFetchRawData) {
    SensorCommunicatorTestMock::SensorCommunicationStrategy mockStrategy;
    SimpleMessageSensorCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.start();

    mockStrategy.waitUntilFetchRawDataCyclesIsCalled();
    auto sequenceRespected = mockStrategy.hasMessageAndRawDataCallSequenceBeenRespected();
    sensorCommunicator.terminateAndJoin();
    ASSERT_TRUE(sequenceRespected);
}

using SimpleMessageProcessingScheduler = DataFlow::DataProcessingScheduler<SimpleMessage, SimpleMessageSinkMock, 1>;

SimpleMessageList SensorCommunicatorTest::fetchMessageProducedBySensorCommunicatorExecution(
        SimpleMessageList&& messages, uint8_t numberOfMessagesToReceive) {
    SimpleMessageSinkMock sink(numberOfMessagesToReceive);
    SimpleMessageProcessingScheduler scheduler(&sink);

    SensorCommunicatorTestMock::SensorCommunicationStrategy mockStrategy;
    mockStrategy.returnThisMessageSequenceWhenFetchMessagesIsCalled(std::forward<SimpleMessageList>(messages));
    SimpleMessageSensorCommunicator sensorCommunicator(&mockStrategy);
    sensorCommunicator.linkConsumer(&scheduler);

    sensorCommunicator.start();

    sink.waitConsumptionToBeReached();

    sensorCommunicator.terminateAndJoin();
    scheduler.terminateAndJoin();

    SimpleMessageList producedMessages = sink.getConsumedData();

    return producedMessages;
}

TEST_F(SensorCommunicatorTest, given_aSequenceOfOneIncomingMessage_when_start_then_willProduceThisData) {
    auto numberOfMessages = 1u;
    auto messages = createASequenceOfDifferentMessagesOfSize(numberOfMessages);
    SimpleMessage expectedMessage = SimpleMessage(messages.front());

    auto resultingMessage = fetchMessageProducedBySensorCommunicatorExecution(
            std::move(messages), numberOfMessages).front();

    ASSERT_EQ(expectedMessage, resultingMessage);
}

TEST_F(SensorCommunicatorTest,
       given_aSequenceOfSeveralIncomingMessages_when_start_then_willProduceTheseMessagesInTheSameOrderTheyAreRead) {
    auto numberOfMessages = 5u;
    auto messages = createASequenceOfDifferentMessagesOfSize(numberOfMessages);
    SimpleMessageList expectedMessages = messages;

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
    SimpleMessageList expectedMessages = messages;
    SimpleMessageList messageToProduce;
    for (auto defaultMessageIndex = 0; defaultMessageIndex < numberOfDefaultMessages; ++defaultMessageIndex) {
        messageToProduce.push_back(SimpleMessage::returnDefaultData());
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

SimpleMessageList
SensorCommunicatorTest::createASequenceOfDifferentMessagesOfSize(uint64_t numberOfMessagesToCreate) const noexcept {
    SimpleMessageList messages;
    for (uint8_t offset = 0; offset < numberOfMessagesToCreate; ++offset) {
        auto message = TestFunctions::DataTestUtil::createRandomSimpleMessageWithEmptyTimestamps();
        messages.push_back(message);
    }
    return messages;
}

using SimpleRawDataProcessingScheduler = DataFlow::DataProcessingScheduler<SimpleRawData, SimpleRawDataSinkMock, 1>;

SimpleRawDataList SensorCommunicatorTest::fetchRawDataProducedBySensorCommunicatorExecution(
        SimpleRawDataList&& rawDataCycles, uint8_t numberOfRawDataToReceive) {
    SimpleRawDataSinkMock sink(numberOfRawDataToReceive);
    SimpleRawDataProcessingScheduler scheduler(&sink);

    SensorCommunicatorTestMock::SensorCommunicationStrategy mockStrategy;
    mockStrategy.returnThisRawDataCyclesSequenceWhenFetchRawDataCyclesIsCalled(
            std::forward<SimpleRawDataList>(rawDataCycles));
    SimpleMessageSensorCommunicator sensorCommunicator(&mockStrategy);
    sensorCommunicator.linkConsumer(&scheduler);

    sensorCommunicator.start();

    sink.waitConsumptionToBeReached();

    sensorCommunicator.terminateAndJoin();
    scheduler.terminateAndJoin();

    SimpleRawDataList producedRawData = sink.getConsumedData();

    return producedRawData;
}

TEST_F(SensorCommunicatorTest, given_aSequenceOfOneIncomingRawData_when_start_then_willProduceThisData) {
    auto numberOfRawData = 1u;
    auto rawDataCycles = createASequenceOfDifferentRawDataCyclesOfSize(numberOfRawData);
    auto expectedRawData = SimpleRawData(rawDataCycles.front());

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
    SimpleRawDataList expectedRawDataCycles = rawDataCycles;
    SimpleRawDataList messageToProduce;
    for (auto defaultRawDataIndex = 0; defaultRawDataIndex < numberOfDefaultRawDataCycles; ++defaultRawDataIndex) {
        messageToProduce.push_back(SimpleRawData::returnDefaultData());
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

SimpleRawDataList SensorCommunicatorTest::createASequenceOfDifferentRawDataCyclesOfSize(
        uint64_t numberOfRawDataCyclesToCreate) const noexcept {
    SimpleRawDataList rawDataCycles;
    for (auto i = 0; i < numberOfRawDataCyclesToCreate; ++i) {
        auto rawData = TestFunctions::DataTestUtil::createRandomSimpleRawData();
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
