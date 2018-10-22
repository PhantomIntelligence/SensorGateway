#ifndef SENSORGATEWAY_FRAMETEST_CPP
#define SENSORGATEWAY_FRAMETEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "sensor-gateway/common/data-structure/spirit/Frame.h"

using DataFlow::Frame;
using DataFlow::PixelId;
using DataFlow::MessageId;
using DataFlow::SystemId;
using DataFlow::TracksArray;
using DataFlow::TrackId;
using DataFlow::PixelsArray;

class FrameTest : public ::testing::Test {

protected:
    TrackId const SOME_TRACK_ID = 2;
    Track const SOME_TRACK = Track(SOME_TRACK_ID, 0, 135, 0, 110, 0);
    PixelId const SOME_PIXEL_ID = 1;
    Pixel const SOME_PIXEL = Pixel(SOME_PIXEL_ID, TracksArray({SOME_TRACK}), 1);
    Pixel const SOME_OTHER_PIXEL = Pixel(2, TracksArray({SOME_TRACK}), 1);
    MessageId const SOME_MESSAGE_ID = 64830;
    MessageId const SOME_OTHER_MESSAGE_ID = 63830;
    SystemId const SOME_SYSTEM_ID = 16;
    SystemId const SOME_OTHER_SYSTEM_ID = 15;
    PixelsArray const SOME_PIXELS_ARRAY = PixelsArray({SOME_PIXEL});
    PixelsArray const SOME_OTHER_PIXELS_ARRAY = PixelsArray({SOME_PIXEL, SOME_OTHER_PIXEL});
};

TEST_F(FrameTest,
       given_aTrackAndAPixelId_when_addingTheTrackToThePixelWithCorrespondingId_then_trackIsAddedCorrectlyAtRightPositionInPixel) {
    Frame frame;
    Track track;
    auto expectedAddedTrack = track;
    auto trackPosition = 0;

    frame.addTrackToPixelWithId(SOME_PIXEL_ID, std::move(track));

    auto actualAddedTrack = frame.getPixels()->at(SOME_PIXEL_ID).getTracks()->at(trackPosition);
    ASSERT_EQ(expectedAddedTrack, actualAddedTrack);
}


TEST_F(FrameTest, given_twoIdenticalFrames_when_checkingIfTheFramesAreEqual_then_returnsTrue) {
    auto firstFrame = Frame(SOME_MESSAGE_ID, SOME_SYSTEM_ID, SOME_PIXELS_ARRAY);
    auto secondFrame = Frame(SOME_MESSAGE_ID, SOME_SYSTEM_ID, SOME_PIXELS_ARRAY);

    auto framesAreEqual = (firstFrame == secondFrame);
    auto framesAreNotEqual = (firstFrame != secondFrame);

    ASSERT_TRUE(framesAreEqual);
    ASSERT_FALSE(framesAreNotEqual);
}


TEST_F(FrameTest, given_twoIdenticalFramesExceptForTheirMessageId_when_checkingIfTheFramesAreEqual_then_returnsFalse) {
    auto firstFrame = Frame(SOME_MESSAGE_ID, SOME_SYSTEM_ID, SOME_PIXELS_ARRAY);
    auto secondFrame = Frame(SOME_OTHER_MESSAGE_ID, SOME_SYSTEM_ID, SOME_PIXELS_ARRAY);

    auto framesAreEqual = (firstFrame == secondFrame);
    auto framesAreNotEqual = (firstFrame != secondFrame);

    ASSERT_FALSE(framesAreEqual);
    ASSERT_TRUE(framesAreNotEqual);
}

TEST_F(FrameTest, given_twoIdenticalFramesExceptForTheirSystemId_when_checkingIfTheFramesAreEqual_then_returnsFalse) {
    auto firstFrame = Frame(SOME_MESSAGE_ID, SOME_SYSTEM_ID, SOME_PIXELS_ARRAY);
    auto secondFrame = Frame(SOME_MESSAGE_ID, SOME_OTHER_SYSTEM_ID, SOME_PIXELS_ARRAY);

    auto framesAreEqual = (firstFrame == secondFrame);
    auto framesAreNotEqual = (firstFrame != secondFrame);

    ASSERT_FALSE(framesAreEqual);
    ASSERT_TRUE(framesAreNotEqual);
}

TEST_F(FrameTest,
       given_twoIdenticalFramesExceptForTheirPixelsArray_when_checkingIfTheFramesAreEqual_then_returnsFalse) {
    auto firstFrame = Frame(SOME_MESSAGE_ID, SOME_SYSTEM_ID, SOME_PIXELS_ARRAY);
    auto secondFrame = Frame(SOME_MESSAGE_ID, SOME_SYSTEM_ID, SOME_OTHER_PIXELS_ARRAY);

    auto framesAreEqual = (firstFrame == secondFrame);
    auto framesAreNotEqual = (firstFrame != secondFrame);

    ASSERT_FALSE(framesAreEqual);
    ASSERT_TRUE(framesAreNotEqual);
}

#endif //SENSORGATEWAY_FRAMETEST_H
