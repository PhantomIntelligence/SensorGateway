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


#ifndef SENSORGATEWAY_CONSTANTSIZEDPOINTERQUEUETEST_CPP
#define SENSORGATEWAY_CONSTANTSIZEDPOINTERQUEUETEST_CPP

#include <gtest/gtest.h>
#include "sensor-gateway/common/container/ConstantSizedPointerQueue.hpp"
#include "test/utilities/data-model/DataModelFixture.h"

using DataModel::SimpleMessage;

using TestFunctions::DataTestUtil;

class ConstantSizedPointerQueueTest : public ::testing::Test {
protected:

    ConstantSizedPointerQueueTest() = default;

    virtual ~ConstantSizedPointerQueueTest() = default;

public:

    static const uint16_t TEST_SIZE = 8;

    using Queue = Container::ConstantSizedPointerQueue<SimpleMessage, ConstantSizedPointerQueueTest::TEST_SIZE>;

    void fillQueue(Queue* queue) const {
        SimpleMessage data = DataTestUtil::createRandomSimpleMessage();
        auto pointer = &data;

        for (auto i = 0; i < ConstantSizedPointerQueueTest::TEST_SIZE; ++i) {
            queue->store(pointer);
        }
    }
};

TEST_F(ConstantSizedPointerQueueTest, given_anEmptyQueue_when_askedIfIsEmpty_then_returnsTrue) {
    Queue queue;

    auto empty = queue.isEmpty();

    ASSERT_TRUE(empty);
}

TEST_F(ConstantSizedPointerQueueTest, given_anQueueWithOneElement_when_askedIfIsEmpty_then_returnsFalse) {
    Queue queue;
    SimpleMessage data = DataTestUtil::createRandomSimpleMessage();
    auto pointer = &data;
    queue.store(pointer);

    auto empty = queue.isEmpty();

    ASSERT_FALSE(empty);
}

TEST_F(ConstantSizedPointerQueueTest, given_anEmptyQueue_when_askedIfIsFull_then_returnsFalse) {
    Queue queue;

    auto full = queue.isFull();

    ASSERT_FALSE(full);
}

TEST_F(ConstantSizedPointerQueueTest, given_aFullQueue_when_askedIfIsFull_then_returnsTrue) {
    Queue queue;
    fillQueue(&queue);

    auto full = queue.isFull();

    ASSERT_TRUE(full);
}

TEST_F(ConstantSizedPointerQueueTest, given_anEmptyQueue_when_consumeNext_then_throwsAnException) {
    Queue queue;

    ASSERT_THROW(queue.consumeNext(), std::runtime_error);
}

TEST_F(ConstantSizedPointerQueueTest, given_aFullQueue_when_storingANewPointer_then_throwsAnException) {
    Queue queue;
    fillQueue(&queue);

    SimpleMessage data = DataTestUtil::createRandomSimpleMessage();
    auto pointer = &data;

    ASSERT_THROW(queue.store(pointer), std::runtime_error);
}

TEST_F(ConstantSizedPointerQueueTest, given_aStoredPointer_when_consumeNext_then_returnsThePointer) {
    Queue queue;
    SimpleMessage data = DataTestUtil::createRandomSimpleMessage();
    auto pointer = &data;
    queue.store(pointer);

    auto storedPointer = queue.consumeNext();

    ASSERT_EQ(pointer, storedPointer);
}

TEST_F(ConstantSizedPointerQueueTest, given_twoPointerStoredInOrder_when_consumed_then_returnedInTheOrderTheyWereStored) {
    Queue queue;
    SimpleMessage firstData = DataTestUtil::createRandomSimpleMessage();
    SimpleMessage secondData = DataTestUtil::createRandomSimpleMessage();
    auto firstPointer = &firstData;
    auto secondPointer = &secondData;
    queue.store(firstPointer);
    queue.store(secondPointer);

    auto firstStoredPointer = queue.consumeNext();
    auto secondStoredPointer = queue.consumeNext();

    ASSERT_EQ(firstPointer, firstStoredPointer);
    ASSERT_EQ(secondPointer, secondStoredPointer);
}

TEST_F(ConstantSizedPointerQueueTest, given_aPointerInTheQueue_when_consumeNext_then_returnsThePointer) {
    Queue queue;
    SimpleMessage data = DataTestUtil::createRandomSimpleMessage();
    auto pointer = &data;
    queue.store(pointer);

    auto storedPointer = queue.consumeNext();

    ASSERT_EQ(pointer, storedPointer);
}

TEST_F(ConstantSizedPointerQueueTest, given_aNonFullQueue_when_fillingAndConsumingTheQueueTwice_then_dataRemainsCoherent) {
    Queue queue;

    for (auto j = 0; j < ConstantSizedPointerQueueTest::TEST_SIZE * 2; ++j) {
        SimpleMessage data = DataTestUtil::createRandomSimpleMessage();
        auto pointer = &data;
        queue.store(pointer);
        auto storedPointer = queue.consumeNext();

        EXPECT_EQ(data, *storedPointer);
    }
}

#endif //SENSORGATEWAY_CONSTANTSIZEDPOINTERQUEUETEST_CPP
