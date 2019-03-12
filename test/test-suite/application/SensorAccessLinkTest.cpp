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

#ifndef SPIRITSENSORGATEWAY_SENSORACCESSLINKTEST_CPP
#define SPIRITSENSORGATEWAY_SENSORACCESSLINKTEST_CPP

#include <gtest/gtest.h>

#include "sensor-gateway/application/SensorAccessLink.hpp"
#include "test/utilities/assertion/SensorMessageAssertion.hpp"
#include "test/utilities/mock/DataTranslationStrategyMock.hpp"
#include "test/utilities/mock/CatchingServerCommunicationStrategyMock.hpp"
#include "test/utilities/mock/ErrorThrowingServerCommunicationStrategyMock.hpp"
#include "test/utilities/mock/ErrorThrowingDataTranslationStrategyMock.hpp"
#include "test/utilities/mock/ErrorThrowingSensorCommunicationStrategyMock.hpp"


class SensorAccessLinkTest : public ::testing::Test {

public:

    using DataStructures = Sensor::Test::RealisticImplementation::Structures;
    using SensorAccessLinkFactory = SensorGateway::SensorAccessLinkFactory<DataStructures>;
    using SensorAccessLink = typename SensorAccessLinkFactory::AccessLink;
    using GatewayStructures = typename SensorAccessLinkFactory::GatewayStructures;
    using DataTranslationStrategyMock = Mock::DataTranslationStrategyMock<DataStructures, GatewayStructures>;
    using ErrorThrowingSensorCommunicationStrategyMock = Mock::ErrorThrowingSensorCommunicationStrategyMock<DataStructures>;

protected:

    SensorAccessLinkTest() = default;

    virtual ~SensorAccessLinkTest() = default;

public:

    std::string const FAKE_SERVER_ADDRESS = "5150 Elm's Way";

};

namespace SensorAccessLinkTestMock {

    using TestFunctions::DataTestUtil;
    using DataStructures = Sensor::Test::RealisticImplementation::Structures;
    using GatewayStructures = typename SensorGateway::SensorAccessLinkFactory<DataStructures>::GatewayStructures;
    using RealisticSensorCommunicationStrategy = SensorCommunication::SensorCommunicationStrategy<DataStructures>;

    class MockSensorCommunicationStrategy final : public RealisticSensorCommunicationStrategy {

    protected:

        using super = SensorCommunicationStrategy<Sensor::Test::RealisticImplementation::Structures>;
        using MessagesList = std::list<super::Messages>;
        using RawDataCyclesList = std::list<super::RawDataCycles>;

    public:

        explicit MockSensorCommunicationStrategy(uint8_t minimumNumberOfMessageToCreate,
                                                 uint8_t minimumNumberOfRawDataToCreate) :
                messagePromiseFulfilled(false),
                rawDataPromiseFulfilled(false),
                openConnectionCalled(false),
                closeConnectionCalled(false),
                numberOfMessageCreated(0),
                numberOfRawDataCyclesCreated(0),
                minimumNumberOfMessagesToCreate(minimumNumberOfMessageToCreate),
                minimumNumberOfRawDataCyclesToCreate(minimumNumberOfRawDataToCreate) {
        }

        void openConnection() override {
            openConnectionCalled.store(true);
        }

        bool hasOpenConnectionBeenCalled() const {
            return openConnectionCalled.load();
        }

        void closeConnection() override {
            closeConnectionCalled.store(true);
        }

        bool hasCloseConnectionBeenCalled() const {
            return closeConnectionCalled.load();
        }

        super::Messages fetchMessages() override {
            super::Messages createdMessages;

            for (auto i = 0u;
                 i < Sensor::Test::RealisticImplementation::Structures::MAX_NUMBER_OF_BULK_FETCHABLE_MESSAGES; ++i) {
                auto createdData = DataTestUtil::createRandomRealisticMessageWithEmptyTimestamps();
                auto copy = super::Messages(createdMessages);
                createdMessages[i] = createdData;
                createdMessagesCopies.push_back(copy);
                ++numberOfMessageCreated;
            }

            if (hasCreatedMinimumNumberOfMessages() && !messagePromiseFulfilled.load()) {
                messagePromiseFulfilled.store(true);
                minimumNumberOfMessageCreated.set_value(true);
            }

            // WARNING! This mock implementation of readMessage needs to be slowed down because the way gtest works. DO NOT REMOVE.
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
           yield();

            return createdMessages;
        }

        super::RawDataCycles fetchRawDataCycles() override {
            super::RawDataCycles createdRawDataCycles;

            for (auto i = 0u; i < Sensor::Test::RealisticImplementation::Structures::MAX_NUMBER_OF_BULK_FETCHABLE_RAW_DATA_CYCLES; ++i) {
                auto createdRawData = DataTestUtil::createRandomRealisticRawData();
                auto copy = super::RawDataCycles(createdRawDataCycles);
                createdRawDataCycles[i] = createdRawData;
                createdRawDataCyclesCopies.push_back(copy);
                ++numberOfRawDataCyclesCreated;
            }

            if (hasCreatedMinimumNumberOfRawDataCycles() && !rawDataPromiseFulfilled.load()) {
                rawDataPromiseFulfilled.store(true);
                minimumNumberOfRawDataCycleCreated.set_value(true);
            }

            // WARNING! This mock implementation of readMessage needs to be slowed down because the way gtest works. DO NOT REMOVE.
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            yield();
            return createdRawDataCycles;
        }

        void sendRequest(typename super::Request&& request) override {
            // TODO : Complete with test/utilities/mock/Function.hpp
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
        AtomicFlag openConnectionCalled;
        AtomicFlag closeConnectionCalled;

        Mutex numberOfMessageCreatedVerificationMutex;
        Mutex numberOfRawDataCyclesCreatedVerificationMutex;

        mutable BooleanPromise minimumNumberOfMessageCreated;
        mutable BooleanPromise minimumNumberOfRawDataCycleCreated;

        MessagesList createdMessagesCopies;
        RawDataCyclesList createdRawDataCyclesCopies;
    };

    using TranslationStrategy = DataTranslation::DataTranslationStrategy<DataStructures, GatewayStructures>;

    class MockTranslationStrategy final : public TranslationStrategy {

    protected:

        using super = TranslationStrategy;
        using SensorMessage = typename super::SensorMessage;
        using SensorRawData = typename super::SensorRawData;

        using ServerMessage = typename super::ServerMessage;
        using ServerRawData = typename super::ServerRawData;

        using super::MessageSource;
        using super::RawDataSource;

        using TranslatedMessages = std::list<ServerMessage>;
        using TranslatedRawData = std::list<ServerRawData>;

    public:

        MockTranslationStrategy() = default;

        void translateMessage(SensorMessage&& sensorMessage) override {
            sensorMessage.messageId *= 2;
            auto newTranslatedMessage = ServerMessage(sensorMessage.messageId, sensorMessage.sensorId,
                                                      *sensorMessage.getPixels());
            auto copy = ServerMessage(newTranslatedMessage);
            translatedMessages.push_back(std::move(copy));
            super::MessageSource::produce(std::move(newTranslatedMessage));
        }

        void translateRawData(SensorRawData&& sensorRawData) override {
            auto const numberOfData = sensorRawData.content.max_size();
            for (auto index = 0u; index < numberOfData; ++index) {
                sensorRawData.content[index] *= 2;
            }
            auto newTranslatedRawData = ServerRawData(sensorRawData.content);
            auto copy = ServerRawData(newTranslatedRawData);
            translatedRawData.push_back(std::move(copy));
            super::RawDataSource::produce(std::move(newTranslatedRawData));
        }

        SensorMessage
        translateControlMessageToSensorMessageRequest(SensorControlMessage&& sensorControlMessage) override {
            return SensorMessage::returnDefaultData();
        }

        SensorControlMessage translateSensorMessageToControlMessageResult(SensorMessage&& sensorMessage) override {
            return SensorControlMessage::returnDefaultData();
        }

        TranslatedMessages const& getTranslatedMessages() const noexcept {
            return translatedMessages;
        }

        TranslatedRawData const& getTranslatedRawData() const noexcept {
            return translatedRawData;
        }

    private:

        TranslatedMessages translatedMessages;
        TranslatedRawData translatedRawData;
    };

    using MockServerCommunicationStrategy = Mock::CatchingServerCommunicationStrategyMock<GatewayStructures>;
}

TEST_F(SensorAccessLinkTest,
       given_strategies_when_start_then_bothSensorAndServerStrategiesReceiveOpenConnectionCall) {
    uint8_t numberOfDataToProcess = 0;
    SensorAccessLinkTestMock::MockSensorCommunicationStrategy mockSensorCommunicationStrategy(numberOfDataToProcess,
                                                                                              numberOfDataToProcess);
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
       given_strategies_when_terminateAndJoin_then_bothSensorAndServerStrategiesReceiveCloseConnectionCall) {
    uint8_t numberOfDataToProcess = 0;
    SensorAccessLinkTestMock::MockSensorCommunicationStrategy mockSensorCommunicationStrategy(
            numberOfDataToProcess, numberOfDataToProcess);
    SensorAccessLinkTestMock::MockTranslationStrategy mockTranslationStrategy;
    SensorAccessLinkTestMock::MockServerCommunicationStrategy mockServerCommunicationStrategy;
    SensorAccessLink sensorAccessLink(&mockServerCommunicationStrategy,
                                      &mockTranslationStrategy,
                                      &mockSensorCommunicationStrategy);

    sensorAccessLink.start(FAKE_SERVER_ADDRESS);
    yield();
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
       given_aSensorOutputingANumberOfMessages_when_executing_then_theSameNumberOfMessagesEndsUpInTheServerCommunicationStrategy) {
    uint8_t numberOfMessagesToProcess = 42;
    uint8_t numberOfRawDataToProcess = 0;
    SensorAccessLinkTestMock::MockSensorCommunicationStrategy mockSensorCommunicationStrategy(
            numberOfMessagesToProcess, numberOfRawDataToProcess);
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
    auto receivedDataList = mockServerCommunicationStrategy.getReceivedMessages();

    ASSERT_EQ(createdDataList.size(), receivedDataList.size());
}

/**
 * Medium test
 */
TEST_F(SensorAccessLinkTest,
       given_aSensorOutputingANumberOfRawData_when_executing_then_theSameNumberOfRawDataCyclesEndsUpInTheServerCommunicationStrategy) {
    uint8_t numberOfMessagesToProcess = 0;
    uint8_t numberOfRawDataToProcess = 42;
    SensorAccessLinkTestMock::MockSensorCommunicationStrategy mockSensorCommunicationStrategy(
            numberOfMessagesToProcess, numberOfRawDataToProcess);
    SensorAccessLinkTestMock::MockTranslationStrategy mockTranslationStrategy;
    SensorAccessLinkTestMock::MockServerCommunicationStrategy mockServerCommunicationStrategy;
    SensorAccessLink sensorAccessLink(&mockServerCommunicationStrategy,
                                      &mockTranslationStrategy,
                                      &mockSensorCommunicationStrategy);

    sensorAccessLink.start(FAKE_SERVER_ADDRESS);

    mockSensorCommunicationStrategy.waitUntilFetchMessagesHasBeenCalledEnough();
    mockSensorCommunicationStrategy.waitUntilFetchRawDataCyclesHasBeenCalledEnough();

    sensorAccessLink.terminateAndJoin();

    auto createdDataList = mockSensorCommunicationStrategy.getCreatedRawDataCopies();
    auto receivedDataList = mockServerCommunicationStrategy.getReceivedRawData();

    ASSERT_EQ(createdDataList.size(), receivedDataList.size());
}

/**
 * Medium Test
 */
TEST_F(SensorAccessLinkTest,
       given_messagesCreatedByTheSensorCommunicationStrategy_when_executing_then_dataGoesThroughTranslationStrategyBeforeEndingInTheServerCommunicationStrategy) {
    uint8_t numberOfMessagesToProcess = 42;
    uint8_t numberOfRawDataToProcess = 0;
    SensorAccessLinkTestMock::MockSensorCommunicationStrategy mockSensorCommunicationStrategy(
            numberOfMessagesToProcess, numberOfRawDataToProcess);
    SensorAccessLinkTestMock::MockTranslationStrategy mockTranslationStrategy;
    SensorAccessLinkTestMock::MockServerCommunicationStrategy mockServerCommunicationStrategy;
    SensorAccessLink sensorAccessLink(&mockServerCommunicationStrategy,
                                      &mockTranslationStrategy,
                                      &mockSensorCommunicationStrategy);


    sensorAccessLink.start(FAKE_SERVER_ADDRESS);

    mockSensorCommunicationStrategy.waitUntilFetchMessagesHasBeenCalledEnough();
    mockSensorCommunicationStrategy.waitUntilFetchRawDataCyclesHasBeenCalledEnough();

    sensorAccessLink.terminateAndJoin();

    auto translatedDataList = mockTranslationStrategy.getTranslatedMessages();
    auto receivedDataList = mockServerCommunicationStrategy.getReceivedMessages();
    auto totalNumberOfMessages = translatedDataList.size();

    bool dataHasPassedThroughCorrectly = true;
    for (auto messageIndex = 0u; messageIndex < totalNumberOfMessages; ++messageIndex) {
        auto expected = translatedDataList.front();
        auto actual = receivedDataList.front();
        ASSERT_EQ(expected, actual);
        translatedDataList.pop_front();
        receivedDataList.pop_front();
    }
}

/**
 * Medium Test
 */
TEST_F(SensorAccessLinkTest,
       given_rawDataCyclesCreatedByTheSensorCommunicationStrategy_when_executing_then_dataGoesThroughTranslationStrategyBeforeEndingInTheServerCommunicationStrategy) {
    uint8_t numberOfMessagesToProcess = 0;
    uint8_t numberOfRawDataToProcess = 42;
    SensorAccessLinkTestMock::MockSensorCommunicationStrategy mockSensorCommunicationStrategy(
            numberOfMessagesToProcess, numberOfRawDataToProcess);
    SensorAccessLinkTestMock::MockTranslationStrategy mockTranslationStrategy;
    SensorAccessLinkTestMock::MockServerCommunicationStrategy mockServerCommunicationStrategy;
    SensorAccessLink sensorAccessLink(&mockServerCommunicationStrategy,
                                      &mockTranslationStrategy,
                                      &mockSensorCommunicationStrategy);

    sensorAccessLink.start(FAKE_SERVER_ADDRESS);

    mockSensorCommunicationStrategy.waitUntilFetchMessagesHasBeenCalledEnough();
    mockSensorCommunicationStrategy.waitUntilFetchRawDataCyclesHasBeenCalledEnough();

    sensorAccessLink.terminateAndJoin();

    auto translatedDataList = mockTranslationStrategy.getTranslatedRawData();
    auto receivedRawDataList = mockServerCommunicationStrategy.getReceivedRawData();
    auto totalNumberOfMessages = translatedDataList.size();

    bool dataHasPassedThroughCorrectly = true;
    for (auto messageIndex = 0u; messageIndex < totalNumberOfMessages; ++messageIndex) {
        auto expected = translatedDataList.front();
        auto actual = receivedRawDataList.front();
        ASSERT_EQ(expected, actual);
        translatedDataList.pop_front();
        receivedRawDataList.pop_front();
    }
}


// TODO: add *MEDIUM* test to check the integration of the SensorAccessLinkErrorHandler;

#endif //SPIRITSENSORGATEWAY_SENSORACCESSLINKTEST_CPP


