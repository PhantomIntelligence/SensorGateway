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

#ifndef SPIRITSENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP
#define SPIRITSENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP

#include <gtest/gtest.h>
#include <list>

#include "spirit-sensor-gateway/sensor-communication/SensorCommunicator.hpp"
#include "test/utilities/data-model/DataModelFixture.h"

using DataFlow::AWLMessage;
using AWLMessages = std::list<AWLMessage>;
using AWLCommunicator = SensorAccessLinkElement::SensorCommunicator<AWLMessage>;
using TestFunctions::DataTestUtil;


class SensorCommunicatorTest : public ::testing::Test {

protected:
    SensorCommunicatorTest() = default;

    virtual ~SensorCommunicatorTest() = default;

    AWLMessage createAMessageWithValueOffsetOf(uint8_t dataValueOffset) const noexcept;

    AWLMessages createASequenceOfDifferentMessagesOfSize(uint64_t numberOfMessagesToCreate) const noexcept;

    AWLMessages fetchMessageProducedBySensorCommunicatorExecution(AWLMessages&& messages);
};

class MockSensorCommunicationStrategy final : public SensorCommunication::SensorCommunicationStrategy<AWLMessage> {

public:

    MockSensorCommunicationStrategy() :
            openConnectionCalled(false),
            closeConnectionCalled(false),
            readMessageCalled(false),
            hasToReturnSpecificData(false) {
    }

    void returnThisMessageSequenceWhenReadMessageIsCalled(AWLMessages&& dataToReturn) {
        hasToReturnSpecificData = true;
        this->dataToReturn = std::forward<AWLMessages>(dataToReturn);
    }

    AWLMessage readMessage() override {
        acknowledgeReadMessageHasBeenCalled();
        if (hasToReturnSpecificData && !dataToReturn.empty()) {
            AWLMessage awlMessage = dataToReturn.front();
            dataToReturn.pop_front();
            return awlMessage;
        }

        // WARNING! This mock implementation of readMessage needs to be slowed down because the way gtest works. DO NOT REMOVE.
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        std::this_thread::yield();

        return AWLMessage::returnDefaultData();
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

    bool hasReadMessageBeenCalled() const {
        return readMessageCalled.load();
    }

    void waitUntilReadMessageIsCalled() {
        if (!hasReadMessageBeenCalled()) {
            readMessageCalledAcknowledgement.get_future().wait();
        }
    }

private:

    void acknowledgeReadMessageHasBeenCalled() {
        LockGuard guard(readMessageAckMutex);
        if (!hasReadMessageBeenCalled()) {
            readMessageCalled.store(true);
            readMessageCalledAcknowledgement.set_value(true);
        }
    }

    AtomicFlag openConnectionCalled;
    AtomicFlag closeConnectionCalled;
    AtomicFlag readMessageCalled;

    Mutex readMessageAckMutex;
    mutable BooleanPromise readMessageCalledAcknowledgement;

    bool hasToReturnSpecificData;
    AWLMessages dataToReturn;
};

TEST_F(SensorCommunicatorTest, given__when_start_then_callsOpenConnectionInStrategy) {
    MockSensorCommunicationStrategy mockStrategy;
    AWLCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.start();

    sensorCommunicator.terminateAndJoin();
    auto strategyHasBeenCalled = mockStrategy.hasOpenConnectionBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(SensorCommunicatorTest, given__when_terminateAndJoin_then_callsCloseConnectionInStrategy) {
    MockSensorCommunicationStrategy mockStrategy;
    AWLCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.terminateAndJoin();

    auto strategyHasBeenCalled = mockStrategy.hasCloseConnectionBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(SensorCommunicatorTest, given__when_start_then_callsReadMessageInStrategy) {
    MockSensorCommunicationStrategy mockStrategy;
    AWLCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.start();

    mockStrategy.waitUntilReadMessageIsCalled();
    auto strategyHasBeenCalled = mockStrategy.hasReadMessageBeenCalled();
    sensorCommunicator.terminateAndJoin();
    ASSERT_TRUE(strategyHasBeenCalled);
}

using AWLSink = DataFlow::DataSink<AWLMessage>;

class AWLSinkMock : public AWLSink {

protected:

    using AWLSink::DATA;

public:

    explicit AWLSinkMock(uint8_t numberOfDataToConsume) :
            actualNumberOfDataConsumed(0),
            numberOfDataToConsume(numberOfDataToConsume) {

    }

    void consume(DATA&& data) override {
        ++actualNumberOfDataConsumed;
        if (hasBeenCalledLessOrEqualToTheExpectedNumberOfTimes()) {
            consumedData.push_back(data);
        }
        if (hasBeenCalledExpectedNumberOfTimes()) {
            consumptionReached.set_value(true);
        }
    }

    bool hasBeenCalledLessOrEqualToTheExpectedNumberOfTimes() {
        LockGuard guard(consumptionMutex);
        return actualNumberOfDataConsumed.load() <= numberOfDataToConsume.load();
    }

    bool hasBeenCalledExpectedNumberOfTimes() {
        LockGuard guard(consumptionMutex);
        return actualNumberOfDataConsumed.load() == numberOfDataToConsume.load();
    };

    void waitConsumptionToBeReached() {
        LockGuard guard(goalReachedMutex);
        if (!hasBeenCalledExpectedNumberOfTimes()) {
            consumptionReached.get_future().wait();
        }
    }

    AWLMessages getConsumedData() const noexcept {
        return consumedData;
    }

private:

    AtomicCounter actualNumberOfDataConsumed;
    AtomicCounter numberOfDataToConsume;

    AWLMessages consumedData;

    Mutex consumptionMutex;
    Mutex goalReachedMutex;
    mutable BooleanPromise consumptionReached;
};

using AWLProcessingScheduler = DataFlow::DataProcessingScheduler<AWLMessage, AWLSinkMock, 1>;

AWLMessages SensorCommunicatorTest::fetchMessageProducedBySensorCommunicatorExecution(AWLMessages&& messages) {
    auto numberOfMessagesToProcess = messages.size();
    AWLSinkMock sink(numberOfMessagesToProcess);
    AWLProcessingScheduler scheduler(&sink);

    MockSensorCommunicationStrategy mockStrategy;
    mockStrategy.returnThisMessageSequenceWhenReadMessageIsCalled(std::forward<AWLMessages>(messages));
    AWLCommunicator sensorCommunicator(&mockStrategy);
    sensorCommunicator.linkConsumer(&scheduler);

    sensorCommunicator.start();

    sink.waitConsumptionToBeReached();

    sensorCommunicator.terminateAndJoin();
    scheduler.terminateAndJoin();

    AWLMessages producedMessages = sink.getConsumedData();

    return producedMessages;
}

TEST_F(SensorCommunicatorTest, given_aSequenceOfOneIncomingMessage_when_start_then_willProduceThisData) {
    auto messages = createASequenceOfDifferentMessagesOfSize(1);
    AWLMessage expectedMessage = AWLMessage(messages.front());

    auto resultingMessage = fetchMessageProducedBySensorCommunicatorExecution(std::move(messages)).front();

    ASSERT_EQ(expectedMessage, resultingMessage);
}

TEST_F(SensorCommunicatorTest,
       given_aSequenceOfSeveralIncomingMessages_when_start_then_willProduceTheseMessagesInTheSameOrderTheyAreRead) {
    auto numberOfMessages = 5U;
    auto messages = createASequenceOfDifferentMessagesOfSize(numberOfMessages);
    AWLMessages expectedMessages = messages;

    auto resultedMessages = fetchMessageProducedBySensorCommunicatorExecution(std::move(messages));

    for (auto t = 0; t < numberOfMessages; ++t) {
        ASSERT_EQ(expectedMessages.front(), resultedMessages.front());
        expectedMessages.pop_front();
        resultedMessages.pop_front();
    }
}

AWLMessage SensorCommunicatorTest::createAMessageWithValueOffsetOf(uint8_t dataValueOffset) const noexcept {
    int64_t const arbitraryId = 42 + dataValueOffset;
    uint64_t const arbitraryTimestamp = 101010 + dataValueOffset;
    uint32_t const arbitraryLength = 7 + dataValueOffset;
    AWL::DataArray arbitraryData;

    for (auto k = 0; k < MAXIMUM_NUMBER_OF_DATA_IN_MESSAGE; ++k) {
        arbitraryData.at(k) = static_cast<unsigned char>(k);
    }

    AWLMessage message = AWLMessage::returnDefaultData();
    message.id = arbitraryId;
    message.timestamp = arbitraryTimestamp;
    message.length = arbitraryLength;
    message.data = arbitraryData;
    return message;
}

AWLMessages SensorCommunicatorTest::createASequenceOfDifferentMessagesOfSize(uint64_t numberOfMessagesToCreate) const noexcept {
    AWLMessages messages;
    for (uint8_t offset = 0; offset < numberOfMessagesToCreate; ++offset) {
        auto message = createAMessageWithValueOffsetOf(offset);
        messages.push_back(message);
    }
    return messages;
}

#endif //SPIRITSENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP

