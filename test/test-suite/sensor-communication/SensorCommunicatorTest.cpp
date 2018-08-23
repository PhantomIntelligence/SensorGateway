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
#include <list>

#include "test/utilities/data-model/DataModelFixture.h"
#include "sensor-gateway/sensor-communication/SensorCommunicator.hpp"

using SimpleData = Sensor::Test::Simple::Structures::Message;
using SimpleDataList = std::list<SimpleData>;
using SimpleDataSensorCommunicator = SensorAccessLinkElement::SensorCommunicator<Sensor::Test::Simple::Structures>;
using TestFunctions::DataTestUtil;


class SensorCommunicatorTest : public ::testing::Test {

protected:
    SensorCommunicatorTest() = default;

    virtual ~SensorCommunicatorTest() = default;

    SimpleDataList createASequenceOfDifferentMessagesOfSize(uint64_t numberOfMessagesToCreate) const noexcept;

    SimpleDataList fetchMessageProducedBySensorCommunicatorExecution(SimpleDataList&& messages);
};

class SensorCommunicationStrategy final
        : public SensorCommunication::SensorCommunicationStrategy<Sensor::Test::Simple::Structures> {
protected:

    using super = SensorCommunication::SensorCommunicationStrategy<Sensor::Test::Simple::Structures>;
public:

    SensorCommunicationStrategy() :
            openConnectionCalled(false),
            closeConnectionCalled(false),
            readMessageCalled(false),
            hasToReturnSpecificData(false) {
    }

    ~SensorCommunicationStrategy() noexcept final = default;

    SensorCommunicationStrategy(SensorCommunicationStrategy const& other) = delete;

    SensorCommunicationStrategy(SensorCommunicationStrategy&& other) noexcept = delete;

    SensorCommunicationStrategy& operator=(SensorCommunicationStrategy const& other)& = delete;

    SensorCommunicationStrategy&
    operator=(SensorCommunicationStrategy&& other)& noexcept = delete;

    void returnThisMessageSequenceWhenReadMessageIsCalled(SimpleDataList&& dataToReturn) {
        hasToReturnSpecificData = true;
        this->dataToReturn = std::forward<SimpleDataList>(dataToReturn);
    }

    super::Messages fetchMessages() override {
        acknowledgeReadMessageHasBeenCalled();
        if (hasToReturnSpecificData && !dataToReturn.empty()) {
            SimpleData message = dataToReturn.front();
            dataToReturn.pop_front();
            super::Messages messages = {message};
            return messages;
        }

        // WARNING! This mock implementation of readMessage needs to be slowed down because the way gtest works. DO NOT REMOVE.
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        std::this_thread::yield();

        auto message = super::Message::returnDefaultData();
        super::Messages messages = {message};
        return messages;
    }

    super::RawDataCycles fetchRawDataCycles() override {
        super::RawDataCycles rawDataCycles;
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
    SimpleDataList dataToReturn;
};

TEST_F(SensorCommunicatorTest, given__when_start_then_callsOpenConnectionInStrategy) {
    SensorCommunicationStrategy mockStrategy;
    SimpleDataSensorCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.connect();

    sensorCommunicator.disconnect();
    auto strategyHasBeenCalled = mockStrategy.hasOpenConnectionBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(SensorCommunicatorTest, given__when_terminateAndJoin_then_callsCloseConnectionInStrategy) {
    SensorCommunicationStrategy mockStrategy;
    SimpleDataSensorCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.disconnect();

    auto strategyHasBeenCalled = mockStrategy.hasCloseConnectionBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(SensorCommunicatorTest, given__when_start_then_callsReadMessageInStrategy) {
    SensorCommunicationStrategy mockStrategy;
    SimpleDataSensorCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.connect();

    mockStrategy.waitUntilReadMessageIsCalled();
    auto strategyHasBeenCalled = mockStrategy.hasReadMessageBeenCalled();
    sensorCommunicator.disconnect();
    ASSERT_TRUE(strategyHasBeenCalled);
}

class SimpleDataSinkMock : public DataFlow::DataSink<SimpleData> {

protected:

    using DataFlow::DataSink<SimpleData>::DATA;

public:

    explicit SimpleDataSinkMock(uint8_t numberOfDataToConsume) :
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

    SimpleDataList getConsumedData() const noexcept {
        return consumedData;
    }

private:

    AtomicCounter actualNumberOfDataConsumed;
    AtomicCounter numberOfDataToConsume;

    SimpleDataList consumedData;

    Mutex consumptionMutex;
    Mutex goalReachedMutex;
    mutable BooleanPromise consumptionReached;
};

using AWLProcessingScheduler = DataFlow::DataProcessingScheduler<SimpleData, SimpleDataSinkMock, 1>;

SimpleDataList SensorCommunicatorTest::fetchMessageProducedBySensorCommunicatorExecution(SimpleDataList&& messages) {
    auto numberOfMessagesToProcess = messages.size();
    SimpleDataSinkMock sink(numberOfMessagesToProcess);
    AWLProcessingScheduler scheduler(&sink);

    SensorCommunicationStrategy mockStrategy;
    mockStrategy.returnThisMessageSequenceWhenReadMessageIsCalled(std::forward<SimpleDataList>(messages));
    SimpleDataSensorCommunicator sensorCommunicator(&mockStrategy);
    sensorCommunicator.linkConsumer(&scheduler);

    sensorCommunicator.connect();

    sink.waitConsumptionToBeReached();

    sensorCommunicator.disconnect();
    scheduler.terminateAndJoin();

    SimpleDataList producedMessages = sink.getConsumedData();

    return producedMessages;
}

TEST_F(SensorCommunicatorTest, given_aSequenceOfOneIncomingMessage_when_start_then_willProduceThisData) {
    auto messages = createASequenceOfDifferentMessagesOfSize(1);
    SimpleData expectedMessage = SimpleData(messages.front());

    auto resultingMessage = fetchMessageProducedBySensorCommunicatorExecution(std::move(messages)).front();

    ASSERT_EQ(expectedMessage, resultingMessage);
}

TEST_F(SensorCommunicatorTest,
       given_aSequenceOfSeveralIncomingMessages_when_start_then_willProduceTheseMessagesInTheSameOrderTheyAreRead) {
    auto numberOfMessages = 5U;
    auto messages = createASequenceOfDifferentMessagesOfSize(numberOfMessages);
    SimpleDataList expectedMessages = messages;

    auto resultedMessages = fetchMessageProducedBySensorCommunicatorExecution(std::move(messages));

    for (auto t = 0; t < numberOfMessages; ++t) {
        ASSERT_EQ(expectedMessages.front(), resultedMessages.front());
        expectedMessages.pop_front();
        resultedMessages.pop_front();
    }
}

SimpleDataList
SensorCommunicatorTest::createASequenceOfDifferentMessagesOfSize(uint64_t numberOfMessagesToCreate) const noexcept {
    SimpleDataList messages;
    for (uint8_t offset = 0; offset < numberOfMessagesToCreate; ++offset) {
        auto message = TestFunctions::DataTestUtil::createRandomSimpleData();
        messages.push_back(message);
    }
    return messages;
}

#endif //SENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP

