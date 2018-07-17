#ifndef SPIRITSENSORGATEWAY_PARSEFRAMETOJSONSTRINGTEST_CPP
#define SPIRITSENSORGATEWAY_PARSEFRAMETOJSONSTRINGTEST_CPP

#include <gtest/gtest.h>
#include "spirit-sensor-gateway/server-communication/JsonParser.h"

using DataFlow::Frame;
using Defaults::Frame::DEFAULT_FRAME;
using Defaults::Pixel::DEFAULT_PIXEL;
using Defaults::Track::DEFAULT_TRACK;
using ServerCommunication::parseFrameToJsonString;
using ServerCommunication::parseTrackToJson;
using ServerCommunication::parsePixelToJson;

TEST(random,random){
    Frame testFrame = DEFAULT_FRAME;
    std::string json = parseFrameToJsonString(testFrame);
    std::cout<<json<<std::endl;
}




#endif //SPIRITSENSORGATEWAY_PARSEFRAMETOJSONSTRINGTEST_CPP