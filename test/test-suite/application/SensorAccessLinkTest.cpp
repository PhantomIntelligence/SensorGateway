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

using DataModel::SimpleData;
using DataModel::SimpleDataContent;
using SimpleDataList = std::list<SimpleData>;
using TestFunctions::DataTestUtil;
using SensorAccessLink = SensorGateway::SensorAccessLink<Sensor::Test::Simple::Structures, SimpleData>;

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
        using DataList = std::list<super::Messages>;

    public:

        explicit MockSensorCommunicationStrategy(uint8_t minimumNumberOfMessageToCreate) :
                promiseFulfilled(false),
                numberOfMessageCreated(0),
                minimumNumberOfMessageToCreate(minimumNumberOfMessageToCreate) {
        }

        void openConnection() override {}

        void closeConnection() override {}

        super::Messages fetchMessages() override {
            super::Messages createdMessages;

            for (uint32_t i = 0; i < Sensor::Test::Simple::Structures::MAX_NUMBER_OF_BULK_FETCHABLE_MESSAGES; ++i) {
                auto createdData = DataTestUtil::createRandomSimpleData();
                createdMessages[i] = createdData;
            }
            auto copy = super::Messages(createdMessages);
            createdDataCopies.push_back(copy);

            ++numberOfMessageCreated;
            if (hasCreatedMinimumNumberOfData() && !promiseFulfilled.load()) {
                promiseFulfilled.store(true);
                minimumNumberOfMessageCreated.set_value(true);
            }

            // WARNING! This mock implementation of readMessage needs to be slowed down because the way gtest works. DO NOT REMOVE.
            std::this_thread::yield();

            return createdMessages;
        }

        super::RawDataCycles fetchRawDataCycles() override {
            super::RawDataCycles createdRawDataCycles;
            return createdRawDataCycles;
        }

        void waitUntilFetchMessagesHasBeenCalledEnough() {
            if (!hasCreatedMinimumNumberOfData()) {
                minimumNumberOfMessageCreated.get_future().wait();
            }
        }

        DataList const& getCreatedMessageCopies() const {
            return createdDataCopies;
        }

    private:

        bool hasCreatedMinimumNumberOfData() {
            LockGuard guard(numberOfMessageCreatedVerificationMutex);
            return numberOfMessageCreated.load() == minimumNumberOfMessageToCreate.load();
        }

        AtomicCounter numberOfMessageCreated;
        AtomicCounter minimumNumberOfMessageToCreate;

        AtomicFlag promiseFulfilled;
        Mutex numberOfMessageCreatedVerificationMutex;
        mutable BooleanPromise minimumNumberOfMessageCreated;

        DataList createdDataCopies;
    };

    using SimpleTranslationStrategy = MessageTranslation::MessageTranslationStrategy<SimpleData, SimpleData>;

    class MockTranslationStrategy final : public SimpleTranslationStrategy {
    protected:
        using SimpleTranslationStrategy::INPUT;
        using SimpleTranslationStrategy::OUTPUT;

    public:
        MockTranslationStrategy() = default;

        void translateMessage(INPUT&& inputMessage) override {
            inputMessage.inverseContent();
            produce(std::move(inputMessage));
        }
    };

    using SimpleServerCommunicationStrategy = ServerCommunication::ServerCommunicationStrategy<SimpleData>;

    class MockServerCommunicationStrategy final : public SimpleServerCommunicationStrategy {
    protected:
        using SimpleServerCommunicationStrategy::MESSAGE;

    public:
        MockServerCommunicationStrategy() = default;

        ~MockServerCommunicationStrategy() noexcept override = default;

        void openConnection(std::string const& serverAddress) override {

        }

        void closeConnection() override {}

        void sendMessage(MESSAGE&& message) override {
            receivedData.push_back(message);
        }

        SimpleDataList const& getReceivedData() const {
            return receivedData;
        }

    private:
        SimpleDataList receivedData;
    };
}

/**
 * Medium test
 */
TEST_F(SensorAccessLinkTest,
       given_aNumberOfDataCreatedByTheSensorCommunicationStrategy_when_executing_then_aSimilarNumberOfDataEndsUpInTheServerCommunicationStrategy) {
    uint8_t numberOfDataToProcess = 42;
    SensorAccessLinkTestMock::MockSensorCommunicationStrategy mockSensorCommunicationStrategy(numberOfDataToProcess);
    SensorAccessLinkTestMock::MockTranslationStrategy mockTranslationStrategy;
    SensorAccessLinkTestMock::MockServerCommunicationStrategy mockServerCommunicationStrategy;
    SensorAccessLink sensorAccessLink(&mockServerCommunicationStrategy,
                                      &mockTranslationStrategy,
                                      &mockSensorCommunicationStrategy);

    sensorAccessLink.connect(FAKE_SERVER_ADDRESS);

    mockSensorCommunicationStrategy.waitUntilFetchMessagesHasBeenCalledEnough();

    sensorAccessLink.disconnect();

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


    sensorAccessLink.connect(FAKE_SERVER_ADDRESS);

    mockSensorCommunicationStrategy.waitUntilFetchMessagesHasBeenCalledEnough();

    sensorAccessLink.disconnect();

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

    auto numberOfProcessedData = receivedDataList.size();
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


