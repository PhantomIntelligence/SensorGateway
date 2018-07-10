#ifndef SPIRITSENSORGATEWAY_RINGBUFFERTEST_CPP
#define SPIRITSENSORGATEWAY_RINGBUFFERTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <spirit-sensor-gateway/domain/AWLMessage.h>
#include <spirit-sensor-gateway/sensor-communication/SensorCommunicator.hpp>
#include <spirit-sensor-gateway/sensor-communication/KvaserCanProtocolStrategy.h>

#include "spirit-sensor-gateway/common/buffer/RingBuffer.hpp"
#include "data-model/DataModelFixture.h"
#include "MockConsumerLink.h"

using ExampleDataModel::NativeData;
using ExampleDataModel::ProcessedData;
using ExampleDataModel::Data;
using TestFunctions::DataTestUtil;

using NativeBuffer = DataFlow::RingBuffer<NativeData>;
using MockConsumerLink = Mock::MockConsumerLink<NativeData>;

class RingBufferTest : public ::testing::Test {

protected:
    RingBufferTest();

    virtual ~RingBufferTest();

};

RingBufferTest::RingBufferTest() {
}

RingBufferTest::~RingBufferTest() {}


NativeData generateRandomNativeData() {
    auto nativeData = DataTestUtil::generateRandomNativeData();
    while (nativeData == NativeData::returnDefaultData()) {
        nativeData = DataTestUtil::generateRandomNativeData();
    }
    return nativeData;
}


TEST_F(RingBufferTest, given_aData_when_consumeNextDataForALink_then_returnsData) {
    NativeBuffer nativeBuffer;
    auto linkMock = MockConsumerLink();
    auto nativeData = generateRandomNativeData();
    auto nativeDataCopy = NativeData(nativeData);
    nativeBuffer.write(std::move(nativeData));

    auto consumedData = nativeBuffer.consumeNextDataFor(&linkMock);

    ASSERT_EQ(nativeDataCopy, consumedData);
}

TEST_F(RingBufferTest, given_twoDataOfWhichOneHasAlreadyBeenConsumed_when_consumeNextDataForALink_then_returnsTheSecondData) {
    NativeBuffer nativeBuffer;
    auto linkMock = MockConsumerLink();
    auto nativeDataOne = generateRandomNativeData();
    auto nativeDataTwo = generateRandomNativeData();
    auto nativeDataTwoCopy = NativeData(nativeDataTwo);
    nativeBuffer.write(std::move(nativeDataOne));
    nativeBuffer.write(std::move(nativeDataTwo));
    nativeBuffer.consumeNextDataFor(&linkMock);

    auto consumedData = nativeBuffer.consumeNextDataFor(&linkMock);

    ASSERT_EQ(nativeDataTwoCopy, consumedData);
}

TEST_F(RingBufferTest, given_noData_when_consumeNextDataForALink_then_throwsARuntimeException) {
    NativeBuffer nativeBuffer;
    auto linkMock = MockConsumerLink();

    EXPECT_THROW(nativeBuffer.consumeNextDataFor(&linkMock), std::runtime_error);
}

TEST_F(RingBufferTest, given_aDataAndTwoConsumers_when_consumeNextDataForALinkAfterTheOtherOne_then_returnsTheData) {
    NativeBuffer nativeBuffer;
    auto firstLinkMock = MockConsumerLink();
    auto secondLinkMock = MockConsumerLink();
    auto nativeData = generateRandomNativeData();
    auto nativeDataCopy = NativeData(nativeData);
    nativeBuffer.write(std::move(std::move(nativeData)));

    nativeBuffer.consumeNextDataFor(&firstLinkMock);
    auto consumedData = nativeBuffer.consumeNextDataFor(&secondLinkMock);

    ASSERT_EQ(nativeDataCopy, consumedData);

}

TEST_F(RingBufferTest, given_aLinkedConsumer_when_writesData_then_activatesConsumer) {
    NativeBuffer nativeBuffer;
    auto linkMock = MockConsumerLink();
    nativeBuffer.linkWith(&linkMock);

    auto nativeData = generateRandomNativeData();
    nativeBuffer.write(std::move(std::move(nativeData)));

    ASSERT_TRUE(linkMock.isActive());
}

TEST_F(RingBufferTest, given_dataAndAConsumer_when_consumesTheLastDataForThatConsumer_then_deactivatesConsumer) {
    NativeBuffer nativeBuffer;
    auto linkMock = MockConsumerLink();
    nativeBuffer.linkWith(&linkMock);
    auto nativeData = generateRandomNativeData();
    nativeBuffer.write(std::move(std::move(nativeData)));

    nativeBuffer.consumeNextDataFor(&linkMock);

    ASSERT_FALSE(linkMock.isActive());
}

TEST_F(RingBufferTest, given_twoDataAndAConsumerThatConsumedThemAll_when_consumesNextDataForANewConsumer_then_returnsTheFirstData) {
    NativeBuffer nativeBuffer;
    auto firstLinkMock = MockConsumerLink();
    auto nativeDataOne = generateRandomNativeData();
    auto nativeDataTwo = generateRandomNativeData();
    auto nativeDataOneCopy = NativeData(nativeDataOne);
    auto nativeDataTwoCopy = NativeData(nativeDataTwo);
    nativeBuffer.write(std::move(nativeDataOne));
    nativeBuffer.write(std::move(nativeDataTwo));
    nativeBuffer.consumeNextDataFor(&firstLinkMock);
    nativeBuffer.consumeNextDataFor(&firstLinkMock);

    auto newLinkMock = MockConsumerLink();
    auto consumedData = nativeBuffer.consumeNextDataFor(&newLinkMock);

    ASSERT_EQ(nativeDataOneCopy, consumedData);
}

TEST_F(RingBufferTest, given_aFullBuffer_when_writesOneNewData_then_overwritesTheFirstData) {
    NativeBuffer nativeBuffer;
    auto firstData = generateRandomNativeData();
    auto firstDataCopy = NativeData(firstData);
    nativeBuffer.write(std::move(firstData));
    for (auto i = 0; i < RING_BUFFER_SIZE; ++i) {
        nativeBuffer.write(generateRandomNativeData());
    }

    auto newLinkMock = MockConsumerLink();
    auto lastWrittenData = nativeBuffer.consumeNextDataFor(&newLinkMock);

    ASSERT_NE(firstDataCopy, lastWrittenData);
}

TEST_F(RingBufferTest, given_aFullBuffer_when_writesMoreThanOneData_then_overwritesTheOldestDataEachTime) {
    NativeBuffer nativeBuffer;
    auto firstData = generateRandomNativeData();
    auto secondData = generateRandomNativeData();
    auto secondDataCopy = NativeData(firstData);
    nativeBuffer.write(std::move(firstData));
    nativeBuffer.write(std::move(secondData));
    for (auto i = 0; i < RING_BUFFER_SIZE; ++i) {
        nativeBuffer.write(generateRandomNativeData());
    }

    auto newLinkMock = MockConsumerLink();
    nativeBuffer.consumeNextDataFor(&newLinkMock);
    auto lastWrittenData = nativeBuffer.consumeNextDataFor(&newLinkMock);

    ASSERT_NE(secondDataCopy, lastWrittenData);
}

#endif //SPIRITSENSORGATEWAY_RINGBUFFERTEST_CPP
