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


#ifndef SPIRITSENSORGATEWAY_CONSTANTSIZEDPOINTERQUEUETEST_CPP
#define SPIRITSENSORGATEWAY_CONSTANTSIZEDPOINTERQUEUETEST_CPP

#include <gtest/gtest.h>
#include "spirit-sensor-gateway/common/ConstantSizedPointerQueue.hpp"
#include "data-model/DataModelFixture.h"

using ExampleDataModel::NativeData;

using TestFunctions::DataTestUtil;

class ConstantSizedPointerQueueTest : public ::testing::Test {
protected:

    ConstantSizedPointerQueueTest() {}

    virtual ~ConstantSizedPointerQueueTest() {}

public:

    static const uint16_t TEST_SIZE = 8;

    using Array = Processing::ConstantSizedPointerQueue<NativeData, ConstantSizedPointerQueueTest::TEST_SIZE>;

    void fillArray(Array* array) const;
};

void ConstantSizedPointerQueueTest::fillArray(Array* array) const {
    NativeData data = DataTestUtil::generateRandomNativeData();
    auto pointer = &data;

    for (auto i = 0; i < ConstantSizedPointerQueueTest::TEST_SIZE; ++i) {
        array->store(pointer);
    }
}

TEST_F(ConstantSizedPointerQueueTest, given_anEmptyArray_when_askedIfIsEmpty_then_returnsTrue) {
    Array array;

    auto empty = array.empty();

    ASSERT_TRUE(empty);
}

TEST_F(ConstantSizedPointerQueueTest, given_anEmptyArray_when_askedIfIsEmpty_then_returnsFalse) {
    Array array;
    NativeData data = DataTestUtil::generateRandomNativeData();
    auto pointer = &data;
    array.store(pointer);

    auto empty = array.empty();

    ASSERT_FALSE(empty);
}

TEST_F(ConstantSizedPointerQueueTest, given_anEmptyArray_when_askedIfIsFull_then_returnsFalse) {
    Array array;

    auto full = array.full();

    ASSERT_FALSE(full);
}

TEST_F(ConstantSizedPointerQueueTest, given_aFullArray_when_askedIfIsFull_then_returnsTrue) {
    Array array;
    fillArray(&array);

    auto full = array.full();

    ASSERT_TRUE(full);
}

TEST_F(ConstantSizedPointerQueueTest, given_anEmptyArray_when_consumeNext_then_throwsAnException) {
    Array array;

    auto hasThrownException = false;
    try {
        array.consumeNext();
    } catch (std::runtime_error& error) {
        hasThrownException = true;
    }

    ASSERT_TRUE(hasThrownException);
}

TEST_F(ConstantSizedPointerQueueTest, given_aFullArray_when_storingANewPointer_then_throwsAnException) {
    Array array;
    fillArray(&array);

    auto hasThrownException = false;
    try {
        NativeData data = DataTestUtil::generateRandomNativeData();
        auto pointer = &data;
        array.store(pointer);
    } catch (std::runtime_error& error) {
        hasThrownException = true;
    }

    ASSERT_TRUE(hasThrownException);
}

TEST_F(ConstantSizedPointerQueueTest, given_aStoredPointer_when_consumeNext_then_returnsThePointer) {
    Array array;
    NativeData data = DataTestUtil::generateRandomNativeData();
    auto pointer = &data;
    array.store(pointer);

    auto storedPointer = array.consumeNext();

    ASSERT_EQ(pointer, storedPointer);
}

TEST_F(ConstantSizedPointerQueueTest, given_twoPointerStoredInOrder_when_consumed_then_returnedInTheOrderTheyWereStored) {
    Array array;
    NativeData firstData = DataTestUtil::generateRandomNativeData();
    NativeData secondData = DataTestUtil::generateRandomNativeData();
    auto firstPointer = &firstData;
    auto secondPointer = &secondData;
    array.store(firstPointer);
    array.store(secondPointer);

    auto firstStoredPointer = array.consumeNext();
    auto secondStoredPointer = array.consumeNext();

    ASSERT_EQ(firstPointer, firstStoredPointer);
    ASSERT_EQ(secondPointer, secondStoredPointer);
}

TEST_F(ConstantSizedPointerQueueTest, given__when_consumeNext_then_returnsThePointer) {
    Array array;
    NativeData data = DataTestUtil::generateRandomNativeData();
    auto pointer = &data;
    array.store(pointer);

    auto storedPointer = array.consumeNext();

    ASSERT_EQ(pointer, storedPointer);
}

TEST_F(ConstantSizedPointerQueueTest, given__when_fillingAndConsumingTheArrayTwice_then_dataRemainsCoherent) {
    Array array;

    for (auto j = 0; j < ConstantSizedPointerQueueTest::TEST_SIZE * 2; ++j) {
        NativeData data = DataTestUtil::generateRandomNativeData();
        auto pointer = &data;
        array.store(pointer);
        auto storedPointer = array.consumeNext();

        EXPECT_EQ(data, *storedPointer);
    }
}

#endif //SPIRITSENSORGATEWAY_CONSTANTSIZEDPOINTERQUEUETEST_CPP
