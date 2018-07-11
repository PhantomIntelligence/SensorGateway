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


#ifndef SPIRITSENSORGATEWAY_WORKSCHEDULERTEST_CPP
#define SPIRITSENSORGATEWAY_WORKSCHEDULERTEST_CPP

#include <gtest/gtest.h>
#include "data-model/DataModelFixture.h"
#include "spirit-sensor-gateway/common/data-flow/DataProcessingScheduler.hpp"

using DataFlow::NUMBER_OF_CONCURRENT_INPUT_FOR_SENSOR_ACCESS_LINK_ELEMENTS;
using ExampleDataModel::NativeData;
using ExampleDataModel::ProcessedData;
using TestFunctions::DataTestUtil;
using NativeBuffer = DataFlow::RingBuffer<NativeData>;
using OutputProducer = DataFlow::DataSource<ProcessedData>;
using ProcessingStrategy = DataFlow::DataSink<NativeData>;
using NativeSink = DataFlow::DataSink<NativeData>;

class DataProcessingSchedulerTest : public ::testing::Test {
protected:

    DataProcessingSchedulerTest() = default;

    virtual ~DataProcessingSchedulerTest() = default;

    uint8_t const ARBITRARY_NUMBER_OF_CONSUMPTION_BEFORE_STOP = 1;
    uint8_t const ARBITRARY_NUMBER_OF_CALL_GOAL = 42;

};

class MockInputBuffer : public NativeBuffer {
public:
    explicit MockInputBuffer(uint16_t numberOfConsumptionBeforeStop) :
            numberOfTimesToBeConsumedBeforeStop(numberOfConsumptionBeforeStop),
            data(DataTestUtil::generateRandomNativeData()),
            numberOfCallsToConsumeNextData(0) {
    }

    auto consumeNextDataFor(Consumer* consumer) -> NativeData const& override {
        numberOfCallsToConsumeNextData++;
        if (hasBeenCalledExpectedNumberOfTimes()) {
            consumptionGoalReached.set_value(true);
        }
        return NativeBuffer::consumeNextDataFor(consumer);
    }

    void linkWith(Consumer* consumer) noexcept override {
        linkedConsumer = consumer;
        NativeBuffer::linkWith(consumer);
    }

    bool hasBeenCalledExpectedNumberOfTimes() const {
        return numberOfCallsToConsumeNextData.load() == numberOfTimesToBeConsumedBeforeStop;
    };

    void waitConsumptionGoalToBeReached() const {
        if (!hasBeenCalledExpectedNumberOfTimes()) {
            consumptionGoalReached.get_future().wait();
        }
    }

    Consumer* getLinkedConsumer() const noexcept {
        return linkedConsumer;
    }

private:
    uint16_t const numberOfTimesToBeConsumedBeforeStop;
    AtomicCounter numberOfCallsToConsumeNextData;
    NativeData data;
    Consumer* linkedConsumer = nullptr;
    mutable BooleanPromise consumptionGoalReached;
};

class MockSink : public NativeSink {
public:
    explicit MockSink(uint8_t numberOfWriteGoal) :
            numberOfWritesGoal(numberOfWriteGoal),
            actualNumberOfWrites(0) {
    }

    void consume(NativeData&& data) override {
        ++actualNumberOfWrites;
        if (hasBeenCalledExpectedNumberOfTimes()) {
            numberOfWritesAchieved.set_value(true);
        }
    }

    bool hasBeenCalledExpectedNumberOfTimes() const {
        return actualNumberOfWrites.load() == numberOfWritesGoal.load();
    };

    void waitUntillReadMessageIsCalled() const {
        if (!hasBeenCalledExpectedNumberOfTimes()) {
            numberOfWritesAchieved.get_future().wait();
        }
    }

private:
    AtomicCounter actualNumberOfWrites;
    AtomicCounter numberOfWritesGoal;
    mutable BooleanPromise numberOfWritesAchieved;
};

using SingleInputScheduler = DataFlow::DataProcessingScheduler<NativeData, MockSink, NUMBER_OF_CONCURRENT_INPUT_FOR_SENSOR_ACCESS_LINK_ELEMENTS>;

TEST_F(DataProcessingSchedulerTest, given_aStoppedWorkScheduler_when_linksAnInputBuffer_then_throwsAnException) {
    MockSink mockSink(ARBITRARY_NUMBER_OF_CALL_GOAL);
    SingleInputScheduler scheduler(&mockSink);
    scheduler.terminateAndJoin();

    NativeBuffer inputBuffer;

    ASSERT_THROW(scheduler.linkWith(&inputBuffer), std::runtime_error);
}

TEST_F(DataProcessingSchedulerTest, given_anUnlinkedInputBuffer_when_activatesForThatBuffer_then_throwsAnException) {
    MockSink mockSink(ARBITRARY_NUMBER_OF_CALL_GOAL);
    SingleInputScheduler scheduler(&mockSink);
    scheduler.terminateAndJoin();

    NativeBuffer inputBuffer;
    auto hasThrownException = false;
    try {
        scheduler.activateFor(&inputBuffer);
    } catch (std::runtime_error) {
        hasThrownException = true;
    }

    scheduler.terminateAndJoin();
    ASSERT_TRUE(hasThrownException);
}

TEST_F(DataProcessingSchedulerTest, given_anUnlinkedInputBuffer_when_deactivatesForThatBuffer_then_throwsAnException) {
    MockSink mockSink(ARBITRARY_NUMBER_OF_CALL_GOAL);
    SingleInputScheduler scheduler(&mockSink);
    scheduler.terminateAndJoin();

    NativeBuffer inputBuffer;
    auto hasThrownException = false;
    try {
        scheduler.deactivateFor(&inputBuffer);
    } catch (std::runtime_error) {
        hasThrownException = true;
    }

    scheduler.terminateAndJoin();
    ASSERT_TRUE(hasThrownException);
}

TEST_F(DataProcessingSchedulerTest, given_anUnlinkedInputBuffer_when_linksIt_then_callsTheRingBuffersLinkFunctionWithAPointerToTheScheduler) {
    MockInputBuffer inputBufferMock(ARBITRARY_NUMBER_OF_CONSUMPTION_BEFORE_STOP);
    MockSink mockSink(ARBITRARY_NUMBER_OF_CALL_GOAL);
    SingleInputScheduler scheduler(&mockSink);

    scheduler.linkWith(&inputBufferMock);

    auto linkedConsumer = inputBufferMock.getLinkedConsumer();
    scheduler.terminateAndJoin();
    ASSERT_EQ(linkedConsumer, &scheduler);
}

TEST_F(DataProcessingSchedulerTest,
       given_aLinkedInputBuffer_when_activatesForThatBuffer_then_consumesFromTheInputBufferUntilStopped) {
    MockSink mockSink(ARBITRARY_NUMBER_OF_CALL_GOAL);
    SingleInputScheduler scheduler(&mockSink);
    MockInputBuffer inputBufferMock(ARBITRARY_NUMBER_OF_CALL_GOAL);
    scheduler.linkWith(&inputBufferMock);

    for (auto l = 0; l < ARBITRARY_NUMBER_OF_CALL_GOAL; ++l) {
        auto newData = DataTestUtil::generateRandomNativeData();
        inputBufferMock.write(std::move(newData));
    }

    inputBufferMock.waitConsumptionGoalToBeReached();
    scheduler.terminateAndJoin();

    auto hasBeenCalledExpectedNumberOfTimes = inputBufferMock.hasBeenCalledExpectedNumberOfTimes();
    ASSERT_TRUE(hasBeenCalledExpectedNumberOfTimes);
}

TEST_F(DataProcessingSchedulerTest,
       given_aWorkSchedulerWithTheMaximumNumberOfBufferLinked_when_attemptingToLinkOneMore_then_throwsAnIllegalNumberOfInputBufferException) {
    MockSink mockSink(ARBITRARY_NUMBER_OF_CALL_GOAL);
    SingleInputScheduler scheduler(&mockSink);
    NativeBuffer inputBuffers[NUMBER_OF_CONCURRENT_INPUT_FOR_SENSOR_ACCESS_LINK_ELEMENTS];
    for (auto& inputBuffer : inputBuffers) {
        scheduler.linkWith(&inputBuffer);
    }

    auto hasThrownException = false;
    try {
        MockInputBuffer inputBufferMock(0);
        scheduler.linkWith(&inputBufferMock);
    } catch (std::runtime_error const&) {
        hasThrownException = true;
    }

    scheduler.terminateAndJoin();
    ASSERT_TRUE(hasThrownException);
}

TEST_F(DataProcessingSchedulerTest, given_anInputBuffer_when_linkingItMoreThanOnce_then_anExceptionIsThrown) {
    MockSink mockSink(ARBITRARY_NUMBER_OF_CALL_GOAL);
    SingleInputScheduler scheduler(&mockSink);

    NativeBuffer inputBuffer;
    auto hasThrownException = false;
    scheduler.linkWith(&inputBuffer);
    try {
        scheduler.linkWith(&inputBuffer);
    } catch (std::runtime_error const&) {
        hasThrownException = true;
    }
    scheduler.terminateAndJoin();
    ASSERT_TRUE(hasThrownException);
}


TEST_F(DataProcessingSchedulerTest,
       given_twoLinkedInputBuffer_when_activatesForTheSecondBuffer_then_theFirstBufferIsNotCalledAndTheSecondBufferIsCalled) {
    uint8_t const NUMBER_OF_INPUT_BUFFERS = 2;
    using Scheduler = DataFlow::DataProcessingScheduler<NativeData, MockSink, NUMBER_OF_INPUT_BUFFERS>;
    uint8_t const noConsumption = 0;
    uint8_t const numberOfConsumptionExpectedForSecondBuffer = 8;
    uint8_t const numberOfConsumptionBeforeStop = noConsumption + numberOfConsumptionExpectedForSecondBuffer;

    MockInputBuffer firstBufferMock(noConsumption);
    MockInputBuffer secondBufferMock(numberOfConsumptionExpectedForSecondBuffer);
    MockSink mockSink(numberOfConsumptionBeforeStop);
    Scheduler scheduler(&mockSink);
    scheduler.linkWith(&firstBufferMock);
    scheduler.linkWith(&secondBufferMock);

    for (auto l = 0; l < numberOfConsumptionExpectedForSecondBuffer; ++l) {
        auto nativeData = DataTestUtil::generateRandomNativeData();
        secondBufferMock.write(std::move(nativeData));
    }

    firstBufferMock.waitConsumptionGoalToBeReached();
    secondBufferMock.waitConsumptionGoalToBeReached();

    scheduler.terminateAndJoin();

    auto firstHasBeenCalledExpectedNumberOfTimes = firstBufferMock.hasBeenCalledExpectedNumberOfTimes();
    ASSERT_TRUE(firstHasBeenCalledExpectedNumberOfTimes);
    auto secondHasBeenCalledExpectedNumberOfTimes = secondBufferMock.hasBeenCalledExpectedNumberOfTimes();
    ASSERT_TRUE(secondHasBeenCalledExpectedNumberOfTimes);
}

/**
 * @note: Medium test
 */
TEST_F(DataProcessingSchedulerTest,
       given_aNumberOfData_when_dataIswrittenToTheInputBuffer_then_callsTheDataSinkTheSameNumberOfTimes) {
    NativeBuffer inputBuffer;
    MockSink mockSink(ARBITRARY_NUMBER_OF_CALL_GOAL);
    SingleInputScheduler scheduler(&mockSink);

    scheduler.linkWith(&inputBuffer);

    for (auto k = 0; k < ARBITRARY_NUMBER_OF_CALL_GOAL; ++k) {
        auto nativeData = DataTestUtil::generateRandomNativeData();
        inputBuffer.write(std::move(nativeData));
    }

    mockSink.waitUntillReadMessageIsCalled();

    scheduler.terminateAndJoin();

    auto calledExpectedNumberOfTimes = mockSink.hasBeenCalledExpectedNumberOfTimes();
    ASSERT_TRUE(calledExpectedNumberOfTimes);
}

#endif //SPIRITSENSORGATEWAY_WORKSCHEDULERTEST_CPP
