#ifndef SPIRITSENSORGATEWAY_FRAMETEST_CPP
#define SPIRITSENSORGATEWAY_FRAMETEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "spirit-sensor-gateway/domain/Frame.h"

using DataFlow::Frame;
using DataFlow::FrameID;
using DataFlow::SystemID;
using DataFlow::TracksArray;
using DataFlow::PixelsArray;

class FrameTest : public ::testing::Test {

protected:
    Track const SOME_TRACK = Track(2, 0, 135, 0, 110, 0);
    Pixel const SOME_PIXEL = Pixel(0, TracksArray({SOME_TRACK}));
    Pixel const SOME_OTHER_PIXEL = Pixel(2, TracksArray({SOME_TRACK}));
    FrameID const SOME_FRAME_ID = 64830;
    FrameID const SOME_OTHER_FRAME_ID = 63830;
    SystemID const SOME_SYSTEM_ID = 16;
    SystemID const SOME_OTHER_SYSTEM_ID = 15;
    PixelsArray const SOME_PIXELS_ARRAY = PixelsArray({SOME_PIXEL});
    PixelsArray const SOME_OTHER_PIXELS_ARRAY = PixelsArray({SOME_PIXEL, SOME_OTHER_PIXEL});
};

TEST_F(FrameTest,given_anExistingPixelInAFrame_when_fetchingThePixelByItsID_then_returnsAPointerToThisPixel){
    Frame frame;
    frame.addPixel(SOME_PIXEL);
    auto pixelPosition = 0;
    Pixel* expectedPixelPointer = &frame.getPixels()->at(pixelPosition);

    auto actualPixelPointer = frame.fetchPixelByID(SOME_PIXEL.getID());

    ASSERT_EQ(expectedPixelPointer, actualPixelPointer);
}

TEST_F(FrameTest,given_anFrameAndAPixel_when_addingThePixelToTheFrame_then_pixelIsAddedCorrectlyAtRightPositionInPixelsArray){
    Frame frame;
    frame.addPixel(SOME_PIXEL);
    auto pixelPosition = 0;
    auto expectedAddedPixel = SOME_PIXEL;

    auto actualAddedPixel = frame.getPixels()->at(pixelPosition);

    ASSERT_EQ(expectedAddedPixel, actualAddedPixel);
}


TEST_F(FrameTest,given_twoIdenticalFrames_when_checkingIfTheFramesAreEqual_then_returnsTrue) {
    auto firstFrame = Frame(SOME_FRAME_ID, SOME_SYSTEM_ID, SOME_PIXELS_ARRAY);
    auto secondFrame = Frame(SOME_FRAME_ID, SOME_SYSTEM_ID, SOME_PIXELS_ARRAY);

    auto framesAreEqual = (firstFrame == secondFrame);
    auto framesAreNotEqual = (firstFrame != secondFrame);

    ASSERT_TRUE(framesAreEqual);
    ASSERT_FALSE(framesAreNotEqual);
}


TEST_F(FrameTest,given_twoIdenticalFramesExceptForTheirFrameID_when_checkingIfTheFramesAreEqual_then_returnsFalse) {
    auto firstFrame = Frame(SOME_FRAME_ID, SOME_SYSTEM_ID, SOME_PIXELS_ARRAY);
    auto secondFrame = Frame(SOME_OTHER_FRAME_ID, SOME_SYSTEM_ID, SOME_PIXELS_ARRAY);

    auto framesAreEqual = (firstFrame == secondFrame);
    auto framesAreNotEqual = (firstFrame != secondFrame);

    ASSERT_FALSE(framesAreEqual);
    ASSERT_TRUE(framesAreNotEqual);
}

TEST_F(FrameTest,given_twoIdenticalFramesExceptForTheirSystemID_when_checkingIfTheFramesAreEqual_then_returnsFalse) {
    auto firstFrame = Frame(SOME_FRAME_ID, SOME_SYSTEM_ID, SOME_PIXELS_ARRAY);
    auto secondFrame = Frame(SOME_FRAME_ID, SOME_OTHER_SYSTEM_ID, SOME_PIXELS_ARRAY);

    auto framesAreEqual = (firstFrame == secondFrame);
    auto framesAreNotEqual = (firstFrame != secondFrame);

    ASSERT_FALSE(framesAreEqual);
    ASSERT_TRUE(framesAreNotEqual);
}

TEST_F(FrameTest,given_twoIdenticalFramesExceptForTheirPixelsArray_when_checkingIfTheFramesAreEqual_then_returnsFalse) {
    auto firstFrame = Frame(SOME_FRAME_ID, SOME_SYSTEM_ID, SOME_PIXELS_ARRAY);
    auto secondFrame = Frame(SOME_FRAME_ID, SOME_SYSTEM_ID, SOME_OTHER_PIXELS_ARRAY);

    auto framesAreEqual = (firstFrame == secondFrame);
    auto framesAreNotEqual = (firstFrame != secondFrame);

    ASSERT_FALSE(framesAreEqual);
    ASSERT_TRUE(framesAreNotEqual);
}


#endif //SPIRITSENSORGATEWAY_FRAMETEST_H
