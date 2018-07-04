#ifndef SPIRITSENSORGATEWAY_PIXELTEST_CPP
#define SPIRITSENSORGATEWAY_PIXELTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "spirit-sensor-gateway/domain/Pixel.h"

using namespace SpiritProtocol;

class PixelTest : public ::testing::Test {

};

TEST_F(PixelTest,given_anExistingTrackInAPixel_when_checkingIfTheTrackExist_then_returnTrue) {
    Track track(0x08,0,0);
    Pixel pixel(0x01);
    pixel.addTrack(track);

    ASSERT_TRUE(pixel.doesTrackExist(track.getID()));
}

TEST_F(PixelTest,given_anExistingTrackInAPixel_when_fetchingTheTrackWithItsId_then_returnAPointerToThisTrack){
    Track track(0x08,0,0);
    Pixel pixel(0x01);
    pixel.addTrack(track);

    auto trackPointer = pixel.fetchTrackByID(track.getID());
    Track* testPointer = &pixel.getTracksAddress()->at(0);

    ASSERT_EQ(trackPointer, testPointer);
}

TEST_F(PixelTest,given_anEmptyPixelAndATrack_when_addingTheTrackToThePixel_then_addTheTrackInThePixelCorrectly){
    Track track(0x08,0,0);
    Pixel pixel(0x01);
    pixel.addTrack(track);

    ASSERT_EQ(track, pixel.getTracksAddress()->at(0));
}
TEST_F(PixelTest,given_aPixelWithAnArrayOfTrackThatIsFull_when_addingOneMoreTrack_then_throwsAnException){
    Pixel pixel(0x01);
    Track track(0x08,0,0);
    for(int i; i < MAXIMUM_NUMBER_OF_TRACK_IN_AWL16_PIXEL; ++i){
        pixel.addTrack(track);
    }

    ASSERT_THROW(pixel.addTrack(track),std::runtime_error);
}

#endif //SPIRITSENSORGATEWAY_PIXELTEST_CPP