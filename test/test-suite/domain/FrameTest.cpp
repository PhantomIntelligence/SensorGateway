#ifndef SPIRITSENSORGATEWAY_FRAMETEST_CPP
#define SPIRITSENSORGATEWAY_FRAMETEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "spirit-sensor-gateway/domain/Frame.h"

using namespace SpiritProtocol;

class FrameTest : public ::testing::Test {

};

TEST_F(FrameTest,given_anEmptyFrameAndAPixel_when_addingThePixelToTheFrame_then_addThePixelToTheFrameCorrectly){
    Frame frame;
    Pixel pixel(0x01);
    frame.addPixel(pixel);
    ASSERT_EQ(pixel,frame.getPixels().at(0x01));
}

TEST_F(FrameTest,given_aFrameContainingAPixel_WhenFetchingThePixelWithThePexelId_then_returnTheCorrectPixel){
    Frame frame;
    Pixel pixel(0x01);
    frame.addPixel(pixel);

    auto pixelPointer = frame.fetchPixelByID(pixel.getID());

    ASSERT_EQ(pixel,*pixelPointer);
}









#endif //SPIRITSENSORGATEWAY_FRAMETEST_H
