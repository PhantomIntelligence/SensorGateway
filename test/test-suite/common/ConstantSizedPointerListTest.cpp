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


#ifndef SPIRITSENSORGATEWAY_CONSTANTSIZEDPOINTERLISTTEST_CPP
#define SPIRITSENSORGATEWAY_CONSTANTSIZEDPOINTERLISTTEST_CPP

#include <gtest/gtest.h>

#include "spirit-sensor-gateway/common/ConstantSizedPointerList.hpp"
#include "data-model/DataModelFixture.h"

using ExampleDataModel::NativeData;
using TestFunctions::DataTestUtil;
using std::runtime_error;

class ConstantSizedPointerListTest : public ::testing::Test {
protected:

    ConstantSizedPointerListTest() {}

    virtual ~ConstantSizedPointerListTest() {}

public:

    static const uint16_t TEST_SIZE = 8;

    using List = Processing::ConstantSizedPointerList<NativeData, ConstantSizedPointerListTest::TEST_SIZE>;

    void fillList(List* list) const {
        NativeData data = DataTestUtil::generateRandomNativeData();
        auto pointer = &data;

        for (auto i = 0; i < ConstantSizedPointerListTest::TEST_SIZE; ++i) {
            list->store(pointer);
        }
    }
};


TEST_F(ConstantSizedPointerListTest, given_anEmptyList_when_askedIfIsEmpty_then_returnsTrue) {
    List list;

    auto empty = list.empty();

    ASSERT_TRUE(empty);
}

TEST_F(ConstantSizedPointerListTest, given_aListHoldingOneData_when_askedIfIsEmpty_then_returnsFalse) {
    List list;
    NativeData data = DataTestUtil::generateRandomNativeData();
    auto pointer = &data;
    list.store(pointer);

    auto empty = list.empty();

    ASSERT_FALSE(empty);
}

TEST_F(ConstantSizedPointerListTest, given_anEmptyList_when_askedIfIsFull_then_returnsFalse) {
    List list;

    auto full = list.full();

    ASSERT_FALSE(full);
}

TEST_F(ConstantSizedPointerListTest, given_aFullList_when_askedIfIsFull_then_returnsTrue) {
    List list;
    fillList(&list);

    auto full = list.full();

    ASSERT_TRUE(full);
}

TEST_F(ConstantSizedPointerListTest, given_anEmptyList_when_consumeNext_then_throwsAnException) {
    List list;

    ASSERT_THROW(list.consumeNext(), std::runtime_error);
}

TEST_F(ConstantSizedPointerListTest, given_aFullList_when_storingANewPointer_then_throwsAnException) {
    List list;
    fillList(&list);

    NativeData data = DataTestUtil::generateRandomNativeData();
    auto pointer = &data;

    ASSERT_THROW(list.store(pointer), std::runtime_error);
}

TEST_F(ConstantSizedPointerListTest, given_aStoredPointer_when_consumeNext_then_returnsThePointer) {
    List list;
    NativeData data = DataTestUtil::generateRandomNativeData();
    auto pointer = &data;
    list.store(pointer);

    auto storedPointer = list.consumeNext();

    ASSERT_EQ(pointer, storedPointer);
}

TEST_F(ConstantSizedPointerListTest, given_aListOfSizeN_when_storingAndConsumingInAListTwoNTimes_then_dataRemainsCoherent) {
    List list;

    for (auto j = 0; j < ConstantSizedPointerListTest::TEST_SIZE * 2; ++j) {
        NativeData data = DataTestUtil::generateRandomNativeData();
        auto pointer = &data;
        list.store(pointer);
        auto storedPointer = list.consumeNext();

        EXPECT_EQ(data, *storedPointer);
    }
}

TEST_F(ConstantSizedPointerListTest, given_aFullList_when_readAllTheElementsAndAskIfFull_then_returnsTrue) {
    List list;
    fillList(&list);
    for (auto k = 0; k < ConstantSizedPointerListTest::TEST_SIZE; ++k) {
        list.readNext();
    }
    auto full = list.full();

    ASSERT_TRUE(full);
}

TEST_F(ConstantSizedPointerListTest, given_aPointerNotInTheList_when_askedIfItIsContained_then_returnsFalse) {
    List list;
    NativeData data = DataTestUtil::generateRandomNativeData();
    auto pointer = &data;

    auto isInList = list.contains(pointer);

    ASSERT_FALSE(isInList);
}

TEST_F(ConstantSizedPointerListTest, given_aPointerInTheList_when_askedIfItIsContained_then_returnsTrue) {
    List list;
    NativeData data = DataTestUtil::generateRandomNativeData();
    auto pointer = &data;
    list.store(pointer);

    auto isInList = list.contains(pointer);

    ASSERT_TRUE(isInList);
}

TEST_F(ConstantSizedPointerListTest, given_aPointerNotInTheList_when_askedToRemoveIt_then_throwsAnException) {
    List list;
    NativeData data = DataTestUtil::generateRandomNativeData();
    auto pointer = &data;

    ASSERT_THROW(list.remove(pointer), std::runtime_error);
}

TEST_F(ConstantSizedPointerListTest, given_aPointerInTheList_when_askedToRemoveIt_then_thePointerIsNoLongerContainedInTheList) {
    List list;
    NativeData data = DataTestUtil::generateRandomNativeData();
    auto pointer = &data;
    list.store(pointer);

    list.remove(pointer);

    auto isInList = list.contains(pointer);
    ASSERT_FALSE(isInList);
}

TEST_F(ConstantSizedPointerListTest, given_aPointerInTheList_when_askedToRemoveIt_then_theListIsEmpty) {
    List list;
    NativeData data = DataTestUtil::generateRandomNativeData();
    auto pointer = &data;
    list.store(pointer);

    list.remove(pointer);

    auto empty = list.empty();
    ASSERT_TRUE(empty);
}

#endif //SPIRITSENSORGATEWAY_CONSTANTSIZEDPOINTERLISTTEST_CPP
