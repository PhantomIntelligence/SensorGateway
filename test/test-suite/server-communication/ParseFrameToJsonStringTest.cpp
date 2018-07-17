#ifndef SPIRITSENSORGATEWAY_PARSEFRAMETOJSONSTRINGTEST_CPP
#define SPIRITSENSORGATEWAY_PARSEFRAMETOJSONSTRINGTEST_CPP

#include <gtest/gtest.h>
#include <fstream>
#include <spirit-sensor-gateway/domain/AWLMessage.h>
#include <test/utilities/files/AWLMessagesFileManager.h>
#include "spirit-sensor-gateway/server-communication/JsonParser.h"
#include "test/utilities/files/FileManager.hpp"

using DataFlow::Frame;
using DataFlow::AWLMessage;
using ServerCommunication::JsonParser;
using TestUtilities::AWLMessagesFileManager;
using DataFlow::PixelsArray;
using DataFlow::TracksArray;
using Defaults::Pixel::DEFAULT_TRACKS_ARRAY;
using Defaults::Frame::DEFAULT_FRAME;
using Defaults::Frame::DEFAULT_PIXELS_ARRAY;


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
    TracksArray tracksArray = DEFAULT_TRACKS_ARRAY;
    tracksArray[2].ID =44;
    tracksArray[2].intensity =44;
    tracksArray[2].speed =44;
    tracksArray[2].confidenceLevel =44;
    tracksArray[2].distance =44;
    tracksArray[2].acceleration =44;
    Pixel pixel = Pixel(4,tracksArray,0);
    PixelsArray pixelsArray = DEFAULT_PIXELS_ARRAY;
    pixelsArray[1] = pixel;
    Frame testFrame = Frame(48, 47, pixelsArray);

    std::string string = JsonParser::parseFrameToJsonString(testFrame);
    std::ofstream file;
    file.open("ActualFrame.json");
    file << string << std::endl;
    file.close();
    AWLMessagesFileManager fileManager;
    bool areEqual = fileManager.areFilesEqual("ActualFrame.json", "ExpectedFrame.json");
    ASSERT_TRUE(areEqual);

}


#endif //SPIRITSENSORGATEWAY_PARSEFRAMETOJSONSTRINGTEST_CPP