#ifndef SPIRITSENSORGATEWAY_PIXELTEST_CPP
#define SPIRITSENSORGATEWAY_PIXELTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "spirit-sensor-gateway/domain/Pixel.h"

using namespace SpiritProtocol;

class PixelTest : public ::testing::Test {

};

TEST_F(PixelTest,given_anExistingTrackInAPixel_when_checkingIfTheTrackExists_then_returnTrue) {
    Track track(8,0,0);
    Pixel pixel(1);
    pixel.addTrack(track);

    ASSERT_TRUE(pixel.doesTrackExist(track.getID()));
}

TEST_F(PixelTest,given_anExistingTrackInAPixel_when_fetchingTheTracByItsId_then_returnAPointerToThisTrack){
    Track track(8,0,0);
    Pixel pixel(1);
    pixel.addTrack(track);

    auto actualTrackPointer = pixel.fetchTrackByID(track.getID());
    Track* expectedTrackPointer = &pixel.getTracksAddress()->at(0);

    ASSERT_EQ(expectedTrackPointer, actualTrackPointer);
}

TEST_F(PixelTest,given_anEmptyPixelAndATrack_when_addingTheTrackToThePixel_then_trackIsAddedCorrectly){
    Track track(8,0,0);
    Pixel pixel(1);
    pixel.addTrack(track);

    ASSERT_EQ(track, pixel.getTracksAddress()->at(0));
}

TEST_F(PixelTest,given_aPixelWithAFullArrayOfTrack_when_addingOneMoreTrack_then_throwsATrackArrayIllegalStoreFullException){
    Pixel pixel(1);
    Track track(8,0,0);
    for(int i; i < MAXIMUM_NUMBER_OF_TRACKS_IN_AWL16_PIXEL; ++i){
        pixel.addTrack(track);
    }

    ASSERT_THROW(pixel.addTrack(track),std::runtime_error);
}

TEST_F(PixelTest,given_twoPixelWithTheSameAttributes_when_checkingIfTheyAreEqual_then_returnTrue){
    Pixel firstPixel(18);
    Pixel secondPixel(18);

    auto pixelsAreEqual = (firstPixel==secondPixel);

    ASSERT_TRUE(pixelsAreEqual);
}

TEST_F(PixelTest,given_twoPixelWithDifferentAttributes_when_checkingIfTheyAreEqual_then_returnFalse){
    Pixel firstPixel(18);
    Pixel secondPixel(19);

    auto pixelsAreEqual = (firstPixel==secondPixel);

    ASSERT_FALSE(pixelsAreEqual);
}

#endif //SPIRITSENSORGATEWAY_PIXELTEST_CPP