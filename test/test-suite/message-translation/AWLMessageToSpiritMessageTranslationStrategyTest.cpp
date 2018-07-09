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
#include "test/utilities/DataStubs.cpp"
#include "test/test-suite/message-translation/stubs/SpiritFramesStub.h"

using DataFlow::Track;
using Defaults::Frame::DEFAULT_FRAME;
using Defaults::Pixel::DEFAULT_PIXEL;
using Defaults::Track::DEFAULT_ACCELERATION_VALUE;
using Defaults::Track::DEFAULT_DISTANCE_VALUE;
using Defaults::Track::DEFAULT_SPEED_VALUE;
using MessageTranslation::AWLMessageToSpiritMessageTranslationStrategy;
using Mock::FrameProcessingScheduler;
using Mock::FrameSinkMock;

class AWLMessageToSpiritMessageTranslationStrategyTest : public ::testing::Test {
protected:
    auto endOfFrameAWLMessage = createAWLMessageWithID(END_OF_FRAME);
    auto detectionTrackAWLMessage = createAWLMessageWithID(DETECTION_TRACK);
    auto detectionVelocityAWLMessage = createAWLMessageWithID(DETECTION_VELOCITY);
    auto const INDEX_OF_FRAME = 0;

    PixelID expectedPixelID;
    TrackID expectedTrackID;
    ConfidenceLevel expectedTrackConfidenceLevel;
    Intensity expectedTrackIntensity = convertTwoBytesToUnsignedBigEndian(detectionTrackAWLMessage.data[6],
                                                                     detectionTrackAWLMessage.data[7]);
    Distance expectedTrackDistance = convertTwoBytesToUnsignedBigEndian(detectionVelocityAWLMessage.data[2],
                                                                    detectionVelocityAWLMessage.data[3]);
    Speed expectedTrackSpeed = convertTwoBytesToSignedBigEndian(detectionTrackAWLMessage.data[4],
                                                                detectionTrackAWLMessage.data[5]);
    Acceleration expectedTrackAcceleration = convertTwoBytesToSignedBigEndian(detectionTrackAWLMessage.data[6],
                                                                              detectionTrackAWLMessage.data[7]);

    bool framesAreEqual(Frame expectedFrame, Frame actualFrame){

        return (expectedFrame.getFrameID() == actualFrame.getFrameID() &&
                expectedFrame.getSystemID() == actualFrame.getSystemID() &&
                expectedFrame.getPixels() == actualFrame.getPixels());
    }

    bool frameAfterDetectionTrackAndEndOfFrameMessagesTranslationIsValid(Frame actualFrame){
        auto expectedFrame = frameAfterEndOfFrameMessageTranslationStub;
        auto expectedPixelID = convertTwoBytesToUnsignedBigEndian(detectionTrackAWLMessage.data[3],
                                                                  detectionTrackAWLMessage.data[4]);
        auto expectedTrackID = convertTwoBytesToUnsignedBigEndian(detectionTrackAWLMessage.data[0],
                                                                  detectionTrackAWLMessage.data[1]);
        auto expectedTrackConfidenceLevel = detectionTrackAWLMessage.data[5];
        auto expectedTrackIntensity = convertTwoBytesToUnsignedBigEndian(detectionTrackAWLMessage.data[6],
                                                                          detectionTrackAWLMessage.data[7]);

        return (expectedFrame.getFrameID() == actualFrame.getFrameID() &&
                expectedFrame.getSystemID() == actualFrame.getSystemID() &&
                expectedFrame.getPixels() == actualFrame.getPixels());
    }

    Frame createExpectedFrameAfterDetectionTrackMessageTranslation(){

        expectedTrackConfidenceLevel = detectionTrackAWLMessage.data[5];
        expectedTrackIntensity = convertTwoBytesToUnsignedBigEndian(detectionTrackAWLMessage.data[6],
                                                                    detectionTrackAWLMessage.data[7]);
    }

    Frame createExpectedFrameAfterDetectionTrackAndDetectionVelocitykMessagesTranslation(){

        expectedTrackDistance = convertTwoBytesToUnsignedBigEndian(detectionVelocityAWLMessage.data[2],
                                                                   detectionVelocityAWLMessage.data[3]);
        expectedTrackSpeed = convertTwoBytesToSignedBigEndian(detectionTrackAWLMessage.data[4],
                                                              detectionTrackAWLMessage.data[5]);
        expectedTrackAcceleration = convertTwoBytesToSignedBigEndian(detectionTrackAWLMessage.data[6],
                                                                     detectionTrackAWLMessage.data[7]);
    }

private:
    AWLMessage createAWLMessageWithID(uint16_t id){
        return AWLMessage(id, 0, 8, {18, 16, 0, 16, 0, 0, 0, 0});

    }
};

TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,
       given_someEndOfFrameAWLMessage_when_translatingOnlyThisMessage_then_buildsFrameWithValidAttributes) {

    auto expectedFrame = frameAfterEndOfFrameMessageTranslationStub;

    AWLMessageToSpiritMessageTranslationStrategy translationStrategy;
    FrameSinkMock frameSinkMock(1);
    FrameProcessingScheduler scheduler(&frameSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateBasicMessage(std::move(endOfFrameAWLMessage));

    scheduler.terminateAndJoin();
    auto actualFrame =  frameSinkMock.getConsumedData().at(INDEX_OF_FRAME);

    ASSERT_TRUE(framesAreEqual(expectedFrame, actualFrame));
}

TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,
       given_someDetectionTrackAndEndOfFrameAWLMessages_when_translatingThoseMessagesInGivenOrder_then_buildsFrameWithValidAttribute) {

    auto expectedFrame = frameAfterDetectionTrackAndEndOfFrameMessagesTranslationStub;

    AWLMessageToSpiritMessageTranslationStrategy translationStrategy;
    FrameSinkMock frameSinkMock(1);
    FrameProcessingScheduler scheduler(&frameSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateBasicMessage(std::move(detectionTrackAWLMessage));
    translationStrategy.translateBasicMessage(std::move(endOfFrameAWLMessage));

    scheduler.terminateAndJoin();
    auto actualFrame =  frameSinkMock.getConsumedData().at(INDEX_OF_FRAME);

    ASSERT_TRUE(framesAreEqual(expectedFrame, actualFrame));

}

TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,
       given_someDetectionTrackAndDetectionVelocityAndEndOfFrameAWLMessages_when_translatingThoseMessagesInGivenOrder_then_buildsFrameWithValidAttribute) {

    auto expectedFrame = frameAfterDetectionTrackAndVelocityTrackAndEndOfFrameMessagesTranslationStub;

    AWLMessageToSpiritMessageTranslationStrategy translationStrategy;
    FrameSinkMock frameSinkMock(1);
    FrameProcessingScheduler scheduler(&frameSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateBasicMessage(std::move(detectionTrackAWLMessage));
    translationStrategy.translateBasicMessage(std::move(detectionVelocityAWLMessage));
    translationStrategy.translateBasicMessage(std::move(endOfFrameAWLMessage));

    scheduler.terminateAndJoin();
    auto actualFrame =  frameSinkMock.getConsumedData().at(INDEX_OF_FRAME);

    ASSERT_TRUE(framesAreEqual(expectedFrame, actualFrame));
}


TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,
       given_someFrameDoneAwlMessage_when_translatingTheAwlMessage_then_callsTheProduceMethodOfDataSource) {

    AWLMessageToSpiritMessageTranslationStrategy translationStrategy;
    FrameSinkMock frameSinkMock(1);
    FrameProcessingScheduler scheduler(&frameSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateBasicMessage(std::move(endOfFrameAWLMessage));

    scheduler.terminateAndJoin();

    ASSERT_EQ(frameSinkMock.hasBeenCalledExpectedNumberOfTimes(),1);

}


#endif //SPIRITSENSORGATEWAY_AWLMESSAGETRANSLATORTEST_CPP
