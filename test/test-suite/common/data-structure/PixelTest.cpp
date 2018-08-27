#ifndef SENSORGATEWAY_PIXELTEST_CPP
#define SENSORGATEWAY_PIXELTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "sensor-gateway/common/data-structure/spirit/Pixel.h"

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
    int const SOME_CURRENT_NUMBER_OF_TRACKS = 1;
    int const SOME_OTHER_CURRENT_NUMBER_OF_TRACKS = 2;
};

TEST_F(PixelTest, given_anExistingTrackInAPixel_when_checkingIfTheTrackExists_then_returnsTrue) {
    Pixel pixel(SOME_ID, {SOME_TRACK}, SOME_CURRENT_NUMBER_OF_TRACKS);

    auto trackExists = pixel.doesTrackExist(SOME_TRACK.ID);

    ASSERT_TRUE(trackExists);
}

TEST_F(PixelTest, given_noExistingTrackInAPixel_when_checkingIfTheTrackExists_then_returnsFalse) {
    Pixel pixel;

    auto trackExists = pixel.doesTrackExist(SOME_TRACK.ID);

    ASSERT_FALSE(trackExists);
}

TEST_F(PixelTest, given_anExistingTrackInAPixel_when_fetchingTheTrackByItsID_then_returnsAPointerToThisTrack) {
    Pixel pixel(SOME_ID, {SOME_TRACK}, SOME_CURRENT_NUMBER_OF_TRACKS);
    auto trackPosition = 0;
    Track* expectedTrackPointer = &pixel.getTracks()->at(trackPosition);

    auto actualTrackPointer = pixel.fetchTrackByID(SOME_TRACK.ID);

    ASSERT_EQ(expectedTrackPointer, actualTrackPointer);
}

TEST_F(PixelTest, given_noExistingTrackInAPixel_when_fetchingTheTrackByItsID_then_returnsANullPointer) {
    Pixel pixel;
    Track* expectedTrackPointer = nullptr;

    auto actualTrackPointer = pixel.fetchTrackByID(SOME_TRACK.ID);

    ASSERT_EQ(expectedTrackPointer, actualTrackPointer);
}

TEST_F(PixelTest,
       given_aPixelAndATrack_when_addingTheTrackToThePixel_then_trackIsAddedCorrectlyAtRightPositionInTracksArray) {
    Pixel pixel;
    auto trackCopy = Track(SOME_TRACK);
    pixel.addTrack(std::move(trackCopy));
    auto trackPosition = 0;
    auto expectedAddedTrack = SOME_TRACK;

    auto actualAddedTrack = pixel.getTracks()->at(trackPosition);

    ASSERT_EQ(expectedAddedTrack, actualAddedTrack);
}

TEST_F(PixelTest,
       given_aPixelWithAFullTracksArrayAndATrack_when_addingTheTrackToThePixel_then_throwsATrackArrayIllegalStoreFullException) {
    Pixel pixel;
    for (auto trackIndex = 0; trackIndex < NUMBER_OF_TRACKS_IN_PIXEL; ++trackIndex) {
        auto trackCopy = Track(SOME_TRACK);
        pixel.addTrack(std::move(trackCopy));
    }

    auto trackCopy = Track(SOME_TRACK);
    ASSERT_THROW(pixel.addTrack(std::move(trackCopy)), std::runtime_error);
}

TEST_F(PixelTest, given_twoIdenticalPixels_when_checkingIfThePixelsAreEqual_then_returnsTrue) {
    auto firstPixel = Pixel(SOME_ID, SOME_TRACKS_ARRAY, SOME_CURRENT_NUMBER_OF_TRACKS);
    auto secondPixel = Pixel(SOME_ID, SOME_TRACKS_ARRAY, SOME_CURRENT_NUMBER_OF_TRACKS);

    auto tracksAreEqual = (firstPixel == secondPixel);
    auto tracksAreNotEqual = (firstPixel != secondPixel);

    ASSERT_TRUE(tracksAreEqual);
    ASSERT_FALSE(tracksAreNotEqual);
}

TEST_F(PixelTest, given_twoIdenticalPixelsExceptForTheirID_when_checkingIfThePixelsAreEqual_then_returnsFalse) {
    auto firstPixel = Pixel(SOME_ID, SOME_TRACKS_ARRAY, SOME_CURRENT_NUMBER_OF_TRACKS);
    auto secondPixel = Pixel(SOME_OTHER_ID, SOME_TRACKS_ARRAY, SOME_CURRENT_NUMBER_OF_TRACKS);

    auto tracksAreEqual = (firstPixel == secondPixel);
    auto tracksAreNotEqual = (firstPixel != secondPixel);

    ASSERT_FALSE(tracksAreEqual);
    ASSERT_TRUE(tracksAreNotEqual);
}

TEST_F(PixelTest,
       given_twoIdenticalPixelsExceptForTheirTracksArrayAndCurrentNumberOfTracks_when_checkingIfThePixelsAreEqual_then_returnsFalse) {
    auto firstPixel = Pixel(SOME_ID, SOME_TRACKS_ARRAY, SOME_CURRENT_NUMBER_OF_TRACKS);
    auto secondPixel = Pixel(SOME_ID, SOME_OTHER_TRACKS_ARRAY, SOME_OTHER_CURRENT_NUMBER_OF_TRACKS);

    auto tracksAreEqual = (firstPixel == secondPixel);
    auto tracksAreNotEqual = (firstPixel != secondPixel);

    ASSERT_FALSE(tracksAreEqual);
    ASSERT_TRUE(tracksAreNotEqual);
}


#endif //SENSORGATEWAY_PIXELTEST_CPP