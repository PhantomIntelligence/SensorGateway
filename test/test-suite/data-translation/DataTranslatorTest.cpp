/**
	Copyright 2014-2019 Phantom Intelligence Inc.

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

#include <gtest/gtest.h>

#include "sensor-gateway/data-translation/DataTranslator.hpp"
#include "test/utilities/data-model/DataModelFixture.h"

#include "test/utilities/mock/ArbitraryDataSinkMock.hpp"
#include "test/utilities/mock/ErrorThrowingDataTranslationStrategyMock.hpp"


class DataTranslatorTest : public ::testing::Test {

public :

    using Structures = Sensor::Test::Simple::Structures;

protected:

    using DataTestUtil = TestFunctions::DataTestUtil;
    using DataTranslator = SensorAccessLinkElement::DataTranslator<Structures, Structures>;

    using Error = ErrorHandling::SensorAccessLinkError;
    using ErrorSinkMock = Mock::ArbitraryDataSinkMock<Error>;
    using ThrowingDataTranslationStrategy = Mock::ErrorThrowingDataTranslationStrategyMock<Sensor::Test::Simple::Structures>;

    DataTranslatorTest() = default;

    ~DataTranslatorTest() override = default;

};

namespace DataTranslatorTestMock {

    using Structures = DataTranslatorTest::Structures;
    using DataTranslationStrategy = DataTranslation::DataTranslationStrategy<Structures, Structures>;
    class MockDataTranslationStrategy final :
            public DataTranslationStrategy {

    protected:

        using super = DataTranslationStrategy;
        using super::SensorMessage;
        using super::SensorRawData;

        using super::MessageSource;
        using super::RawDataSource;

    public:

        MockDataTranslationStrategy() :
                translateMessageCalled(false),
                translateRawDataCalled(false),
                receivedSensorMessage(Structures::Message::returnDefaultData()),
                receivedSensorRawData(Structures::RawData::returnDefaultData()) {};

        ~MockDataTranslationStrategy() noexcept = default;

        void translateMessage(super::SensorMessage&& sensorMessage) override {
            receivedSensorMessage = sensorMessage;
            translateMessageCalled.store(true);
        };

        void translateRawData(super::SensorRawData&& sensorRawData) override {
            receivedSensorRawData = sensorRawData;
            translateRawDataCalled.store(true);
        }

        super::SensorMessage
        translateControlMessageToSensorMessageRequest(
                super::ParameterControlMessage&& parameterControlMessage) override {
            return super::SensorMessage::returnDefaultData();
        }

        super::ParameterControlMessage
        translateSensorMessageToControlMessageResult(super::SensorMessage&& sensorMessage) override {
            return super::ParameterControlMessage::returnDefaultData();
        }

        bool hasTranslateMessageBeenCalledWithRightSensorMessage(
                super::SensorMessage const& expectedSensorMessage) const {
            return (translateMessageCalled.load() && (expectedSensorMessage == receivedSensorMessage));
        };

        bool hasTranslateRawDataBeenCalledWithRightSensorRawData(
                super::SensorRawData const& expectedSensorRawData) const {
            return (translateRawDataCalled.load() && (expectedSensorRawData == receivedSensorRawData));

        };

    private:

        AtomicFlag translateMessageCalled;
        AtomicFlag translateRawDataCalled;

        super::SensorMessage receivedSensorMessage;
        super::SensorRawData receivedSensorRawData;

    };
}

TEST_F(DataTranslatorTest,
       given_aTranslationStrategy_when_consumingMessage_then_callsTranslateMessageInStrategyWithTheConsumedMessage) {
    auto message = DataTestUtil::createRandomSimpleMessageWithEmptyTimestamps();
    auto copy = Structures::Message(message);

    DataTranslatorTestMock::MockDataTranslationStrategy mockStrategy;
    DataTranslator dataTranslator(&mockStrategy);

    dataTranslator.consume(std::move(message));
    auto strategyCalledWithTheRightMessage = mockStrategy.hasTranslateMessageBeenCalledWithRightSensorMessage(copy);

    ASSERT_TRUE(strategyCalledWithTheRightMessage);
}

TEST_F(DataTranslatorTest,
       given_aTranslationStrategy_when_consumingRawData_then_callsTranslateRawDataInStrategyWithTheConsumedRawData) {
    auto data = DataTestUtil::createRandomSimpleRawData();
    auto copy = Structures::RawData(data);

    DataTranslatorTestMock::MockDataTranslationStrategy mockStrategy;
    DataTranslator dataTranslator(&mockStrategy);

    dataTranslator.consume(std::move(data));
    auto strategyCalledWithTheRightRawData = mockStrategy.hasTranslateRawDataBeenCalledWithRightSensorRawData(copy);

    ASSERT_TRUE(strategyCalledWithTheRightRawData);
}

TEST_F(DataTranslatorTest,
       given_aThrowingTranslationStrategy_when_consumingMessage_then_producesAnErrorCorrectlyFormatted) {
    auto numberOfErrorToReceive = 1;
    Mock::ArbitraryDataSinkMock<Error> sink(numberOfErrorToReceive);
    DataFlow::DataProcessingScheduler<Error, ErrorSinkMock, 1> scheduler(&sink);

    ThrowingDataTranslationStrategy throwingMockStrategy;
    DataTranslator dataTranslator(&throwingMockStrategy);

    dataTranslator.linkConsumer(&scheduler);
    auto data = DataTestUtil::createRandomSimpleMessageWithEmptyTimestamps();
    dataTranslator.consume(std::move(data));
    sink.waitConsumptionToBeReached();

    scheduler.terminateAndJoin();

    auto producedErrors = sink.getConsumedData();

    Error expectedError = ErrorHandling::SensorAccessLinkError(
            ErrorHandling::Origin::TRANSLATE_MESSAGE + ErrorHandling::Message::SEPARATOR + throwingMockStrategy.ORIGIN,
            ErrorHandling::Category::TRANSLATION_ERROR,
            ErrorHandling::Severity::ERROR,
            throwingMockStrategy.ERROR_CODE,
            throwingMockStrategy.MESSAGE_ERROR_MESSAGE);
    for (auto t = 0; t < numberOfErrorToReceive; ++t) {
        ASSERT_EQ(producedErrors.front(), expectedError);
        producedErrors.pop_front();
    }
}

TEST_F(DataTranslatorTest,
       given_aThrowingTranslationStrategy_when_consumingRawData_then_producesAnErrorCorrectlyFormatted) {
    auto numberOfErrorToReceive = 1;
    Mock::ArbitraryDataSinkMock<Error> sink(numberOfErrorToReceive);
    DataFlow::DataProcessingScheduler<Error, ErrorSinkMock, 1> scheduler(&sink);

    ThrowingDataTranslationStrategy throwingMockStrategy;
    DataTranslator dataTranslator(&throwingMockStrategy);

    Error expectedError = ErrorHandling::SensorAccessLinkError(
            ErrorHandling::Origin::TRANSLATE_RAWDATA + ErrorHandling::Message::SEPARATOR + throwingMockStrategy.ORIGIN,
            ErrorHandling::Category::TRANSLATION_ERROR,
            ErrorHandling::Severity::ERROR,
            throwingMockStrategy.ERROR_CODE,
            throwingMockStrategy.RAW_DATA_ERROR_MESSAGE);

    auto data = DataTestUtil::createRandomSimpleRawData();

    dataTranslator.linkConsumer(&scheduler);
    data = DataTestUtil::createRandomSimpleRawData();
    dataTranslator.consume(std::move(data));
    sink.waitConsumptionToBeReached();

    scheduler.terminateAndJoin();

    auto producedErrors = sink.getConsumedData();

    for (auto t = 0; t < numberOfErrorToReceive; ++t) {
        ASSERT_EQ(producedErrors.front(), expectedError);
        producedErrors.pop_front();
    }
}
