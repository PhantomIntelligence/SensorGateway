#ifndef SPIRITSENSORGATEWAY_AWLMESSAGETRANSLATORTEST_CPP
#define SPIRITSENSORGATEWAY_AWLMESSAGETRANSLATORTEST_CPP


#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "spirit-sensor-gateway/spirit-protocol-translation/AWLMessageTranslator.h"

class AWLMessageTranslatorTest : public ::testing::Test {
protected:
    AWLMessage givenAnAWLMessageWithAnId(uint16_t id) const ;
};


TEST_F(AWLMessageTranslatorTest,given_anAWLMessageWithAFrameDoneId_when_translatingTheAwlMessage_then_setTheCorrectFrameIdAndSystemIdOfTheFrame){
    AWLMessageTranslator* awlMessageTranslatorTest =  new AWLMessageTranslator();
    AWLMessage awlMessage;
    awlMessage.id = FRAME_DONE;
    awlMessage.data[0] = 0x12;
    awlMessage.data[1] = 0x10;
    awlMessage.data[2] = 0x00;
    awlMessage.data[3] = 0x10;

    awlMessageTranslatorTest->translateBasicMessage(&awlMessage);
    auto expectedFrameIdOfSpiritFrame = 0x1012;
    auto expectedSystemIdOfSpiritFrame = 0x1000;

    auto spiritFrame = awlMessageTranslatorTest->getFrames()[0];
    ASSERT_EQ(expectedFrameIdOfSpiritFrame,spiritFrame.getFrameID());
    ASSERT_EQ(expectedSystemIdOfSpiritFrame,spiritFrame.getSystemID());

}

TEST_F(AWLMessageTranslatorTest,given_anAWLMessageWithADetectionTrackId_when_translatingTheAwlMessage_then_setTheCorrectAttributesOfTheTrack) {

    AWLMessageTranslator awlMessageTranslator;
    auto detectionTrackAwlMessage = givenAnAWLMessageWithAnId(DETECTION_TRACK);
    auto endOfFrameAwlMessage = givenAnAWLMessageWithAnId(FRAME_DONE);

    awlMessageTranslator.translateBasicMessage(&detectionTrackAwlMessage);
    awlMessageTranslator.translateBasicMessage(&endOfFrameAwlMessage);

    auto expectedPixelId = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[3],detectionTrackAwlMessage.data[4]);
    auto expectedTrackId = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[0],detectionTrackAwlMessage.data[1]);
    uint8_t expectedTrackConfidenceLevel = detectionTrackAwlMessage.data[5];
    auto expectedTrackIntensity = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[6],detectionTrackAwlMessage.data[7]);
    auto frame = awlMessageTranslator.getFrames()[0];
    auto pixel = frame.fetchPixelByID(expectedPixelId);
    auto track = *(pixel->fetchTrackByID(expectedTrackId));


    ASSERT_EQ(track.getID(),expectedTrackId);
    ASSERT_EQ(track.getConfidenceLevel(),expectedTrackConfidenceLevel);
    ASSERT_EQ(track.getIntensity(),expectedTrackIntensity);
}

TEST_F(AWLMessageTranslatorTest,given_anAWLMessageWithAnIdofEleven_when_translatingTheAwlMessage_then_setTheCorrectAttributesOfTheTrack) {

    AWLMessageTranslator awlMessageTranslator;

    auto detectionTrackAwlMessage = givenAnAWLMessageWithAnId(DETECTION_TRACK);
    auto detectionVelocityAwlMessage = givenAnAWLMessageWithAnId(DETECTION_VELOCITY);
    auto endOfFrameAwlMessage = givenAnAWLMessageWithAnId(FRAME_DONE);

    awlMessageTranslator.translateBasicMessage(&detectionTrackAwlMessage);
    awlMessageTranslator.translateBasicMessage(&detectionVelocityAwlMessage);
    awlMessageTranslator.translateBasicMessage(&endOfFrameAwlMessage);


    auto expectedPixelId = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[3],detectionTrackAwlMessage.data[4]);
    auto expectedTrackId = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[0],detectionTrackAwlMessage.data[1]);
    uint16_t expectedTrackDistance = convertTwoBytesToUnsignedBigEndian(detectionVelocityAwlMessage.data[2],detectionVelocityAwlMessage.data[3]);
    int16_t expectedTrackSpeed = convertTwoBytesToSignedBigEndian(detectionTrackAwlMessage.data[4],detectionTrackAwlMessage.data[5]);
    int16_t expectedTrackAcceleration = convertTwoBytesToSignedBigEndian(detectionTrackAwlMessage.data[6],detectionTrackAwlMessage.data[7]);

    auto frame = awlMessageTranslator.getFrames().at(0);
    auto pixel = frame.fetchPixelByID(expectedPixelId);
    auto track = *(pixel->fetchTrackByID(expectedTrackId));

    ASSERT_EQ(expectedTrackDistance,track.getDistance());
    ASSERT_EQ(expectedTrackSpeed,track.getSpeed());
    ASSERT_EQ(expectedTrackAcceleration,track.getAcceleration());
}


AWLMessage AWLMessageTranslatorTest::givenAnAWLMessageWithAnId(uint16_t id) const {

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