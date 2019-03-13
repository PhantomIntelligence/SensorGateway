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

#ifndef SENSORGATEWAY_RINGBUFFERPADTEST_CPP
#define SENSORGATEWAY_RINGBUFFERPADTEST_CPP

#include <gtest/gtest.h>

#include "sensor-gateway/common/data-flow/RingBufferPad.hpp"
#include "test/utilities/data-model/DataModelFixture.h"

using DataFlow::RingBufferPad;
using TestFunctions::DataTestUtil;

typedef RingBufferPad<DataModel::SimpleMessage> Pad;

/**
 * @brief Test Fixture meant to ensure correct behavior of RingBufferPad.
 * @note A RingBuffer is used to implement the different tested functions
 */
class RingBufferPadTest : public ::testing::Test {

protected:
    RingBufferPadTest() = default;

    virtual ~RingBufferPadTest() = default;
};


TEST_F(RingBufferPadTest, given_aNewPad_when_fetchingNextPad_then_returnsSelf) {
    auto initialPad = Pad();

    auto testedNextPad = initialPad.next();

    ASSERT_EQ(testedNextPad, &initialPad);
}


TEST_F(RingBufferPadTest, given_aNextPad_when_fetchingNextPad_then_returnsTheNextPad) {
    auto initialPad = Pad();
    auto testedPad = Pad();
    auto nextPadPointer = &initialPad;
    testedPad.setNext(nextPadPointer);

    auto testedNextPad = testedPad.next();

    ASSERT_EQ(testedNextPad, nextPadPointer);
}

TEST_F(RingBufferPadTest, given_aNextPad_when_attemptingToSetANewNextPad_then_theNewNextPadIsNotAdded) {
    auto initialPad = Pad();
    auto ignoredPad = Pad();
    auto testedPad = Pad();
    auto nextPadPointer = &initialPad;
    testedPad.setNext(nextPadPointer);

    testedPad.setNext(&ignoredPad);

    auto testedNextPad = testedPad.next();
    ASSERT_EQ(testedNextPad, nextPadPointer);
}


TEST_F(RingBufferPadTest, given_data_when_read_then_returnsData) {
    auto testedPad = Pad();
    auto data = DataTestUtil::createRandomSimpleMessageWithEmptyTimestamps();
    using DataType = decltype(data);
    auto dataCopy = DataType(data);
    testedPad.write(std::move(data));

    auto readData = testedPad.read();

    ASSERT_EQ(readData, dataCopy);
}

#endif //SENSORGATEWAY_RINGBUFFERPADTEST_CPP

