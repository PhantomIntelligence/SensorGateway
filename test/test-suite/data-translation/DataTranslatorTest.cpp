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


using TestFunctions::DataTestUtil;
using Sensor::Test::Simple::Structures;
using DataTranslation::DataTranslationStrategy;
using SensorAccessLinkElement::DataTranslator;

class DataTranslatorTest : public ::testing::Test {

protected:

    DataTranslatorTest() = default;

    ~DataTranslatorTest() override = default;
};

namespace DataTranslatorTestMock {

    class MockDataTranslationStrategy final
            : public DataTranslationStrategy<Structures::Message, Structures::Message> {

    protected:

        using super = DataTranslationStrategy<Structures::Message, Structures::Message>;

    public:

        MockDataTranslationStrategy() : translateMessageCalled(false),
                                           receivedInputMessage(Structures::Message::returnDefaultData()) {};

        void translateMessage(super::INPUT&& inputMessage) override {
            receivedInputMessage = inputMessage;
            translateMessageCalled.store(true);
        };

        bool hasTranslateMessageBeenCalledWithRightInputMessage(super::INPUT expectedInputMessage) const {

            return (translateMessageCalled.load() && (expectedInputMessage == receivedInputMessage));

        };

    private:

        AtomicFlag translateMessageCalled;
        super::INPUT receivedInputMessage;

    };
}

TEST_F(DataTranslatorTest,
       given_aTranslationStrategy_when_consumingMessage_then_callsTranslateMessageInStrategyWithTheConsumedMessage) {
    auto message = DataTestUtil::createRandomSimpleMessage();
    auto copy = Structures::Message(message);

    DataTranslatorTestMock::MockDataTranslationStrategy mockStrategy;
    DataTranslator<Structures::Message, Structures::Message> dataTranslator(&mockStrategy);

    dataTranslator.consume(std::move(message));
    auto strategyCalledWithTheRightMessage = mockStrategy.hasTranslateMessageBeenCalledWithRightInputMessage(copy);

    ASSERT_TRUE(strategyCalledWithTheRightMessage);
}

//TEST_F(DataTranslatorTest,
//       given_aTranslationStrategy_when_consumingRawData_then_callsTranslateRawDataInStrategyWithTheConsumedRawData) {
//    auto data = DataTestUtil::createRandomSimpleRawData();
//    auto copy = Structures::RawData(data);
//
//    DataTranslatorTestMock::MockDataTranslationStrategy mockStrategy;
//    DataTranslator<Structures::RawData, Structures::RawData> dataTranslator(&mockStrategy);
//
//    dataTranslator.consume(std::move(data));
//    auto strategyCalledWithTheRightMessage = mockStrategy.hasTranslateMessageBeenCalledWithRightInputMessage(copy);
//
//    ASSERT_TRUE(strategyCalledWithTheRightMessage);
//}
