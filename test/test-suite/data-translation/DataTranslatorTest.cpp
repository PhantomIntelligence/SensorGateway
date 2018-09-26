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

#include <gtest/gtest.h>

#include "sensor-gateway/data-translation/DataTranslator.hpp"
#include "test/utilities/data-model/DataModelFixture.h"

#include "test/utilities/mock/ArbitraryDataSinkMock.hpp"

using TestFunctions::DataTestUtil;
using Sensor::Test::Simple::Structures;
using DataTranslation::DataTranslationStrategy;
using SensorAccessLinkElement::DataTranslator;

class DataTranslatorTest : public ::testing::Test {

protected:

    using Error = ErrorHandling::SensorAccessLinkError;
    using ErrorSinkMock = Mock::ArbitraryDataSinkMock<Error>;

    DataTranslatorTest() = default;

    ~DataTranslatorTest() override = default;
};

namespace DataTranslatorTestMock {

    class MockDataTranslationStrategy final :
            public DataTranslationStrategy<Structures, Structures> {

    protected:

        using super = DataTranslation::DataTranslationStrategy<Structures, Structures>;
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
    auto message = DataTestUtil::createRandomSimpleMessage();
    auto copy = Structures::Message(message);

    DataTranslatorTestMock::MockDataTranslationStrategy mockStrategy;
    DataTranslator<Structures, Structures> dataTranslator(&mockStrategy);

    dataTranslator.consume(std::move(message));
    auto strategyCalledWithTheRightMessage = mockStrategy.hasTranslateMessageBeenCalledWithRightSensorMessage(copy);

    ASSERT_TRUE(strategyCalledWithTheRightMessage);
}

TEST_F(DataTranslatorTest,
       given_aTranslationStrategy_when_consumingRawData_then_callsTranslateRawDataInStrategyWithTheConsumedRawData) {
    auto data = DataTestUtil::createRandomSimpleRawData();
    auto copy = Structures::RawData(data);

    DataTranslatorTestMock::MockDataTranslationStrategy mockStrategy;
    DataTranslator<Structures, Structures> dataTranslator(&mockStrategy);

    dataTranslator.consume(std::move(data));
    auto strategyCalledWithTheRightRawData = mockStrategy.hasTranslateRawDataBeenCalledWithRightSensorRawData(copy);

    ASSERT_TRUE(strategyCalledWithTheRightRawData);
}

namespace DataTranslatorTestMock {

    class ThrowingDataTranslationStrategy final :
            public DataTranslationStrategy<Structures, Structures> {

    protected:

        using super = DataTranslation::DataTranslationStrategy<Structures, Structures>;
        using super::SensorMessage;
        using super::SensorRawData;

        using super::MessageSource;
        using super::RawDataSource;

    public:

        ThrowingDataTranslationStrategy() = default;

        ~ThrowingDataTranslationStrategy() noexcept = default;

        void translateMessage(super::SensorMessage&& sensorMessage) override {
            std::cout << "sensorMessage.toString() : " << sensorMessage.toString() << std::endl;
            throw ErrorHandling::SensorAccessLinkError(origin,
                                                       ErrorHandling::Category::EMPTY_CATEGORY,
                                                       ErrorHandling::Severity::EMPTY_SEVERITY,
                                                       errorCode,
                                                       messageErrorMessage);
        };

        void translateRawData(super::SensorRawData&& sensorRawData) override {
            throw ErrorHandling::SensorAccessLinkError(origin,
                                                       ErrorHandling::Category::EMPTY_CATEGORY,
                                                       ErrorHandling::Severity::EMPTY_SEVERITY,
                                                       errorCode,
                                                       rawDataErrorMessage);
        }

        std::string const origin = "from the throwing strategy";
        ErrorHandling::ErrorCode const errorCode = ErrorHandling::GatewayErrorCode::DATA_NOT_RECOGNIZED;
        std::string const messageErrorMessage = "Error message from the message consumption strategy";
        std::string const rawDataErrorMessage = "Error message from the rawdata consumption strategy";
    };
}

TEST_F(DataTranslatorTest,
       given_aThrowingTranslationStrategy_when_consumingMessage_then_producesAnErrorCorrectlyFormatted) {
    auto numberOfErrorToReceive = 1;
    Mock::ArbitraryDataSinkMock<Error> sink(numberOfErrorToReceive);
    DataFlow::DataProcessingScheduler<Error, ErrorSinkMock, 1> scheduler(&sink);

    DataTranslatorTestMock::ThrowingDataTranslationStrategy throwingMockStrategy;
    DataTranslator<Structures, Structures> dataTranslator(&throwingMockStrategy);

    dataTranslator.linkConsumer(&scheduler);
    auto data = DataTestUtil::createRandomSimpleMessage();
    dataTranslator.consume(std::move(data));
    sink.waitConsumptionToBeReached();

    scheduler.terminateAndJoin();

    auto producedErrors = sink.getConsumedData();

    Error expectedError = ErrorHandling::SensorAccessLinkError(
            ErrorHandling::Origin::TRANSLATE_MESSAGE + ErrorHandling::Message::SEPARATOR + throwingMockStrategy.origin,
            ErrorHandling::Category::TRANSLATION_ERROR,
            ErrorHandling::Severity::ERROR,
            throwingMockStrategy.errorCode,
            throwingMockStrategy.messageErrorMessage);
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

    DataTranslatorTestMock::ThrowingDataTranslationStrategy throwingMockStrategy;
    DataTranslator<Structures, Structures> dataTranslator(&throwingMockStrategy);

    Error expectedError = ErrorHandling::SensorAccessLinkError(
            ErrorHandling::Origin::TRANSLATE_RAWDATA + ErrorHandling::Message::SEPARATOR + throwingMockStrategy.origin,
            ErrorHandling::Category::TRANSLATION_ERROR,
            ErrorHandling::Severity::ERROR,
            throwingMockStrategy.errorCode,
            throwingMockStrategy.rawDataErrorMessage);

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
