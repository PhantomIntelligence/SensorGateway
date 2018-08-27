#ifndef SENSORGATEWAY_RINGBUFFERTEST_CPP
#define SENSORGATEWAY_RINGBUFFERTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "sensor-gateway/common/data-flow/RingBuffer.hpp"
#include "test/utilities/data-model/DataModelFixture.h"
#include "test/utilities/mock/ConsumerLinkMock.h"

using DataModel::SimpleMessage;
using TestFunctions::DataTestUtil;

using SimpleBuffer = DataFlow::RingBuffer<SimpleMessage>;
using MockConsumerLink = Mock::ConsumerLinkMock<SimpleMessage>;

class RingBufferTest : public ::testing::Test {

protected:
    RingBufferTest();

    virtual ~RingBufferTest();

};

RingBufferTest::RingBufferTest() {
}

RingBufferTest::~RingBufferTest() {}


SimpleMessage createRandomSimpleData() {
    auto simpleData = DataTestUtil::createRandomSimpleMessage();
    while (simpleData == SimpleMessage::returnDefaultData()) {
        simpleData = DataTestUtil::createRandomSimpleMessage();
    }
    return simpleData;
}


TEST_F(RingBufferTest, given_aData_when_consumeNextDataForALink_then_returnsData) {
    SimpleBuffer simpleBuffer;
    auto linkMock = MockConsumerLink();
    auto simpleData = createRandomSimpleData();
    auto simpleDataCopy = SimpleMessage(simpleData);
    simpleBuffer.write(std::move(simpleData));

    auto consumedData = simpleBuffer.consumeNextDataFor(&linkMock);

    ASSERT_EQ(simpleDataCopy, consumedData);
}

TEST_F(RingBufferTest, given_twoDataOfWhichOneHasAlreadyBeenConsumed_when_consumeNextDataForALink_then_returnsTheSecondData) {
    SimpleBuffer simpleBuffer;
    auto linkMock = MockConsumerLink();
    auto simpleDataOne = createRandomSimpleData();
    auto simpleDataTwo = createRandomSimpleData();
    auto simpleDataTwoCopy = SimpleMessage(simpleDataTwo);
    simpleBuffer.write(std::move(simpleDataOne));
    simpleBuffer.write(std::move(simpleDataTwo));
    simpleBuffer.consumeNextDataFor(&linkMock);

    auto consumedData = simpleBuffer.consumeNextDataFor(&linkMock);

    ASSERT_EQ(simpleDataTwoCopy, consumedData);
}

TEST_F(RingBufferTest, given_noData_when_consumeNextDataForALink_then_throwsARuntimeException) {
    SimpleBuffer simpleBuffer;
    auto linkMock = MockConsumerLink();

    EXPECT_THROW(simpleBuffer.consumeNextDataFor(&linkMock), std::runtime_error);
}

TEST_F(RingBufferTest, given_aDataAndTwoConsumers_when_consumeNextDataForALinkAfterTheOtherOne_then_returnsTheData) {
    SimpleBuffer simpleBuffer;
    auto firstLinkMock = MockConsumerLink();
    auto secondLinkMock = MockConsumerLink();
    auto simpleData = createRandomSimpleData();
    auto simpleDataCopy = SimpleMessage(simpleData);
    simpleBuffer.write(std::move(std::move(simpleData)));

    simpleBuffer.consumeNextDataFor(&firstLinkMock);
    auto consumedData = simpleBuffer.consumeNextDataFor(&secondLinkMock);

    ASSERT_EQ(simpleDataCopy, consumedData);

}

TEST_F(RingBufferTest, given_aLinkedConsumer_when_writesData_then_activatesConsumer) {
    SimpleBuffer simpleBuffer;
    auto linkMock = MockConsumerLink();
    simpleBuffer.linkWith(&linkMock);

    auto simpleData = createRandomSimpleData();
    simpleBuffer.write(std::move(std::move(simpleData)));

    ASSERT_TRUE(linkMock.isActive());
}

TEST_F(RingBufferTest, given_dataAndAConsumer_when_consumesTheLastDataForThatConsumer_then_deactivatesConsumer) {
    SimpleBuffer simpleBuffer;
    auto linkMock = MockConsumerLink();
    simpleBuffer.linkWith(&linkMock);
    auto simpleData = createRandomSimpleData();
    simpleBuffer.write(std::move(std::move(simpleData)));

    simpleBuffer.consumeNextDataFor(&linkMock);

    ASSERT_FALSE(linkMock.isActive());
}

TEST_F(RingBufferTest, given_twoDataAndAConsumerThatConsumedThemAll_when_consumesNextDataForANewConsumer_then_returnsTheFirstData) {
    SimpleBuffer simpleBuffer;
    auto firstLinkMock = MockConsumerLink();
    auto simpleDataOne = createRandomSimpleData();
    auto simpleDataTwo = createRandomSimpleData();
    auto simpleDataOneCopy = SimpleMessage(simpleDataOne);
    auto simpleDataTwoCopy = SimpleMessage(simpleDataTwo);
    simpleBuffer.write(std::move(simpleDataOne));
    simpleBuffer.write(std::move(simpleDataTwo));
    simpleBuffer.consumeNextDataFor(&firstLinkMock);
    simpleBuffer.consumeNextDataFor(&firstLinkMock);

    auto newLinkMock = MockConsumerLink();
    auto consumedData = simpleBuffer.consumeNextDataFor(&newLinkMock);

    ASSERT_EQ(simpleDataOneCopy, consumedData);
}

TEST_F(RingBufferTest, given_aFullBuffer_when_writesOneNewData_then_overwritesTheFirstData) {
    SimpleBuffer simpleBuffer;
    auto firstData = createRandomSimpleData();
    auto firstDataCopy = SimpleMessage(firstData);
    simpleBuffer.write(std::move(firstData));
    for (auto i = 0; i < RING_BUFFER_SIZE; ++i) {
        simpleBuffer.write(createRandomSimpleData());
    }

    auto newLinkMock = MockConsumerLink();
    auto lastWrittenData = simpleBuffer.consumeNextDataFor(&newLinkMock);

    ASSERT_NE(firstDataCopy, lastWrittenData);
}

TEST_F(RingBufferTest, given_aFullBuffer_when_writesMoreThanOneData_then_overwritesTheOldestDataEachTime) {
    SimpleBuffer simpleBuffer;
    auto firstData = createRandomSimpleData();
    auto secondData = createRandomSimpleData();
    auto secondDataCopy = SimpleMessage(firstData);
    simpleBuffer.write(std::move(firstData));
    simpleBuffer.write(std::move(secondData));
    for (auto i = 0; i < RING_BUFFER_SIZE; ++i) {
        simpleBuffer.write(createRandomSimpleData());
    }

    auto newLinkMock = MockConsumerLink();
    simpleBuffer.consumeNextDataFor(&newLinkMock);
    auto lastWrittenData = simpleBuffer.consumeNextDataFor(&newLinkMock);

    ASSERT_NE(secondDataCopy, lastWrittenData);
}

#endif //SENSORGATEWAY_RINGBUFFERTEST_CPP
