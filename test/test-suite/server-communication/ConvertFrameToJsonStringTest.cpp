#ifndef SPIRITSENSORGATEWAY_PARSEFRAMETOJSONSTRINGTEST_CPP
#define SPIRITSENSORGATEWAY_PARSEFRAMETOJSONSTRINGTEST_CPP

#include <gtest/gtest.h>
#include "spirit-sensor-gateway/server-communication/JsonConverter.h"
#include "test/utilities/stubs/FrameStub.h"

using DataFlow::Frame;
using ServerCommunication::JsonConverter;
using DataFlow::PixelsArray;
using DataFlow::TracksArray;
using Defaults::Pixel::DEFAULT_TRACKS_ARRAY;
using Defaults::Frame::DEFAULT_FRAME;
using Defaults::Frame::DEFAULT_PIXELS_ARRAY;
using DataStub::frameStub;


class ConvertFrameToJsonStringTest : public ::testing::Test {
};

TEST_F(ConvertFrameToJsonStringTest,
       given_twoDifferentsFrames_when_convertingThemToJson_then_returnsTwoDifferentsConvertedJsonStrings) {
    Frame firstFrame = DEFAULT_FRAME;
    Frame secondFrame = DEFAULT_FRAME;

    secondFrame.frameID = 33;

    auto firstConvertedFrame = JsonConverter::convertFrameToJsonString(firstFrame);
    auto secondConvertedFrame = JsonConverter::convertFrameToJsonString(secondFrame);
    ASSERT_NE(firstConvertedFrame, secondConvertedFrame);
}

TEST_F(ConvertFrameToJsonStringTest, given_aFrame_when_convertingItToAJsonString_then_returnTheCorrectlConvertedJSonString) {
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

    std::string expectedConvertedFrame = frameStub;
    auto actualConvertedFrame = JsonConverter::convertFrameToJsonString(testFrame);

    ASSERT_EQ(expectedConvertedFrame,actualConvertedFrame);
}
#endif //SPIRITSENSORGATEWAY_PARSEFRAMETOJSONSTRINGTEST_CPP