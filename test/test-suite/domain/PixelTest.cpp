#ifndef SPIRITSENSORGATEWAY_PIXELTEST_CPP
#define SPIRITSENSORGATEWAY_PIXELTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "spirit-sensor-gateway/domain/Pixel.h"

using DataFlow::Pixel;
using DataFlow::PixelID;
using DataFlow::TracksArray;

class PixelTest : public ::testing::Test {

protected:
    Track const SOME_TRACK = Track(2, 0, 135, 0, 110, 0);
    Track const SOME_OTHER_TRACK = Track(14286, 0, 125, 0, 105, 0);
    PixelID const SOME_ID = 1;
    PixelID const SOME_OTHER_ID = 2;
    TracksArray const SOME_TRACKS_ARRAY = TracksArray({SOME_TRACK});
    TracksArray const SOME_OTHER_TRACKS_ARRAY = TracksArray({SOME_TRACK, SOME_OTHER_TRACK});
};

TEST_F(PixelTest,given_anExistingTrackInAPixel_when_checkingIfTheTrackExists_then_returnsTrue) {
    Pixel pixel(SOME_ID);
    pixel.addTrack(SOME_TRACK);

    auto trackExists = pixel.doesTrackExist(SOME_TRACK.getID());

    ASSERT_TRUE(trackExists);
}

TEST_F(PixelTest,given_anNonExistingTrackInAPixel_when_checkingIfTheTrackExists_then_returnsFalse) {
    Pixel pixel(SOME_ID);

    auto trackExists = pixel.doesTrackExist(SOME_TRACK.getID());

    ASSERT_FALSE(trackExists);
}

TEST_F(PixelTest,given_anExistingTrackInAPixel_when_fetchingTheTrackByItsID_then_returnsAPointerToThisTrack){
    Pixel pixel(SOME_ID);
    pixel.addTrack(SOME_TRACK);
    auto trackPosition = 0;
    Track* expectedTrackPointer = &pixel.getTracks()->at(trackPosition);

    auto actualTrackPointer = pixel.fetchTrackByID(SOME_TRACK.getID());

    ASSERT_EQ(expectedTrackPointer, actualTrackPointer);
}

TEST_F(PixelTest,given_anNonExistingTrackInAPixel_when_fetchingTheTrackByItsID_then_returnsANullPointer){
    Pixel pixel(SOME_ID);
    Track* expectedTrackPointer = nullptr;

    auto actualTrackPointer = pixel.fetchTrackByID(SOME_TRACK.getID());

    ASSERT_EQ(expectedTrackPointer, actualTrackPointer);
}

TEST_F(PixelTest,given_aPixelAndATrack_when_addingTheTrackToThePixel_then_trackIsAddedCorrectlyAtRightPositionInTracksArray){
    Pixel pixel(SOME_ID);
    pixel.addTrack(SOME_TRACK);
    auto trackPosition = 0;
    auto expectedAddedTrack = SOME_TRACK;

    auto actualAddedTrack = pixel.getTracks()->at(trackPosition);

    ASSERT_EQ(expectedAddedTrack, actualAddedTrack);
}


TEST_F(PixelTest,given_aPixelWithAFullTracksArrayAndATrack_when_addingTheTrackToThePixel_then_throwsATrackArrayIllegalStoreFullException){
    Pixel pixel(SOME_ID);
    for(auto trackIndex = 0; trackIndex < NUMBER_OF_TRACKS_IN_PIXEL; ++trackIndex){
        pixel.addTrack(SOME_TRACK);
    }

    ASSERT_THROW(pixel.addTrack(SOME_TRACK),std::runtime_error);
}

TEST_F(PixelTest,given_twoIdenticalPixels_when_checkingIfThePixelsAreEqual_then_returnsTrue) {
    auto firstPixel = Pixel(SOME_ID, SOME_TRACKS_ARRAY);
    auto secondPixel = Pixel(SOME_ID, SOME_TRACKS_ARRAY);

    auto tracksAreEqual = (firstPixel == secondPixel);
    auto tracksAreNotEqual = (firstPixel != secondPixel);

    ASSERT_TRUE(tracksAreEqual);
    ASSERT_FALSE(tracksAreNotEqual);
}

TEST_F(PixelTest,given_twoIdenticalPixelsExceptForTheirID_when_checkingIfThePixelsAreEqual_then_returnsFalse) {
    auto firstPixel = Pixel(SOME_ID, SOME_TRACKS_ARRAY);
    auto secondPixel = Pixel(SOME_OTHER_ID, SOME_TRACKS_ARRAY);

    auto tracksAreEqual = (firstPixel == secondPixel);
    auto tracksAreNotEqual = (firstPixel != secondPixel);

    ASSERT_FALSE(tracksAreEqual);
    ASSERT_TRUE(tracksAreNotEqual);
}

TEST_F(PixelTest,given_twoIdenticalPixelsExceptForTheirTracksArray_when_checkingIfThePixelsAreEqual_then_returnsFalse) {
    auto firstPixel = Pixel(SOME_ID, SOME_TRACKS_ARRAY);
    auto secondPixel = Pixel(SOME_ID, SOME_OTHER_TRACKS_ARRAY);

    auto tracksAreEqual = (firstPixel == secondPixel);
    auto tracksAreNotEqual = (firstPixel != secondPixel);

    ASSERT_FALSE(tracksAreEqual);
    ASSERT_TRUE(tracksAreNotEqual);
}

#endif //SPIRITSENSORGATEWAY_PIXELTEST_CPP
