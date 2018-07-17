#ifndef SPIRITSENSORGATEWAY_PARSEFRAMETOJSONSTRINGTEST_CPP
#define SPIRITSENSORGATEWAY_PARSEFRAMETOJSONSTRINGTEST_CPP

#include <gtest/gtest.h>
#include "spirit-sensor-gateway/server-communication/JsonParser.h"

using DataFlow::Frame;
using ServerCommunication::JsonParser;
using Defaults::Frame::DEFAULT_FRAME;
using Defaults::Pixel::DEFAULT_PIXEL;
using Defaults::Track::DEFAULT_TRACK;


class ParseFrameToJsonStringTest : public ::testing::Test {

};

TEST_F(ParseFrameToJsonStringTest,
       given_twoDifferentsFrames_when_parsingThenToJson_then_returnsTwoDifferentParsedJsonString) {
    Frame firstFrame = DEFAULT_FRAME;
    Frame secondFrame = DEFAULT_FRAME;

    secondFrame.frameID = 33;

    auto firstParsedFrame = JsonParser::parseFrameToJsonString(firstFrame);
    auto secondParsedFrame = JsonParser::parseFrameToJsonString(secondFrame);
    ASSERT_NE(firstParsedFrame, secondParsedFrame);
}

TEST_F(ParseFrameToJsonStringTest, given_aFrame_when_parsongItToAJsonString_then_returnTheCorrectlyParsedJSonString) {
    Frame testFrame = DEFAULT_FRAME;

    std::string string = JsonParser::parseFrameToJsonString(testFrame);
    std::cout<<string<<std::endl;

}


#endif //SPIRITSENSORGATEWAY_PARSEFRAMETOJSONSTRINGTEST_CPP