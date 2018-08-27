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

#include "sensor-gateway/application/SensorAccessLink.hpp"
#include "test/utilities/data-model/DataModelFixture.h"

using TestFunctions::DataTestUtil;
using SensorAccessLink = SensorGateway::SensorAccessLink<Sensor::Test::Simple::Structures, Sensor::Test::Simple::Structures>;

class SensorAccessLinkTest : public ::testing::Test {

protected:
    SensorAccessLinkTest() = default;

    virtual ~SensorAccessLinkTest() = default;

public:
    std::string const FAKE_SERVER_ADDRESS = "5150 Elm's Way";

};

namespace SensorAccessLinkTestMock {
    using SimpleSensorCommunicationStrategy = SensorCommunication::SensorCommunicationStrategy<Sensor::Test::Simple::Structures>;

    class MockSensorCommunicationStrategy final : public SimpleSensorCommunicationStrategy {

    protected:

        using super = SensorCommunicationStrategy<Sensor::Test::Simple::Structures>;
        using MessagesList = std::list<super::Messages>;
        using RawDataCyclesList = std::list<super::RawDataCycles>;

    public:

        explicit MockSensorCommunicationStrategy(uint8_t minimumNumberOfMessageToCreate) :
                messagePromiseFulfilled(false),
                rawDataPromiseFulfilled(false),
                connectCalled(false),
                disconnectCalled(false),
                numberOfMessageCreated(0),
                numberOfRawDataCyclesCreated(0),
                minimumNumberOfMessagesToCreate(minimumNumberOfMessageToCreate),
                minimumNumberOfRawDataCyclesToCreate(minimumNumberOfMessageToCreate) {
        }

        void openConnection() override {
            connectCalled.store(true);
        }

        bool hasOpenConnectionBeenCalled() const {
            return connectCalled.load();
        }

        void closeConnection() override {
            disconnectCalled.store(true);
        }

        bool hasCloseConnectionBeenCalled() const {
            return disconnectCalled.load();
        }

        super::Messages fetchMessages() override {
            super::Messages createdMessages;

            for (auto i = 0u; i < Sensor::Test::Simple::Structures::MAX_NUMBER_OF_BULK_FETCHABLE_MESSAGES; ++i) {
                auto createdData = DataTestUtil::createRandomSimpleMessage();
                createdMessages[i] = createdData;
            }
            auto copy = super::Messages(createdMessages);
            createdMessagesCopies.push_back(copy);

            ++numberOfMessageCreated;
            if (hasCreatedMinimumNumberOfMessages() && !messagePromiseFulfilled.load()) {
                messagePromiseFulfilled.store(true);
                minimumNumberOfMessageCreated.set_value(true);
            }

            // WARNING! This mock implementation of readMessage needs to be slowed down because the way gtest works. DO NOT REMOVE.
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            std::this_thread::yield();

            return createdMessages;
        }

        super::RawDataCycles fetchRawDataCycles() override {
            super::RawDataCycles createdRawDataCycles;

            for (auto i = 0u; i < Sensor::Test::Simple::Structures::MAX_NUMBER_OF_BULK_FETCHABLE_RAW_DATA_CYCLES; ++i) {
                auto createdRawData = DataTestUtil::createRandomSimpleRawData();
                createdRawDataCycles[i] = createdRawData;
            }
            auto copy = super::RawDataCycles(createdRawDataCycles);
            createdRawDataCyclesCopies.push_back(copy);

            ++numberOfRawDataCyclesCreated;
            if (hasCreatedMinimumNumberOfRawDataCycles() && !rawDataPromiseFulfilled.load()) {
                rawDataPromiseFulfilled.store(true);
                minimumNumberOfRawDataCycleCreated.set_value(true);
            }

            // WARNING! This mock implementation of readMessage needs to be slowed down because the way gtest works. DO NOT REMOVE.
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            std::this_thread::yield();
            return createdRawDataCycles;
        }

        void waitUntilFetchMessagesHasBeenCalledEnough() {
            if (!hasCreatedMinimumNumberOfMessages()) {
                minimumNumberOfMessageCreated.get_future().wait();
            }
        }

        void waitUntilFetchRawDataCyclesHasBeenCalledEnough() {
            if (!hasCreatedMinimumNumberOfRawDataCycles()) {
                minimumNumberOfRawDataCycleCreated.get_future().wait();
            }
        }

        MessagesList const& getCreatedMessageCopies() const {
            return createdMessagesCopies;
        }

        RawDataCyclesList const& getCreatedRawDataCopies() const {
            return createdRawDataCyclesCopies;
        }

    private:

        bool hasCreatedMinimumNumberOfMessages() {
            LockGuard guard(numberOfMessageCreatedVerificationMutex);
            return numberOfMessageCreated.load() >= minimumNumberOfMessagesToCreate.load();
        }

        bool hasCreatedMinimumNumberOfRawDataCycles() {
            LockGuard guard(numberOfRawDataCyclesCreatedVerificationMutex);
            return numberOfRawDataCyclesCreated.load() >= minimumNumberOfRawDataCyclesToCreate.load();
        }

        AtomicCounter numberOfMessageCreated;
        AtomicCounter numberOfRawDataCyclesCreated;
        AtomicCounter minimumNumberOfMessagesToCreate;
        AtomicCounter minimumNumberOfRawDataCyclesToCreate;

        AtomicFlag messagePromiseFulfilled;
        AtomicFlag rawDataPromiseFulfilled;
        AtomicFlag connectCalled;
        AtomicFlag disconnectCalled;

        Mutex numberOfMessageCreatedVerificationMutex;
        Mutex numberOfRawDataCyclesCreatedVerificationMutex;

        mutable BooleanPromise minimumNumberOfMessageCreated;
        mutable BooleanPromise minimumNumberOfRawDataCycleCreated;

        MessagesList createdMessagesCopies;
        RawDataCyclesList createdRawDataCyclesCopies;
    };

    using SimpleTranslationStrategy = DataTranslation::DataTranslationStrategy<Sensor::Test::Simple::Structures, Sensor::Test::Simple::Structures>;

    class MockTranslationStrategy final : public SimpleTranslationStrategy {
    protected:
        using super = SimpleTranslationStrategy;
        using super::SensorMessage;
        using super::SensorRawData;

        using super::MessageSource;
        using super::RawDataSource;

    public:
        MockTranslationStrategy() = default;

        void translateMessage(super::SensorMessage&& sensorMessage) override {
            sensorMessage.inverseContent();
            super::MessageSource::produce(std::move(sensorMessage));
        }

        void translateRawData(super::SensorRawData&& sensorRawData) override {
        }
    };

    using SimpleServerCommunicationStrategy = ServerCommunication::ServerCommunicationStrategy<DataModel::SimpleMessage>;

    class MockServerCommunicationStrategy final : public SimpleServerCommunicationStrategy {
    protected:
        using SimpleServerCommunicationStrategy::MESSAGE;

    public:
        MockServerCommunicationStrategy() :
                connectCalled(false),
                disconnectCalled(false) {}

        ~MockServerCommunicationStrategy() noexcept override = default;

        void openConnection(std::string const& serverAddress) override {
            connectCalled.store(true);
        }

        bool hasOpenConnectionBeenCalled() const {
            return connectCalled.load();
        }

        void closeConnection() override {
            disconnectCalled.store(true);
        }

        bool hasCloseConnectionBeenCalled() const {
            return disconnectCalled.load();
        }

        void sendMessage(MESSAGE&& message) override {
            receivedData.push_back(message);
        }

        std::list<DataModel::SimpleMessage> const& getReceivedData() const {
            return receivedData;
        }

    private:
        std::list<DataModel::SimpleMessage> receivedData;
        AtomicFlag connectCalled;
        AtomicFlag disconnectCalled;
    };
}

TEST_F(SensorAccessLinkTest,
       given_strategies_when_connect_then_bothSensorAndServerStrategiesReceiveOpenConnectionCall) {
    uint8_t numberOfDataToProcess = 0;
    SensorAccessLinkTestMock::MockSensorCommunicationStrategy mockSensorCommunicationStrategy(numberOfDataToProcess);
    SensorAccessLinkTestMock::MockTranslationStrategy mockTranslationStrategy;
    SensorAccessLinkTestMock::MockServerCommunicationStrategy mockServerCommunicationStrategy;
    SensorAccessLink sensorAccessLink(&mockServerCommunicationStrategy,
                                      &mockTranslationStrategy,
                                      &mockSensorCommunicationStrategy);

    sensorAccessLink.start(FAKE_SERVER_ADDRESS);

    auto sensorOpenConnectionHasBeenCalled = mockSensorCommunicationStrategy.hasOpenConnectionBeenCalled();
    auto serverOpenConnectionHasBeenCalled = mockServerCommunicationStrategy.hasOpenConnectionBeenCalled();

    sensorAccessLink.terminateAndJoin();

    ASSERT_TRUE(serverOpenConnectionHasBeenCalled);
    ASSERT_TRUE(sensorOpenConnectionHasBeenCalled);
}

TEST_F(SensorAccessLinkTest,
       given_strategies_when_disconnect_then_bothSensorAndServerStrategiesReceiveCloseConnectionCall) {
    uint8_t numberOfDataToProcess = 0;
    SensorAccessLinkTestMock::MockSensorCommunicationStrategy mockSensorCommunicationStrategy(numberOfDataToProcess);
    SensorAccessLinkTestMock::MockTranslationStrategy mockTranslationStrategy;
    SensorAccessLinkTestMock::MockServerCommunicationStrategy mockServerCommunicationStrategy;
    SensorAccessLink sensorAccessLink(&mockServerCommunicationStrategy,
                                      &mockTranslationStrategy,
                                      &mockSensorCommunicationStrategy);

    sensorAccessLink.start(FAKE_SERVER_ADDRESS);
    std::this_thread::yield();
    sensorAccessLink.terminateAndJoin();

    auto sensorCloseConnectionHasBeenCalled = mockSensorCommunicationStrategy.hasCloseConnectionBeenCalled();
    auto serverCloseConnectionHasBeenCalled = mockServerCommunicationStrategy.hasCloseConnectionBeenCalled();

    ASSERT_TRUE(serverCloseConnectionHasBeenCalled);
    ASSERT_TRUE(sensorCloseConnectionHasBeenCalled);
}
/**
 * Medium test
 */
TEST_F(SensorAccessLinkTest,
       given_aNumberOfDataCreatedByTheSensorCommunicationStrategy_when_executing_then_theSameNumberOfDataEndsUpInTheServerCommunicationStrategy) {
    uint8_t numberOfDataToProcess = 42;
    SensorAccessLinkTestMock::MockSensorCommunicationStrategy mockSensorCommunicationStrategy(numberOfDataToProcess);
    SensorAccessLinkTestMock::MockTranslationStrategy mockTranslationStrategy;
    SensorAccessLinkTestMock::MockServerCommunicationStrategy mockServerCommunicationStrategy;
    SensorAccessLink sensorAccessLink(&mockServerCommunicationStrategy,
                                      &mockTranslationStrategy,
                                      &mockSensorCommunicationStrategy);

    sensorAccessLink.start(FAKE_SERVER_ADDRESS);

    mockSensorCommunicationStrategy.waitUntilFetchMessagesHasBeenCalledEnough();
    mockSensorCommunicationStrategy.waitUntilFetchRawDataCyclesHasBeenCalledEnough();

    sensorAccessLink.terminateAndJoin();

    auto createdDataList = mockSensorCommunicationStrategy.getCreatedMessageCopies();
    auto receivedDataList = mockServerCommunicationStrategy.getReceivedData();

    ASSERT_EQ(createdDataList.size(), receivedDataList.size());
}

/**
 * Medium Test
 */
TEST_F(SensorAccessLinkTest,
       given_dataCreatedByTheSensorCommunicationStrategy_when_executing_then_dataGoesThroughTranslationStrategyBeforeEndingInTheServerCommunicationStrategy) {
    uint8_t numberOfDataToProcess = 42;
    SensorAccessLinkTestMock::MockSensorCommunicationStrategy mockSensorCommunicationStrategy(numberOfDataToProcess);
    SensorAccessLinkTestMock::MockTranslationStrategy mockTranslationStrategy;
    SensorAccessLinkTestMock::MockServerCommunicationStrategy mockServerCommunicationStrategy;
    SensorAccessLink sensorAccessLink(&mockServerCommunicationStrategy,
                                      &mockTranslationStrategy,
                                      &mockSensorCommunicationStrategy);


    sensorAccessLink.start(FAKE_SERVER_ADDRESS);

    mockSensorCommunicationStrategy.waitUntilFetchMessagesHasBeenCalledEnough();
    mockSensorCommunicationStrategy.waitUntilFetchRawDataCyclesHasBeenCalledEnough();

    sensorAccessLink.terminateAndJoin();

    auto createdDataList = mockSensorCommunicationStrategy.getCreatedMessageCopies();
    std::vector<SensorAccessLinkTestMock::SimpleSensorCommunicationStrategy::Message> flattenMessages;
    auto const numberOfBulkMessages = createdDataList.size();
    auto const numberOfMessagesPerBulk = Sensor::Test::Simple::Structures::MAX_NUMBER_OF_BULK_FETCHABLE_MESSAGES;
    auto const totalNumberOfMessages = numberOfBulkMessages * numberOfMessagesPerBulk;
    flattenMessages.reserve(totalNumberOfMessages);
    for (uint32_t i = 0; i < numberOfBulkMessages; ++i) {
        auto currentMessages = createdDataList.front();
        createdDataList.pop_front();
        for (uint32_t j = 0; j < numberOfMessagesPerBulk; ++j) {
            flattenMessages.push_back(currentMessages.at(j));
        }
    }
    auto receivedDataList = mockServerCommunicationStrategy.getReceivedData();

    bool dataHasPassedThroughCorrectly = true;
    for (uint32_t messageIndex = 0;
         messageIndex < totalNumberOfMessages && dataHasPassedThroughCorrectly; ++messageIndex) {
        auto createdMessage = flattenMessages.at(messageIndex);
        auto receivedMessage = receivedDataList.front();
        receivedDataList.pop_front();
        dataHasPassedThroughCorrectly = createdMessage.isTheInverseOf(receivedMessage);
    }

    ASSERT_TRUE(dataHasPassedThroughCorrectly);
}

#endif //SPIRITSENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP


