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


#ifndef SPIRITSENSORGATEWAY_AWLMESSAGETRANSLATORTEST_CPP
#define SPIRITSENSORGATEWAY_AWLMESSAGETRANSLATORTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "test/mock/FrameSinkMock.h"
#include "spirit-sensor-gateway/message-translation/AWLMessageToSpiritMessageTranslationStrategy.h"

using Defaults::Track::DEFAULT_ACCELERATION;
using Defaults::Track::DEFAULT_DISTANCE;
using Defaults::Track::DEFAULT_SPEED;
using MessageTranslation::AWLMessageToSpiritMessageTranslationStrategy;
using Mock::FrameProcessingScheduler;
using Mock::FrameSinkMock;
using AWL::DataArray;

class AWLMessageToSpiritMessageTranslationStrategyTest : public ::testing::Test {
protected:
    unsigned int const INDEX_OF_FRAME = 0;

    Frame frameAfterEndOfFrameMessageTranslationStub = Frame(64829, 16, {});

    Frame frameAfterDetectionTrackAndEndOfFrameMessagesTranslationStub =
            Frame(64829, 16, {
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel(11, {Track(14291, 96, 379, DEFAULT_ACCELERATION, DEFAULT_DISTANCE, DEFAULT_SPEED)}, 1)
            });

    Frame frameAfterDetectionTrackAndVelocityTrackAndEndOfFrameMessagesTranslationStub =
            Frame(64829, 16, {
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel::returnDefaultData(),
                    Pixel(11, {Track(14291, 96, 379, 256, 106, 0)}, 1)
            });

    AWLMessage createDetectionTrackAWLMessage() {
        return AWLMessage(10, 2188169, 8, {211, 55, 0, 11, 0, 96, 123, 1});
    }

    AWLMessage createDetectionVelocityAWLMessage() {
        return AWLMessage(11, 2188169, 8, {211, 55, 106, 0, 0, 0, 0, 1});
    }

    AWLMessage createEndOfFrameAWLMessage() {
        return AWLMessage(9, 2188170, 8, {61, 253, 16, 0, 0, 0, 0, 0});
    }

};

TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,
       given_someEndOfFrameAWLMessage_when_translatingOnlyThisMessage_then_buildsFrameWithValidAttributes) {

    auto endOfFrameAWLMessage = createEndOfFrameAWLMessage();
    auto expectedFrame = frameAfterEndOfFrameMessageTranslationStub;

    AWLMessageToSpiritMessageTranslationStrategy translationStrategy;
    FrameSinkMock frameSinkMock(1);
    FrameProcessingScheduler scheduler(&frameSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateBasicMessage(std::move(endOfFrameAWLMessage));

    scheduler.terminateAndJoin();
    auto actualFrame = frameSinkMock.getConsumedData().at(INDEX_OF_FRAME);

    ASSERT_EQ(expectedFrame, actualFrame);
}

TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,
       given_someDetectionTrackAndEndOfFrameAWLMessages_when_translatingThoseMessagesInGivenOrder_then_builtFrameContainsTheCorrectTrackInTheCorrectPixel) {

    auto detectionTrackAWLMessage = createDetectionTrackAWLMessage();
    auto endOfFrameAWLMessage = createEndOfFrameAWLMessage();
    auto expectedFrame = frameAfterDetectionTrackAndEndOfFrameMessagesTranslationStub;

    AWLMessageToSpiritMessageTranslationStrategy translationStrategy;
    FrameSinkMock frameSinkMock(1);
    FrameProcessingScheduler scheduler(&frameSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateBasicMessage(std::move(detectionTrackAWLMessage));
    translationStrategy.translateBasicMessage(std::move(endOfFrameAWLMessage));

    scheduler.terminateAndJoin();

    auto actualFrame = frameSinkMock.getConsumedData().at(INDEX_OF_FRAME);

    ASSERT_EQ(expectedFrame, actualFrame);
}

TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,
       given_someDetectionTrackAndDetectionVelocityAndEndOfFrameAWLMessages_when_translatingThoseMessagesInGivenOrder_then_buildsFrameWithValidAttribute) {

    auto detectionTrackAWLMessage = createDetectionTrackAWLMessage();
    auto detectionVelocityAWLMessage = createDetectionVelocityAWLMessage();
    auto endOfFrameAWLMessage = createEndOfFrameAWLMessage();
    auto expectedFrame = frameAfterDetectionTrackAndVelocityTrackAndEndOfFrameMessagesTranslationStub;

    AWLMessageToSpiritMessageTranslationStrategy translationStrategy;
    FrameSinkMock frameSinkMock(1);
    FrameProcessingScheduler scheduler(&frameSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateBasicMessage(std::move(detectionTrackAWLMessage));
    translationStrategy.translateBasicMessage(std::move(detectionVelocityAWLMessage));
    translationStrategy.translateBasicMessage(std::move(endOfFrameAWLMessage));

    scheduler.terminateAndJoin();
    auto actualFrame = frameSinkMock.getConsumedData().at(INDEX_OF_FRAME);

    ASSERT_EQ(expectedFrame, actualFrame);
}

TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,
       given_someEndOfFrameAWLMessage_when_translatingThisMessage_then_callsProduceOneTime) {

    auto endOfFrameAWLMessage = createEndOfFrameAWLMessage();

    AWLMessageToSpiritMessageTranslationStrategy translationStrategy;
    FrameSinkMock frameSinkMock(1);
    FrameProcessingScheduler scheduler(&frameSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateBasicMessage(std::move(endOfFrameAWLMessage));

    scheduler.terminateAndJoin();

    ASSERT_EQ(frameSinkMock.hasBeenCalledExpectedNumberOfTimes(), 1);
}

#endif //SPIRITSENSORGATEWAY_AWLMESSAGETRANSLATORTEST_CPP
