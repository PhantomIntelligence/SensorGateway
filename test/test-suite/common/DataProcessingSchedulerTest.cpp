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
#include "spirit-sensor-gateway/common/DataProcessingScheduler.hpp"

using ExampleDataModel::NativeData;
using ExampleDataModel::ProcessedData;

using TestFunctions::DataTestUtil;

using NativeBuffer = DataFlow::RingBuffer<NativeData>;
using OutputProducer = DataFlow::DataSource<ProcessedData>;
using ProcessingStrategy = DataFlow::DataSink<NativeData>;

using ProcessedOutputSequencer = DataFlow::OutputSequencer<ProcessedData, TestWorker>;
using NativeProcedure = DataFlow::Procedure<NativeData, ProcessedData>;
using Scheduler = DataFlow::DataProcessingScheduler<NativeData>;

class WorkSchedulerTest : public ::testing::Test {
protected:

    WorkSchedulerTest() = default;

    virtual ~WorkSchedulerTest() = default;

};


class MockProcedure : public NativeProcedure {
public:
    MockProcedure() : numberOfTimesCalled(0), processedData(
            ProcessedData(DataTestUtil::generateRandomNativeData(), DataTestUtil::initProcessedComplement())) {
    }

    ProcessedData process(NativeData&& dataIn) const override {
        ++numberOfTimesCalled;
        return expectedProcessedData();
    }

    int getNumberOfTimesCalled() const noexcept {
        return numberOfTimesCalled;
    }

    ProcessedData const& expectedProcessedData() const noexcept {
        return processedData;
    }

private:

    mutable AtomicCounter numberOfTimesCalled;
    ProcessedData processedData;
};

class MockInputBuffer : public NativeBuffer {
public:
    MockInputBuffer(uData16 numberOfConsumptionBeforeStop) :
            numberOfTimesToBeConsumedBeforeStop(numberOfConsumptionBeforeStop),
            data(DataTestUtil::generateRandomNativeData()),
            numberOfCallsToConsumeNextData(0) {
    }

    auto consumeNextDataFor(Consumer* consumer) -> DATA const& override {
        numberOfCallsToConsumeNextData++;
        if (hasBeenCalledExpectedNumberOfTimes()) {
            consumer->deactivateFor(this);
        }
        return data;
    }

    void linkWith(Consumer* consumer) noexcept override {
        linkedConsumer = consumer;
    }

    bool hasBeenCalledExpectedNumberOfTimes() const {
        return numberOfCallsToConsumeNextData.load() == numberOfTimesToBeConsumedBeforeStop;
    };

    Consumer* getLinkedConsumer() const noexcept {
        return linkedConsumer;
    }

private:
    uData16 const numberOfTimesToBeConsumedBeforeStop;
    AtomicCounter numberOfCallsToConsumeNextData;
    DATA data;
    Consumer* linkedConsumer = nullptr;
};

class MockOutputProducer : public OutputProducer {
public:
    MockOutputProducer(uData16 numberOfWriteGoal) :
            numberOfWritesGoal(numberOfWriteGoal),
            actualNumberOfWrites(0) {
    }

    void produce(DATA&& data) override {
        ++actualNumberOfWrites;
        if (hasBeenCalledExpectedNumberOfTimes()) {
            numberOfWritesAchieved.set_value(true);
        }
    }

    bool hasBeenCalledExpectedNumberOfTimes() const {
        return actualNumberOfWrites.load() == numberOfWritesGoal.load();
    };

private:
    AtomicCounter actualNumberOfWrites;
    AtomicCounter numberOfWritesGoal;
    mutable BooleanPromise numberOfWritesAchieved;
};

TEST_F(WorkSchedulerTest, given_aStoppedWorkScheduler_when_linksAnInputBuffer_then_throwsAnException) {
    MockInputBuffer inputBuffer(1);
    MockOutputProducer outputProducer(5);
    ProcessedOutputSequencer processedOutputSequencer(&outputProducer);
    processedOutputSequencer.terminateAndJoin();
    MockProcedure processMock;
    WorkerPool workerPool(&processMock);
    workerPool.terminateWorkers();
    Scheduler scheduler(&workerPool, &processedOutputSequencer);
    scheduler.terminateAndJoin();

    auto hasThrownException = false;
    try {
        scheduler.linkWith(&inputBuffer);
    } catch (std::runtime_error) {
        hasThrownException = true;
    }
    ASSERT_TRUE(hasThrownException);
}

TEST_F(WorkSchedulerTest, given_anUnlinkedInputBuffer_when_activatesForThatBuffer_then_throwsAnException) {
    MockInputBuffer inputBuffer(1);
    MockOutputProducer outputProducer(5);
    ProcessedOutputSequencer processedOutputSequencer(&outputProducer);
    MockProcedure processMock;
    WorkerPool workerPool(&processMock);
    Scheduler scheduler(&workerPool, &processedOutputSequencer);

    auto hasThrownException = false;
    try {
        scheduler.activateFor(&inputBuffer);
    } catch (std::runtime_error) {
        hasThrownException = true;
    }
    scheduler.terminateAndJoin();
    processedOutputSequencer.terminateAndJoin();
    workerPool.terminateWorkers();
    ASSERT_TRUE(hasThrownException);
}

TEST_F(WorkSchedulerTest, given_anUnlinkedInputBuffer_when_deactivatesForThatBuffer_then_throwsAnException) {
    MockInputBuffer inputBuffer(1);
    MockOutputProducer outputProducer(5);
    ProcessedOutputSequencer processedOutputSequencer(&outputProducer);
    MockProcedure processMock;
    WorkerPool workerPool(&processMock);
    Scheduler scheduler(&workerPool, &processedOutputSequencer);

    auto hasThrownException = false;
    try {
        scheduler.deactivateFor(&inputBuffer);
    } catch (std::runtime_error) {
        hasThrownException = true;
    }
    scheduler.terminateAndJoin();
    processedOutputSequencer.terminateAndJoin();
    workerPool.terminateWorkers();
    ASSERT_TRUE(hasThrownException);
}

TEST_F(WorkSchedulerTest, given_anUnlinkedInputBuffer_when_linksIt_then_callsTheRingBuffersLinkFunction) {
    MockInputBuffer inputBufferMock(1);
    MockOutputProducer outputProducer(5);
    ProcessedOutputSequencer processedOutputSequencer(&outputProducer);
    MockProcedure processMock;
    WorkerPool workerPool(&processMock);
    Scheduler scheduler(&workerPool, &processedOutputSequencer);

    scheduler.linkWith(&inputBufferMock);

    auto linkedConsumer = inputBufferMock.getLinkedConsumer();
    scheduler.terminateAndJoin();
    processedOutputSequencer.terminateAndJoin();
    workerPool.terminateWorkers();
    ASSERT_EQ(linkedConsumer, &scheduler);
}

class MockConsumptionCountingInputBuffer : public NativeBuffer {
public:
    MockConsumptionCountingInputBuffer(uData16 numberOfConsumptionBeforeStop) :
            numberOfTimesToBeConsumedBeforeStop(numberOfConsumptionBeforeStop),
            numberOfCallsToConsumeNextData(0) {}

    auto consumeNextDataFor(Consumer* consumer) -> DATA const& override {
        numberOfCallsToConsumeNextData++;
        if (hasBeenCalledExpectedNumberOfTimes()) {
            consumptionGoalReached.set_value(true);
        }
        return NativeBuffer::consumeNextDataFor(consumer);
    }

    Boolean hasBeenCalledExpectedNumberOfTimes() const {
        return numberOfCallsToConsumeNextData.load() == numberOfTimesToBeConsumedBeforeStop;
    };

    void waitConsumptionGoalToBeReached() const {
        if (!hasBeenCalledExpectedNumberOfTimes()) {
            consumptionGoalReached.get_future().wait();
        }
    }

private:
    uData16 const numberOfTimesToBeConsumedBeforeStop;
    AtomicCounter numberOfCallsToConsumeNextData;
    mutable BooleanPromise consumptionGoalReached;
};

TEST_F(WorkSchedulerTest,
       given_aLinkedInputBuffer_when_activatesForThatBuffer_then_consumesFromTheInputBufferUntilStopped) {
    uData16 numberOfConsumptionBeforeStop = NUMBER_OF_WORKER_PER_PIPE + 3;
    MockOutputProducer outputProducer(numberOfConsumptionBeforeStop);
    ProcessedOutputSequencer processedOutputSequencer(&outputProducer);
    MockConsumptionCountingInputBuffer inputBufferMock(numberOfConsumptionBeforeStop);

    MockProcedure processMock;
    WorkerPool workerPool(&processMock);
    Scheduler scheduler(&workerPool, &processedOutputSequencer);
    scheduler.linkWith(&inputBufferMock);

    for (auto l = 0; l < numberOfConsumptionBeforeStop; ++l) {
        auto newData = DataTestUtil::generateRandomNativeData();
        inputBufferMock.write(std::move(newData));
    }


    inputBufferMock.waitConsumptionGoalToBeReached();

    scheduler.terminateAndJoin();
    processedOutputSequencer.terminateAndJoin();
    workerPool.terminateWorkers();

    auto hasBeenCalledExpectedNumberOfTimes = inputBufferMock.hasBeenCalledExpectedNumberOfTimes();
    ASSERT_TRUE(hasBeenCalledExpectedNumberOfTimes);
}

TEST_F(WorkSchedulerTest,
       given_aWorkSchedulerWithTheMaximumNumberOfBufferLinked_when_attemptingToLinkOneMore_then_throwsAnIllegalNumberOfInputBufferException) {
    NativeBuffer inputBuffers[NUMBER_OF_INPUT_BUFFER_PER_PIPE];
    MockOutputProducer outputProducer(5);
    ProcessedOutputSequencer processedOutputSequencer(&outputProducer);
    MockProcedure processMock;
    WorkerPool workerPool(&processMock);
    Scheduler scheduler(&workerPool, &processedOutputSequencer);
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
    processedOutputSequencer.terminateAndJoin();
    workerPool.terminateWorkers();
    ASSERT_TRUE(hasThrownException);
}

TEST_F(WorkSchedulerTest, given_anInputBuffer_when_linkingItMoreThanOnce_then_anExceptionIsThrown) {
    MockInputBuffer inputBufferMock(0);
    MockOutputProducer outputProducer(5);
    ProcessedOutputSequencer processedOutputSequencer(&outputProducer);
    MockProcedure processMock;
    WorkerPool workerPool(&processMock);
    Scheduler scheduler(&workerPool, &processedOutputSequencer);

    auto hasThrownException = false;
    scheduler.linkWith(&inputBufferMock);
    try {
        scheduler.linkWith(&inputBufferMock);
    } catch (std::runtime_error const&) {
        hasThrownException = true;
    }
    scheduler.terminateAndJoin();
    processedOutputSequencer.terminateAndJoin();
    workerPool.terminateWorkers();
    ASSERT_TRUE(hasThrownException);
}

TEST_F(WorkSchedulerTest,
       given_twoLinkedInputBuffer_when_activatesForTheSecondBuffer_then_theFirstBufferIsNotCalledAndTheSecondBufferIsCalled) {
    uData16 numberOfConsumptionBeforeStop = 8;
    MockConsumptionCountingInputBuffer firstBufferMock(0);
    MockConsumptionCountingInputBuffer secondBufferMock(numberOfConsumptionBeforeStop);
    MockOutputProducer outputProducer(numberOfConsumptionBeforeStop);
    ProcessedOutputSequencer processedOutputSequencer(&outputProducer);
    MockProcedure processMock;
    WorkerPool workerPool(&processMock);
    Scheduler scheduler(&workerPool, &processedOutputSequencer);
    scheduler.linkWith(&firstBufferMock);
    scheduler.linkWith(&secondBufferMock);

    for (auto l = 0; l < numberOfConsumptionBeforeStop; ++l) {
        auto nativeData = DataTestUtil::generateRandomNativeData();
        secondBufferMock.write(std::move(nativeData));
    }

    firstBufferMock.waitConsumptionGoalToBeReached();
    secondBufferMock.waitConsumptionGoalToBeReached();

    scheduler.terminateAndJoin();
    processedOutputSequencer.terminateAndJoin();
    workerPool.terminateWorkers();

    auto firstHasBeenCalledExpectedNumberOfTimes = firstBufferMock.hasBeenCalledExpectedNumberOfTimes();
    ASSERT_TRUE(firstHasBeenCalledExpectedNumberOfTimes);
    auto secondHasBeenCalledExpectedNumberOfTimes = secondBufferMock.hasBeenCalledExpectedNumberOfTimes();
    ASSERT_TRUE(secondHasBeenCalledExpectedNumberOfTimes);
}

/**
 * @note: Medium test
 */
TEST_F(WorkSchedulerTest,
       given_aNumberOfData_when_dataIswrittenToTheInputBuffer_then_callsTheProcedureTheSameNumberOfTimes) {
    NativeBuffer inputBuffer;
    uData16 numberOfData = 16;
    MockOutputProducer outputProducer(numberOfData);
    ProcessedOutputSequencer processedOutputSequencer(&outputProducer);
    MockProcedure processMock;
    WorkerPool workerPool(&processMock);
    Scheduler scheduler(&workerPool, &processedOutputSequencer);
    scheduler.linkWith(&inputBuffer);

    for (uData16 k = 0; k < numberOfData; ++k) {
        auto nativeData = DataTestUtil::generateRandomNativeData();
        inputBuffer.write(std::move(nativeData));
    }

    scheduler.terminateAndJoin();
    processedOutputSequencer.terminateAndJoin();
    workerPool.terminateWorkers();

    auto numberOfTimesProcessExecuted = processMock.getNumberOfTimesCalled();
    ASSERT_EQ(numberOfData, numberOfTimesProcessExecuted);
}

class MockSequenceCountingOutputSequencer : public ProcessedOutputSequencer {
    using super = ProcessedOutputSequencer;
public:
    MockSequenceCountingOutputSequencer(uData16 goal, MockOutputProducer* outputProducer) :
            ProcessedOutputSequencer(outputProducer),
            numberOfCallGoal(goal),
            numberOfCallsToConsumeNextData(0) {}

    void sequenceWork(TestWorker* nextWorker) override {
        super::sequenceWork(nextWorker);
        numberOfCallsToConsumeNextData++;
    }

    bool hasBeenCalledExpectedNumberOfTimes() const {
        return numberOfCallsToConsumeNextData.load() == numberOfCallGoal;
    };

private:
    uData16 const numberOfCallGoal;
    AtomicCounter numberOfCallsToConsumeNextData;
};

/**
 * @note: Medium test
 */
TEST_F(WorkSchedulerTest,
       given_aNumberOfData_when_dataIswrittenToTheInputBuffer_then_callsTheOutputSequencerTheSameNumberOfTimes) {
    NativeBuffer inputBuffer;
    uData16 numberOfData = 5;
    MockOutputProducer outputProducer(numberOfData);
    MockSequenceCountingOutputSequencer processedOutputSequencer(numberOfData, &outputProducer);
    MockProcedure processMock;
    WorkerPool workerPool(&processMock);
    Scheduler scheduler(&workerPool, &processedOutputSequencer);
    scheduler.linkWith(&inputBuffer);

    for (uData16 k = 0; k < numberOfData; ++k) {
        auto nativeData = DataTestUtil::generateRandomNativeData();
        inputBuffer.write(std::move(nativeData));
    }

    scheduler.terminateAndJoin();
    processedOutputSequencer.terminateAndJoin();
    workerPool.terminateWorkers();

    auto sequencerCalledCorrectNumberOfTimes = processedOutputSequencer.hasBeenCalledExpectedNumberOfTimes();
    ASSERT_TRUE(sequencerCalledCorrectNumberOfTimes);
}

#endif //SPIRITSENSORGATEWAY_WORKSCHEDULERTEST_CPP
