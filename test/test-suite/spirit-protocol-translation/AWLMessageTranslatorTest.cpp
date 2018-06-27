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

    auto spiritFrame = awlMessageTranslatorTest->getSpiritFrames()[0];
    ASSERT_EQ(expectedFrameIdOfSpiritFrame,spiritFrame.getFrameID());
    ASSERT_EQ(expectedSystemIdOfSpiritFrame,spiritFrame.getSystemID());

}

TEST_F(AWLMessageTranslatorTest,given_anAWLMessageWithADetectionTrackId_when_translatingTheAwlMessage_then_) {

    AWLMessageTranslator awlMessageTranslatorTest;
    auto detectionTrackAwlMessage = givenAnAWLMessageWithAnId(DETECTION_TRACK);
    auto endOfFrameAwlMessage = givenAnAWLMessageWithAnId(FRAME_DONE);

    awlMessageTranslatorTest.translateBasicMessage(&detectionTrackAwlMessage);
    awlMessageTranslatorTest.translateBasicMessage(&endOfFrameAwlMessage);

    auto expectedPixelId = 0x0001;
    auto expectedTrackId = 0x2010;
    auto expectedTrackConfidenceLevel = 0x60;
    auto expectedTrackIntensity = 0x8070;
    auto spiritFrame = awlMessageTranslatorTest.getSpiritFrames()[0];
    auto spiritPixel = spiritFrame.getPixelByID(expectedPixelId);
    auto spiritTrack = spiritPixel->getTrackById(expectedTrackId);


    ASSERT_EQ(spiritTrack.getTrackID(),expectedTrackId);
    ASSERT_EQ(spiritTrack.getTrackID(),expectedTrackId);
    ASSERT_EQ(spiritTrack.getConfidenceLevel(),expectedTrackConfidenceLevel);
    ASSERT_EQ(spiritTrack.getIntensity(),expectedTrackIntensity);
}

TEST_F(AWLMessageTranslatorTest,given_anAWLMessageWithAnIdofEleven_when_translatingTheAwlMessage) {

    AWLMessageTranslator* awlMessageTranslatorTest =  new AWLMessageTranslator();

    auto detectionTrackAwlMessage = givenAnAWLMessageWithAnId(DETECTION_TRACK);
    auto detectionVelocityAwlMessage = givenAnAWLMessageWithAnId(DETECTION_VELOCITY);
    auto endOfFrameAwlMessage = givenAnAWLMessageWithAnId(FRAME_DONE);

    awlMessageTranslatorTest->translateBasicMessage(&detectionTrackAwlMessage);
    awlMessageTranslatorTest->translateBasicMessage(&detectionVelocityAwlMessage);
    awlMessageTranslatorTest->translateBasicMessage(&endOfFrameAwlMessage);





}

TEST_F(AWLMessageTranslatorTest,given_aTrackVelocityMessageAndNoDetectionTrackMessage_when_translatingTheMessage_then_throwsAnException){
    AWLMessageTranslator awlMessageTranslatorTest;

    auto detectionVelocityAwlMessage = givenAnAWLMessageWithAnId(DETECTION_VELOCITY);

}

TEST_F(AWLMessageTranslatorTest,given_anAWLMessageWithAnUnknownID) {
    AWLMessageTranslator* awlMessageTranslatorTest =  new AWLMessageTranslator();
    AWLMessage* awlMessage= new AWLMessage();
    awlMessage->id = 0x11;

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