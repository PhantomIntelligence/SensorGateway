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
#include "test/utilities/mock/SimpleStructuresSinkMock.h"

using SimpleMessage = Sensor::Test::Simple::Structures::Message;
using SimpleRawData = Sensor::Test::Simple::Structures::RawData;

using SimpleMessageSinkMock = Mock::SimpleDataSinkMock<SimpleMessage>;
using SimpleRawDataSinkMock = Mock::SimpleDataSinkMock<SimpleRawData>;

using SimpleMessageList = SimpleMessageSinkMock::DataList;
using SimpleRawDataList = SimpleRawDataSinkMock::DataList;

using SimpleMessageSensorCommunicator = SensorAccessLinkElement::SensorCommunicator<Sensor::Test::Simple::Structures>;
using TestFunctions::DataTestUtil;


class SensorCommunicatorTest : public ::testing::Test {

protected:
    SensorCommunicatorTest() = default;

    virtual ~SensorCommunicatorTest() = default;

    SimpleMessageList createASequenceOfDifferentMessagesOfSize(uint64_t numberOfMessagesToCreate) const noexcept;

    SimpleMessageList fetchMessageProducedBySensorCommunicatorExecution(
            SimpleMessageList&& messages, uint8_t numberOfMessagesToReceive);
};

namespace SensorCommunicatorTestMock {
    class SensorCommunicationStrategy final
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

        ~SensorCommunicationStrategy() noexcept final = default;

        SensorCommunicationStrategy(SensorCommunicationStrategy const& other) = delete;

        SensorCommunicationStrategy(SensorCommunicationStrategy&& other) noexcept = delete;

        SensorCommunicationStrategy& operator=(SensorCommunicationStrategy const& other)& = delete;

        SensorCommunicationStrategy&
        operator=(SensorCommunicationStrategy&& other)& noexcept = delete;

        void returnThisMessageSequenceWhenFetchMessagesIsCalled(SimpleMessageList&& dataToReturn) {
            hasToReturnSpecificMessages = true;
            this->messagesToReturn = std::forward<SimpleMessageList>(dataToReturn);
        }

        void returnThisMessageSequenceWhenFetchRawDataCyclesIsCalled(SimpleRawDataList&& dataToReturn) {
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

        AtomicFlag openConnectionCalled;
        AtomicFlag closeConnectionCalled;

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

TEST_F(SensorCommunicatorTest, given__when_start_then_callsOpenConnectionInStrategy) {
    SensorCommunicatorTestMock::SensorCommunicationStrategy mockStrategy;
    SimpleMessageSensorCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.start();

    sensorCommunicator.terminateAndJoin();
    auto strategyHasBeenCalled = mockStrategy.hasOpenConnectionBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(SensorCommunicatorTest, given__when_terminateAndJoin_then_callsCloseConnectionInStrategy) {
    SensorCommunicatorTestMock::SensorCommunicationStrategy mockStrategy;
    SimpleMessageSensorCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.terminateAndJoin();

    auto strategyHasBeenCalled = mockStrategy.hasCloseConnectionBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(SensorCommunicatorTest, given__when_start_then_callsFetchMessagesInStrategy) {
    SensorCommunicatorTestMock::SensorCommunicationStrategy mockStrategy;
    SimpleMessageSensorCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.start();

    mockStrategy.waitUntilFetchMessagesIsCalled();

    auto strategyHasBeenCalled = mockStrategy.hasFetchMessagesBeenCalled();
    sensorCommunicator.terminateAndJoin();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(SensorCommunicatorTest, given__when_start_then_callsFetchRawDataCyclesInStrategy) {
    SensorCommunicatorTestMock::SensorCommunicationStrategy mockStrategy;
    SimpleMessageSensorCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.start();

    mockStrategy.waitUntilFetchRawDataCyclesIsCalled();

    auto strategyHasBeenCalled = mockStrategy.hasFetchRawDataCyclesBeenCalled();
    sensorCommunicator.terminateAndJoin();
    ASSERT_TRUE(strategyHasBeenCalled);
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

using AWLProcessingScheduler = DataFlow::DataProcessingScheduler<SimpleMessage, SimpleMessageSinkMock, 1>;

SimpleMessageList SensorCommunicatorTest::fetchMessageProducedBySensorCommunicatorExecution(
        SimpleMessageList&& messages, uint8_t numberOfMessagesToReceive) {
    SimpleMessageSinkMock sink(numberOfMessagesToReceive);
    AWLProcessingScheduler scheduler(&sink);

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
    auto numberOfMessages = 1U;
    auto messages = createASequenceOfDifferentMessagesOfSize(numberOfMessages);
    SimpleMessage expectedMessage = SimpleMessage(messages.front());

    auto resultingMessage = fetchMessageProducedBySensorCommunicatorExecution(
            std::move(messages), numberOfMessages).front();

    ASSERT_EQ(expectedMessage, resultingMessage);
}

TEST_F(SensorCommunicatorTest,
       given_aSequenceOfSeveralIncomingMessages_when_start_then_willProduceTheseMessagesInTheSameOrderTheyAreRead) {
    auto numberOfMessages = 5U;
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
       given_aStrategyThatReturnsDefaultAndNonDefaultData_when_start_then_willNotProduceDefaultData) {
    auto numberOfDefaultMessages = 5U;
    auto numberOfMessages = 2U;
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
        auto message = TestFunctions::DataTestUtil::createRandomSimpleMessage();
        messages.push_back(message);
    }
    return messages;
}

#endif //SENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP
