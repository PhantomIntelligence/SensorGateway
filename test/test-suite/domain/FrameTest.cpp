#ifndef SPIRITSENSORGATEWAY_FRAMETEST_CPP
#define SPIRITSENSORGATEWAY_FRAMETEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "spirit-sensor-gateway/domain/Frame.h"

using DataFlow::Frame;
using DataFlow::PixelID;
using DataFlow::FrameID;
using DataFlow::SystemID;
using DataFlow::TracksArray;
using DataFlow::PixelsArray;

class FrameTest : public ::testing::Test {

protected:
    PixelID const ARBITRARY_TRACK_ID = 2;
    Track const SOME_TRACK = Track(ARBITRARY_TRACK_ID, 0, 135, 0, 110, 0);
    PixelID const ARBITRARY_PIXEL_ID = 1;
    Pixel const SOME_PIXEL = Pixel(ARBITRARY_PIXEL_ID, TracksArray({SOME_TRACK}), 1);
    Pixel const SOME_OTHER_PIXEL = Pixel(2, TracksArray({SOME_TRACK}), 1);
    FrameID const SOME_FRAME_ID = 64830;
    FrameID const SOME_OTHER_FRAME_ID = 63830;
    SystemID const SOME_SYSTEM_ID = 16;
    SystemID const SOME_OTHER_SYSTEM_ID = 15;
    PixelsArray const SOME_PIXELS_ARRAY = PixelsArray({SOME_PIXEL});
    PixelsArray const SOME_OTHER_PIXELS_ARRAY = PixelsArray({SOME_PIXEL, SOME_OTHER_PIXEL});
};

TEST_F(FrameTest, given_aTrackAndAPixelID_when_addingTheTrackToThePixel_then_aPixelWithTheCorrectIDIsInTheFrame) {
    Frame frame;
    Track track;
    frame.addTrackToPixelWithID(ARBITRARY_PIXEL_ID, std::move(track));

    auto actualAddedPixel = frame.getPixels()->at(ARBITRARY_PIXEL_ID);
    auto expectedAddedPixelID = ARBITRARY_PIXEL_ID;

    ASSERT_EQ(expectedAddedPixelID, actualAddedPixel.ID);
}

TEST_F(FrameTest, given_aTrackAndAPixelID_when_addingTheTrackToThePixel_then_thePixelContainsTheTrack) {
    Frame frame;
    auto trackCopy = Track(SOME_TRACK);
    frame.addTrackToPixelWithID(ARBITRARY_PIXEL_ID, std::move(trackCopy));

    auto addedTrack = *(frame.getPixels()->at(ARBITRARY_PIXEL_ID).fetchTrackByID(ARBITRARY_TRACK_ID));

    auto expectedTrack = SOME_TRACK;
    ASSERT_EQ(expectedTrack, addedTrack);
}


TEST_F(FrameTest, given_twoIdenticalFrames_when_checkingIfTheFramesAreEqual_then_returnsTrue) {
    auto firstFrame = Frame(SOME_FRAME_ID, SOME_SYSTEM_ID, SOME_PIXELS_ARRAY);
    auto secondFrame = Frame(SOME_FRAME_ID, SOME_SYSTEM_ID, SOME_PIXELS_ARRAY);

    auto framesAreEqual = (firstFrame == secondFrame);
    auto framesAreNotEqual = (firstFrame != secondFrame);

    ASSERT_TRUE(framesAreEqual);
    ASSERT_FALSE(framesAreNotEqual);
}


TEST_F(FrameTest, given_twoIdenticalFramesExceptForTheirFrameID_when_checkingIfTheFramesAreEqual_then_returnsFalse) {
    auto firstFrame = Frame(SOME_FRAME_ID, SOME_SYSTEM_ID, SOME_PIXELS_ARRAY);
    auto secondFrame = Frame(SOME_OTHER_FRAME_ID, SOME_SYSTEM_ID, SOME_PIXELS_ARRAY);

    auto framesAreEqual = (firstFrame == secondFrame);
    auto framesAreNotEqual = (firstFrame != secondFrame);

    ASSERT_FALSE(framesAreEqual);
    ASSERT_TRUE(framesAreNotEqual);
}

TEST_F(FrameTest, given_twoIdenticalFramesExceptForTheirSystemID_when_checkingIfTheFramesAreEqual_then_returnsFalse) {
    auto firstFrame = Frame(SOME_FRAME_ID, SOME_SYSTEM_ID, SOME_PIXELS_ARRAY);
    auto secondFrame = Frame(SOME_FRAME_ID, SOME_OTHER_SYSTEM_ID, SOME_PIXELS_ARRAY);

    auto framesAreEqual = (firstFrame == secondFrame);
    auto framesAreNotEqual = (firstFrame != secondFrame);

    ASSERT_FALSE(framesAreEqual);
    ASSERT_TRUE(framesAreNotEqual);
}

TEST_F(FrameTest,
       given_twoIdenticalFramesExceptForTheirPixelsArray_when_checkingIfTheFramesAreEqual_then_returnsFalse) {
    auto firstFrame = Frame(SOME_FRAME_ID, SOME_SYSTEM_ID, SOME_PIXELS_ARRAY);
    auto secondFrame = Frame(SOME_FRAME_ID, SOME_SYSTEM_ID, SOME_OTHER_PIXELS_ARRAY);

    auto framesAreEqual = (firstFrame == secondFrame);
    auto framesAreNotEqual = (firstFrame != secondFrame);

    ASSERT_FALSE(framesAreEqual);
    ASSERT_TRUE(framesAreNotEqual);
}


#endif //SPIRITSENSORGATEWAY_FRAMETEST_H
