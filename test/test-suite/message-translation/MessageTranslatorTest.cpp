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
#include <sensor-gateway/domain/AWLMessage.h>
#include <sensor-gateway/domain/Frame.h>
#include "sensor-gateway/message-translation/MessageTranslator.hpp"

using DataFlow::AWLMessage;
using DataFlow::Frame;
using MessageTranslation::MessageTranslationStrategy;
using SensorAccessLinkElement::MessageTranslator;

class MessageTranslatorTest : public ::testing::Test {

protected:

    MessageTranslatorTest() = default;

    virtual ~MessageTranslatorTest() = default;
};

class MockMessageTranslationStrategy final : public MessageTranslationStrategy<AWLMessage, Frame> {

public:

    MockMessageTranslationStrategy() : translateMessageCalled(false),
                                       receivedInputMessage(AWLMessage::returnDefaultData()) {};

    void translateMessage(AWLMessage&& inputMessage) override {
        receivedInputMessage = inputMessage;
        translateMessageCalled.store(true);

    };

    bool hasTranslateMessageBeenCalledWithRightInputMessage(AWLMessage expectedInputMessage) const {

        return (translateMessageCalled.load() && (expectedInputMessage == receivedInputMessage));

    };


private:

    AtomicFlag translateMessageCalled;
    AWLMessage receivedInputMessage;

};

TEST_F(MessageTranslatorTest,
       given_aMessageTranslationStrategy_when_consumingAAWLMessage_then_callsTranslateMessageInStrategy) {
    AWLMessage awlMessage = AWLMessage::returnDefaultData();

    MockMessageTranslationStrategy mockStrategy;
    MessageTranslator<AWLMessage,Frame> messageTranslator(&mockStrategy);

    messageTranslator.consume(std::move(awlMessage));
    auto strategyHasBeenCalledWithRightParameter = mockStrategy.hasTranslateMessageBeenCalledWithRightInputMessage(awlMessage);
    ASSERT_TRUE(strategyHasBeenCalledWithRightParameter);
}
