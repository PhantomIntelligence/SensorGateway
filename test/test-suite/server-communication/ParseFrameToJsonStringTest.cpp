#ifndef SPIRITSENSORGATEWAY_PARSEFRAMETOJSONSTRINGTEST_CPP
#define SPIRITSENSORGATEWAY_PARSEFRAMETOJSONSTRINGTEST_CPP

#include <gtest/gtest.h>
#include "spirit-sensor-gateway/server-communication/JsonParser.h"
#include "test/utilities/stubs/FrameStub.h"

using DataFlow::Frame;
using ServerCommunication::JsonParser;
using DataFlow::PixelsArray;
using DataFlow::TracksArray;
using Defaults::Pixel::DEFAULT_TRACKS_ARRAY;
using Defaults::Frame::DEFAULT_FRAME;
using Defaults::Frame::DEFAULT_PIXELS_ARRAY;
using DataStub::frameStub;


class ParseFrameToJsonStringTest : public ::testing::Test {
};

TEST_F(ParseFrameToJsonStringTest,
       given_twoDifferentsFrames_when_parsingThemToJson_then_returnsTwoDifferentsParsedJsonStrings) {
    Frame firstFrame = DEFAULT_FRAME;
    Frame secondFrame = DEFAULT_FRAME;

    secondFrame.frameID = 33;

    auto firstParsedFrame = JsonParser::parseFrameToJsonString(firstFrame);
    auto secondParsedFrame = JsonParser::parseFrameToJsonString(secondFrame);
    ASSERT_NE(firstParsedFrame, secondParsedFrame);
}

TEST_F(ParseFrameToJsonStringTest, given_aFrame_when_parsingItToAJsonString_then_returnTheCorrectlyParsedJSonString) {
    TracksArray tracksArray = DEFAULT_TRACKS_ARRAY;
    tracksArray[2].ID = 44;
    tracksArray[2].intensity = 44;
    tracksArray[2].speed = 44;
    tracksArray[2].confidenceLevel = 44;
    tracksArray[2].distance = 44;
    tracksArray[2].acceleration = 44;
    Pixel pixel = Pixel(4, tracksArray, 0);
    PixelsArray pixelsArray = DEFAULT_PIXELS_ARRAY;
    pixelsArray[1] = pixel;
    Frame testFrame = Frame(48, 47, pixelsArray);

    std::string expectedParsedFrame = frameStub;
    auto actualParsedFrame = JsonParser::parseFrameToJsonString(testFrame);

    ASSERT_EQ(expectedParsedFrame,actualParsedFrame);
}
#endif //SPIRITSENSORGATEWAY_PARSEFRAMETOJSONSTRINGTEST_CPP