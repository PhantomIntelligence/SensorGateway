#ifndef SPIRITSENSORGATEWAY_FRAMETEST_CPP
#define SPIRITSENSORGATEWAY_FRAMETEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "spirit-sensor-gateway/domain/Frame.h"

using namespace DataFlow;

class FrameTest : public ::testing::Test {

};

TEST_F(FrameTest,given_anEmptyFrameAndAPixel_when_addingThePixelToTheFrame_then_pixelIsAddedCorrectly){
    Frame frame;
    Pixel pixel(1);
    frame.addPixel(pixel);
    ASSERT_EQ(pixel, frame.getPixels()->at(pixel.getID()));
}

TEST_F(FrameTest,given_aFrameContainingAPixel_whenFetchingThePixeByItsID_then_returnsTheCorrectPixel){
    Frame frame;
    Pixel pixel(1);
    frame.addPixel(pixel);

    auto pixelPointer = frame.fetchPixelByID(pixel.getID());

    ASSERT_EQ(pixel,*pixelPointer);
}


#endif //SPIRITSENSORGATEWAY_FRAMETEST_H
