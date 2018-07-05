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
    Pixel pixel(1);
    frame.addPixel(pixel);
    ASSERT_EQ(pixel, frame.getPixelsAddress()->at(1));
}

TEST_F(FrameTest,given_aFrameContainingAPixel_WhenFetchingThePixeById_then_returnsTheCorrectPixel){
    Frame frame;
    Pixel pixel(1);
    frame.addPixel(pixel);

    auto pixelPointer = frame.fetchPixelByID(pixel.getID());

    ASSERT_EQ(pixel,*pixelPointer);
}


#endif //SPIRITSENSORGATEWAY_FRAMETEST_H
