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
#include "test/utilities/mock/FrameSinkMock.h"
#include "spirit-sensor-gateway/message-translation/AWLMessageToSpiritMessageTranslationStrategy.h"

using Defaults::Track::DEFAULT_ACCELERATION;
using Defaults::Track::DEFAULT_DISTANCE;
using Defaults::Track::DEFAULT_SPEED;
using MessageTranslation::AWLMessageToSpiritMessageTranslationStrategy;
using Mock::FrameProcessingScheduler;
using Mock::FrameSinkMock;
using DataFlow::PixelsArray;



class AWLMessageToSpiritMessageTranslationStrategyTest : public ::testing::Test {

protected:
    int const FRAME_INDEX = 0;
    PixelID const SOME_PIXEL_ID = 11;
    Frame const BASE_FRAME = Frame(64829, 16, PixelsArray());
    Frame const FRAME_AFTER_END_OF_FRAME_MESSAGE_TRANSLATION = BASE_FRAME;
    Frame const FRAME_AFTER_DETECTION_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION =
            addTrackToFrame(BASE_FRAME, Track(14291, 96, 379, DEFAULT_ACCELERATION, DEFAULT_DISTANCE,
                                                   DEFAULT_SPEED));
    Frame const FRAME_AFTER_DETECTION_TRACK_AND_VELOCITY_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION =
            addTrackToFrame(BASE_FRAME, Track(14291, 96, 379, 256, 106, 0));

    AWLMessage const SOME_DETECTION_TRACK_AWL_MESSAGE = AWLMessage(10, 2188169, 8, {211, 55, 0, 11, 0, 96, 123, 1});
    AWLMessage const SOME_VELOCITY_TRACK_AWL_MESSAGE = AWLMessage(11, 2188169, 8, {211, 55, 106, 0, 0, 0, 0, 1});
    AWLMessage const SOME_END_FRAME_AWL_MESSAGE = AWLMessage(9, 2188170, 8, {61, 253, 16, 0, 0, 0, 0, 0});

private:
    Frame const addTrackToFrame(Frame frame, Track track) const {
        Frame frameCopy = Frame(frame);
        frameCopy.addTrackToPixelWithID(SOME_PIXEL_ID, std::move(track));
        return frameCopy;
    }
};

TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,
       given_someEndOfFrameAWLMessage_when_translatingOnlyThisMessage_then_buildsFrameWithValidAttributes) {

    auto endOfFrameAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    auto expectedFrame = FRAME_AFTER_END_OF_FRAME_MESSAGE_TRANSLATION;
    AWLMessageToSpiritMessageTranslationStrategy translationStrategy;
    FrameSinkMock frameSinkMock(1);
    FrameProcessingScheduler scheduler(&frameSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(endOfFrameAWLMessage));

    scheduler.terminateAndJoin();
    auto actualFrame = frameSinkMock.getConsumedData().at(FRAME_INDEX);
    ASSERT_EQ(expectedFrame, actualFrame);
}

TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,
       given_someDetectionTrackAndEndOfFrameAWLMessages_when_translatingThoseMessagesInGivenOrder_then_buildsFrameWithValidAttribute) {

    auto detectionTrackAWLMessage = SOME_DETECTION_TRACK_AWL_MESSAGE;
    auto endOfFrameAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    auto expectedFrame = FRAME_AFTER_DETECTION_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION;
    AWLMessageToSpiritMessageTranslationStrategy translationStrategy;
    FrameSinkMock frameSinkMock(1);
    FrameProcessingScheduler scheduler(&frameSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(detectionTrackAWLMessage));
    translationStrategy.translateMessage(std::move(endOfFrameAWLMessage));

    scheduler.terminateAndJoin();
    auto actualFrame = frameSinkMock.getConsumedData().at(FRAME_INDEX);
    ASSERT_EQ(expectedFrame, actualFrame);
}

TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,
       given_someDetectionTrackAndDetectionVelocityAndEndOfFrameAWLMessages_when_translatingThoseMessagesInGivenOrder_then_buildsFrameWithValidAttribute) {

    auto detectionTrackAWLMessage = SOME_DETECTION_TRACK_AWL_MESSAGE;
    auto detectionVelocityAWLMessage = SOME_VELOCITY_TRACK_AWL_MESSAGE;
    auto endOfFrameAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    auto expectedFrame = FRAME_AFTER_DETECTION_TRACK_AND_VELOCITY_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION;
    AWLMessageToSpiritMessageTranslationStrategy translationStrategy;
    FrameSinkMock frameSinkMock(1);
    FrameProcessingScheduler scheduler(&frameSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(detectionTrackAWLMessage));
    translationStrategy.translateMessage(std::move(detectionVelocityAWLMessage));
    translationStrategy.translateMessage(std::move(endOfFrameAWLMessage));

    scheduler.terminateAndJoin();
    auto actualFrame = frameSinkMock.getConsumedData().at(FRAME_INDEX);
    ASSERT_EQ(expectedFrame, actualFrame);
}

TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,
       given_someEndOfFrameAWLMessage_when_translatingThisMessage_then_callsProduceOneTime) {

    auto endOfFrameAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    AWLMessageToSpiritMessageTranslationStrategy translationStrategy;
    FrameSinkMock frameSinkMock(1);
    FrameProcessingScheduler scheduler(&frameSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(endOfFrameAWLMessage));

    scheduler.terminateAndJoin();
    ASSERT_EQ(frameSinkMock.hasBeenCalledExpectedNumberOfTimes(), 1);
}

#endif //SPIRITSENSORGATEWAY_AWLMESSAGETRANSLATORTEST_CPP

