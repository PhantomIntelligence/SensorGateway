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

#ifndef SPIRITSERVERGATEWAY_SENSORACCESSLINKTEST_CPP
#define SPIRITSERVERGATEWAY_SENSORACCESSLINKTEST_CPP

#include <gtest/gtest.h>
#include <list>

#include "spirit-sensor-gateway/application/SensorAccessLink.hpp"
#include "test/utilities/data-model/DataModelFixture.h"

using DataModel::SimpleData;
using DataModel::SimpleDataContent;
using SimpleDataList = std::list<SimpleData>;
using TestFunctions::DataTestUtil;
using SensorAccessLink = SpiritSensorGateway::SensorAccessLink<SimpleData, SimpleData>;

class SensorAccessLinkTest : public ::testing::Test {

protected:
    SensorAccessLinkTest() = default;

    virtual ~SensorAccessLinkTest() = default;

};

using SimpleSensorCommunicationStrategy = SensorCommunication::SensorCommunicationStrategy<SimpleData>;

namespace SensorAccessLinkTestMock {

    class MockSensorCommunicationStrategy final : public SimpleSensorCommunicationStrategy {

    protected:

        using SimpleSensorCommunicationStrategy::DATA;
        using DataList = std::list<DATA>;

    public:

        explicit MockSensorCommunicationStrategy(uint8_t minimumNumberOfMessageToCreate) :
                promiseAlreadyFulfilled(false),
                numberOfMessageCreated(0),
                minimumNumberOfMessageToCreate(minimumNumberOfMessageToCreate) {
        }

        void openConnection() override {}

        void closeConnection() override {}

        DATA readMessage() override {
            DATA data = DataTestUtil::createRandomSimpleData();
            auto copy = DATA(data);
            createdDataCopies.push_back(copy);

            ++numberOfMessageCreated;
            if (hasCreatedMinimumNumberOfData() && !promiseAlreadyFulfilled.load()) {
                promiseAlreadyFulfilled.store(true);
                requiredNumberOfMessageCreated.set_value(true);
            }

            // WARNING! This mock implementation of readMessage needs to be slowed down because the way gtest works. DO NOT REMOVE.
            std::this_thread::sleep_for(std::chrono::milliseconds(2));
            std::this_thread::yield();

            return data;
        }

        void waitUntilReadMessageHasBeenCalledMinimumNumberOfTimes() {
            if (!hasCreatedMinimumNumberOfData()) {
                requiredNumberOfMessageCreated.get_future().wait();
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

        AtomicFlag promiseAlreadyFulfilled;
        Mutex numberOfMessageCreatedVerificationMutex;
        mutable BooleanPromise requiredNumberOfMessageCreated;

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

        void openConnection() override {}

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
    SensorAccessLink sensorAccessLink(&mockSensorCommunicationStrategy,
                                      &mockTranslationStrategy,
                                      &mockServerCommunicationStrategy);

    sensorAccessLink.start();

    mockSensorCommunicationStrategy.waitUntilReadMessageHasBeenCalledMinimumNumberOfTimes();

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
    SensorAccessLink sensorAccessLink(&mockSensorCommunicationStrategy,
                                      &mockTranslationStrategy,
                                      &mockServerCommunicationStrategy);

    sensorAccessLink.start();

    mockSensorCommunicationStrategy.waitUntilReadMessageHasBeenCalledMinimumNumberOfTimes();

    sensorAccessLink.terminateAndJoin();

    auto createdDataList = mockSensorCommunicationStrategy.getCreatedMessageCopies();
    auto receivedDataList = mockServerCommunicationStrategy.getReceivedData();

    auto numberOfProcessedData = receivedDataList.size();
    bool dataHasPassedThroughCorrectly = true;
    for (int i = 0; i < numberOfProcessedData && dataHasPassedThroughCorrectly; ++i) {
        auto createdData = createdDataList.front();
        createdDataList.pop_front();
        auto receivedData = receivedDataList.front();
        receivedDataList.pop_front();
        dataHasPassedThroughCorrectly = createdData.isTheInverseOf(receivedData);
    }

    ASSERT_TRUE(dataHasPassedThroughCorrectly);
}

#endif //SPIRITSERVERGATEWAY_SENSORACCESSLINKTEST_CPP
