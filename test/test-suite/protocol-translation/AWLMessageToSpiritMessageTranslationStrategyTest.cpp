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

#include "spirit-sensor-gateway/protocol-translation/AWLMessageToSpiritMessageTranslationStrategy.h"

using namespace Defaults::Track;

class AWLMessageToSpiritMessageTranslationStrategyTest : public ::testing::Test {
protected:
    AWLMessage createAWLMessageWithID(uint16_t id) const ;
};


TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,given_someFrameDoneAWLMessage_when_translatingTheAwlMessage_then_setsTheCorrectFrameIdAndSystemId){
    AWLMessageToSpiritMessageTranslationStrategy awlMessageTranslator;

    AWLMessage awlMessage = AWLMessage::returnDefaultData();

    awlMessage.id = FRAME_DONE;
    awlMessage.data[0] = 18;
    awlMessage.data[1] = 16;
    awlMessage.data[2] = 0;
    awlMessage.data[3] = 16;

    awlMessageTranslator.translateBasicMessage(&awlMessage);
    auto expectedFrameID = 4114;
    auto expectedSystemID = 4096;

    auto spiritFrame = awlMessageTranslator.getFrames()[0];
    ASSERT_EQ(expectedFrameID,spiritFrame.getFrameID());
    ASSERT_EQ(expectedSystemID,spiritFrame.getSystemID());

}

TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,given_someDetectionTrackAWLMessage_when_translatingTheAwlMessage_then_setsTheCorrectAttributesOfTheTrack) {

    AWLMessageToSpiritMessageTranslationStrategy awlMessageTranslator;
    auto detectionTrackAwlMessage = createAWLMessageWithID(DETECTION_TRACK);
    auto endOfFrameAwlMessage = createAWLMessageWithID(FRAME_DONE);

    awlMessageTranslator.translateBasicMessage(&detectionTrackAwlMessage);
    awlMessageTranslator.translateBasicMessage(&endOfFrameAwlMessage);

    auto expectedPixelID = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[3],detectionTrackAwlMessage.data[4]);
    auto expectedTrackID = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[0],detectionTrackAwlMessage.data[1]);
    auto expectedTrackConfidenceLevel = detectionTrackAwlMessage.data[5];
    auto expectedTrackIntensity = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[6],detectionTrackAwlMessage.data[7]);
    auto frame = awlMessageTranslator.getFrames()[0];
    auto pixel = frame.fetchPixelByID(expectedPixelID);
    auto track = *(pixel->fetchTrackByID(expectedTrackID));


    ASSERT_EQ(expectedTrackID,track.getID());
    ASSERT_EQ(expectedTrackConfidenceLevel,track.getConfidenceLevel());
    ASSERT_EQ(expectedTrackIntensity,track.getIntensity());
    ASSERT_EQ(DEFAULT_ACCELERATION_VALUE,track.getAcceleration());
    ASSERT_EQ(DEFAULT_DISTANCE_VALUE,track.getDistance());
    ASSERT_EQ(DEFAULT_SPEED_VALUE,track.getSpeed());
}

TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,given_someVelocityTrackAWLMessage_when_translatingTheAWLMessage_then_setsTheCorrectAttributesOfTheTrack) {

    AWLMessageToSpiritMessageTranslationStrategy awlMessageTranslator;

    auto detectionTrackAwlMessage = createAWLMessageWithID(DETECTION_TRACK);
    auto detectionVelocityAwlMessage = createAWLMessageWithID(DETECTION_VELOCITY);
    auto endOfFrameAwlMessage = createAWLMessageWithID(FRAME_DONE);

    awlMessageTranslator.translateBasicMessage(&detectionTrackAwlMessage);
    awlMessageTranslator.translateBasicMessage(&detectionVelocityAwlMessage);
    awlMessageTranslator.translateBasicMessage(&endOfFrameAwlMessage);


    auto expectedPixelID = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[3],detectionTrackAwlMessage.data[4]);
    auto expectedTrackID = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[0],detectionTrackAwlMessage.data[1]);
    auto expectedTrackConfidenceLevel = detectionTrackAwlMessage.data[5];
    auto expectedTrackIntensity = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[6],detectionTrackAwlMessage.data[7]);
    auto expectedTrackDistance = convertTwoBytesToUnsignedBigEndian(detectionVelocityAwlMessage.data[2],detectionVelocityAwlMessage.data[3]);
    Speed expectedTrackSpeed = convertTwoBytesToSignedBigEndian(detectionTrackAwlMessage.data[4],detectionTrackAwlMessage.data[5]);
    Acceleration expectedTrackAcceleration = convertTwoBytesToSignedBigEndian(detectionTrackAwlMessage.data[6],detectionTrackAwlMessage.data[7]);

    auto frame = awlMessageTranslator.getFrames().at(0);
    auto pixel = frame.fetchPixelByID(expectedPixelID);
    auto track = *(pixel->fetchTrackByID(expectedTrackID));

    ASSERT_EQ(expectedTrackID,track.getID());
    ASSERT_EQ(expectedTrackConfidenceLevel,track.getConfidenceLevel());
    ASSERT_EQ(expectedTrackIntensity,track.getIntensity());
    ASSERT_EQ(expectedTrackDistance,track.getDistance());
    ASSERT_EQ(expectedTrackSpeed,track.getSpeed());
    ASSERT_EQ(expectedTrackAcceleration,track.getAcceleration());
}


AWLMessage AWLMessageToSpiritMessageTranslationStrategyTest::createAWLMessageWithID(uint16_t id) const {

    AWLMessage awlMessage = AWLMessage::returnDefaultData();
    awlMessage.id = id;
    awlMessage.data[0] = 16;
    awlMessage.data[1] = 32;
    awlMessage.data[2] = 48;
    awlMessage.data[3] = 1;
    awlMessage.data[4] = 0;
    awlMessage.data[5] = 96;
    awlMessage.data[6] = 112;
    awlMessage.data[7] = 128;
    return awlMessage;
}

#endif //SPIRITSENSORGATEWAY_AWLMESSAGETRANSLATORTEST_CPP
