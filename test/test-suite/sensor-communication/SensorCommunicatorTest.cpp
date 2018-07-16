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
#include <queue>

#include "spirit-sensor-gateway/sensor-communication/SensorCommunicator.hpp"
#include "data-model/DataModelFixture.h"

using DataFlow::AWLMessage;
using AWLData = std::queue<AWLMessage>;
using AWLCommunicator = SensorAccessLinkElement::SensorCommunicator<AWLMessage>;
using TestFunctions::DataTestUtil;

/**
 * @brief Test Fixture meant to ensure correct behavior of SensorCommunicator.
 * @note A RingBuffer is used to implement the different tested functions
 */
class SensorCommunicatorTest : public ::testing::Test {

protected:
    SensorCommunicatorTest() = default;

    virtual ~SensorCommunicatorTest() = default;

    AWLMessage givenOneMessage() const noexcept {
        return givenOneMessage(0);
    }

    AWLMessage givenOneMessage(uint8_t offsetForDataDifference) const noexcept;

    AWLData givenANumberOfMessages(uint8_t numberOfMessagesToCreate) const noexcept;

    void given_aNumberOfMessage_when_start_then_produceTheCorrectNumberOfMessageInTheCorrectOrder(uint8_t number);
};

class MockSensorCommunicationStrategy final : public SensorCommunication::CommunicationProtocolStrategy<AWLMessage> {

public:

    MockSensorCommunicationStrategy() :
            openConnectionCalled(false),
            closeConnectionCalled(false),
            readMessageCalled(false),
            hasToReturnSpecificData(false) {
    }

    void whenCalledReadMessageThenWillReturnTheseMessages(AWLData dataToReturn) {
        hasToReturnSpecificData = true;
        this->dataToReturn = dataToReturn;
    }

    AWLMessage readMessage() override {
        acknowledgeReadMessageHasBeenCalled();
        if (hasToReturnSpecificData && !dataToReturn.empty()) {
            AWLMessage awlMessage = dataToReturn.front();
            dataToReturn.pop();
            return awlMessage;
        }
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

    void waitUntillReadMessageIsCalled() {
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
    AWLData dataToReturn;
};

TEST_F(SensorCommunicatorTest,
       given_aSensorCommunicationStrategy_when_start_then_callsOpenConnectionInStrategy) {
    MockSensorCommunicationStrategy mockStrategy;
    AWLCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.start();

    sensorCommunicator.terminateAndJoin();
    auto strategyHasBeenCalled = mockStrategy.hasOpenConnectionBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(SensorCommunicatorTest,
       given_aSensorCommunicationStrategy_when_terminateAndJoin_then_callsCloseConnectionInStrategy) {
    MockSensorCommunicationStrategy mockStrategy;
    AWLCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.terminateAndJoin();

    auto strategyHasBeenCalled = mockStrategy.hasCloseConnectionBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(SensorCommunicatorTest, given_aSensorCommunicationStrategy_when_start_then_callsReadMessageInStrategy) {
    MockSensorCommunicationStrategy mockStrategy;
    AWLCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.start();

    mockStrategy.waitUntillReadMessageIsCalled();
    auto strategyHasBeenCalled = mockStrategy.hasReadMessageBeenCalled();
    sensorCommunicator.terminateAndJoin();
    ASSERT_TRUE(strategyHasBeenCalled);
}

using AWLSink = DataFlow::DataSink<AWLMessage>;

class AWLSinkMock : public AWLSink {

protected:

    using AWLSink::DATA;

public:

    AWLSinkMock(uint8_t numberOfDataToConsumeGoal) :
            actualNumberOfDataConsumed(0),
            numberOfDataToConsumeGoal(numberOfDataToConsumeGoal) {

    }

    void consume(DATA&& data) override {
        ++actualNumberOfDataConsumed;
        if (hasBeenCalledLessOrEqualToTheExpectedNumberOfTimes()) {
            consumedData.push(data);
        }
        if (hasBeenCalledExpectedNumberOfTimes()) {
            consumptionGoalReached.set_value(true);
        }
    }

    bool hasBeenCalledLessOrEqualToTheExpectedNumberOfTimes() const {
        return actualNumberOfDataConsumed.load() <= numberOfDataToConsumeGoal;
    }

    bool hasBeenCalledExpectedNumberOfTimes() const {
        return actualNumberOfDataConsumed.load() == numberOfDataToConsumeGoal;
    };

    void waitConsumptionGoalToBeReached() {
        LockGuard guard(goalReachedMutex);
        if (!hasBeenCalledExpectedNumberOfTimes()) {
            consumptionGoalReached.get_future().wait();
        }
    }

    AWLData getConsumedData() const noexcept {
        return consumedData;
    }

private:

    AtomicCounter actualNumberOfDataConsumed;
    AtomicCounter numberOfDataToConsumeGoal;

    AWLData consumedData;

    Mutex goalReachedMutex;
    mutable BooleanPromise consumptionGoalReached;
};

using AWLProcessingScheduler = DataFlow::DataProcessingScheduler<AWLMessage, AWLSinkMock, 1>;


void SensorCommunicatorTest::given_aNumberOfMessage_when_start_then_produceTheCorrectNumberOfMessageInTheCorrectOrder(
        uint8_t number) {
    auto messages = givenANumberOfMessages(number);
    auto expectedMessages = messages;
    MockSensorCommunicationStrategy mockStrategy;
    mockStrategy.whenCalledReadMessageThenWillReturnTheseMessages(messages);
    
    AWLSinkMock sink(number);
    AWLProcessingScheduler scheduler(&sink);

    AWLCommunicator sensorCommunicator(&mockStrategy);
    sensorCommunicator.linkConsumer(&scheduler);
    sensorCommunicator.start();

    sink.waitConsumptionGoalToBeReached();

    sensorCommunicator.terminateAndJoin();
    scheduler.terminateAndJoin();

    auto producedMessages = sink.getConsumedData();
    ASSERT_EQ(expectedMessages, producedMessages);
}

TEST_F(SensorCommunicatorTest, given_oneMessage_when_start_then_willProduceThisData) {
    auto numberOfMessage = 1;
    given_aNumberOfMessage_when_start_then_produceTheCorrectNumberOfMessageInTheCorrectOrder(numberOfMessage);
}

TEST_F(SensorCommunicatorTest, given_severalMessage_when_start_then_willProduceTheseDataInTheSameOrderItConsumedIt) {
    auto numberOfMessage = 5;
    given_aNumberOfMessage_when_start_then_produceTheCorrectNumberOfMessageInTheCorrectOrder(numberOfMessage);
}

AWLMessage SensorCommunicatorTest::givenOneMessage(uint8_t offsetForDataDifference) const noexcept {
    int64_t const ARBITRARY_ID = 42 + offsetForDataDifference;
    uint64_t const ARBITRARY_TIMESTAMP = 101010 + offsetForDataDifference;
    uint32_t const ARBITRARY_LENGTH = 7 + offsetForDataDifference;
    AWL::DataArray ARBITRARY_DATA;
    for(auto k = 0; k < MAXIMUM_NUMBER_OF_DATA_IN_MESSAGE; ++k) {
        ARBITRARY_DATA.at(k) = static_cast<unsigned char>(k);
    }

    AWLMessage message = AWLMessage::returnDefaultData();
    message.id = ARBITRARY_ID;
    message.timestamp = ARBITRARY_TIMESTAMP;
    message.length = ARBITRARY_LENGTH;
    message.data = ARBITRARY_DATA;
    return message;
}

AWLData SensorCommunicatorTest::givenANumberOfMessages(uint8_t numberOfMessagesToCreate) const noexcept {
    AWLData messages;
    for (uint8_t offset = 0; offset < numberOfMessagesToCreate; ++offset) {
        auto message = givenOneMessage(offset);
        messages.push(message);
    }
    return messages;
}

#endif //SPIRITSENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP

