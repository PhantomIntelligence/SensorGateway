#ifndef SPIRITSENSORGATEWAY_AWLMESSAGETRANSLATORTEST_CPP
#define SPIRITSENSORGATEWAY_AWLMESSAGETRANSLATORTEST_CPP


#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "spirit-sensor-gateway/spirit-protocol-translation/AWLMessageTranslator.h"

using namespace DefaultValues::Track;

class AWLMessageTranslatorTest : public ::testing::Test {
protected:
    AWLMessage createAWLMessageWithID(uint16_t id) const ;
};


TEST_F(AWLMessageTranslatorTest,given_someFrameDoneAWLMessage_when_translatingTheAwlMessage_then_setTheCorrectFrameIdAndSystemId){
    AWLMessageTranslator awlMessageTranslator;
    AWLMessage awlMessage;
    awlMessage.id = FRAME_DONE;
    awlMessage.data[0] = 0x12;
    awlMessage.data[1] = 0x10;
    awlMessage.data[2] = 0x00;
    awlMessage.data[3] = 0x10;

    awlMessageTranslator.translateBasicMessage(&awlMessage);
    auto expectedFrameIDOfSpiritFrame = 0x1012;
    auto expectedSystemIDOfSpiritFrame = 0x1000;

    auto spiritFrame = awlMessageTranslator.getFrames()[0];
    ASSERT_EQ(expectedFrameIDOfSpiritFrame,spiritFrame.getFrameID());
    ASSERT_EQ(expectedSystemIDOfSpiritFrame,spiritFrame.getSystemID());

}

TEST_F(AWLMessageTranslatorTest,given_someDetectionTrackAWLMessage_when_translatingTheAwlMessage_then_setsTheCorrectAttributesOfTheTrack) {

    AWLMessageTranslator awlMessageTranslator;
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

TEST_F(AWLMessageTranslatorTest,given_someVelocityTrackAWLMessage_when_translatingTheAWLMessage_then_setsTheCorrectAttributesOfTheTrack) {

    AWLMessageTranslator awlMessageTranslator;

    auto detectionTrackAwlMessage = createAWLMessageWithID(DETECTION_TRACK);
    auto detectionVelocityAwlMessage = createAWLMessageWithID(DETECTION_VELOCITY);
    auto endOfFrameAwlMessage = createAWLMessageWithID(FRAME_DONE);

    awlMessageTranslator.translateBasicMessage(&detectionTrackAwlMessage);
    awlMessageTranslator.translateBasicMessage(&detectionVelocityAwlMessage);
    awlMessageTranslator.translateBasicMessage(&endOfFrameAwlMessage);


    auto expectedPixelId = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[3],detectionTrackAwlMessage.data[4]);
    auto expectedTrackId = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[0],detectionTrackAwlMessage.data[1]);
    auto expectedTrackDistance = convertTwoBytesToUnsignedBigEndian(detectionVelocityAwlMessage.data[2],detectionVelocityAwlMessage.data[3]);
    Speed expectedTrackSpeed = convertTwoBytesToSignedBigEndian(detectionTrackAwlMessage.data[4],detectionTrackAwlMessage.data[5]);
    Acceleration expectedTrackAcceleration = convertTwoBytesToSignedBigEndian(detectionTrackAwlMessage.data[6],detectionTrackAwlMessage.data[7]);

    auto frame = awlMessageTranslator.getFrames().at(0);
    auto pixel = frame.fetchPixelByID(expectedPixelId);
    auto track = *(pixel->fetchTrackByID(expectedTrackId));

    ASSERT_EQ(expectedTrackDistance,track.getDistance());
    ASSERT_EQ(expectedTrackSpeed,track.getSpeed());
    ASSERT_EQ(expectedTrackAcceleration,track.getAcceleration());
}


AWLMessage AWLMessageTranslatorTest::createAWLMessageWithID(uint16_t id) const {

    AWLMessage awlMessage;
    awlMessage.id = id;
    awlMessage.data[0] = 0x10;
    awlMessage.data[1] = 0x20;
    awlMessage.data[2] = 0x30;
    awlMessage.data[3] = 0x01;
    awlMessage.data[4] = 0x00;
    awlMessage.data[5] = 0x60;
    awlMessage.data[6] = 0x70;
    awlMessage.data[7] = 0x80;
    return awlMessage;
}

#endif //SPIRITSENSORGATEWAY_AWLMESSAGETRANSLATORTEST_CPP