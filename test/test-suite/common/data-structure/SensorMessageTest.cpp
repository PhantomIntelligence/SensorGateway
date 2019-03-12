#ifndef SENSORGATEWAY_SENSORMESSAGETEST_CPP
#define SENSORGATEWAY_SENSORMESSAGETEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "sensor-gateway/common/data-structure/sensor/AWLStructures.h"
#include "sensor-gateway/common/data-structure/gateway/SensorMessage.hpp"


class SensorMessageTest : public ::testing::Test {

protected:

    using SensorMessage = typename DataFlow::SensorMessage<Sensor::AWL::Structures::MessageDefinition>;
    using MessageId = decltype(SensorMessage::messageId);
    using SensorId = decltype(SensorMessage::sensorId);
    using Pixel = typename SensorMessage::PixelType;
    using Pixels = typename SensorMessage::Pixels;
    using Track = typename Pixel::Track;
    using TracksArray = typename Pixel::TracksArray;
    using PixelId = decltype(Pixel::id);
    using TrackId = decltype(Track::id);

    TrackId const SOME_TRACK_ID = 2;
    Track const SOME_TRACK = Track(SOME_TRACK_ID, 110, 135, 0, 0, 0);
    PixelId const SOME_PIXEL_ID = 1;
    Pixel const SOME_PIXEL = Pixel(SOME_PIXEL_ID, TracksArray({SOME_TRACK}), 1);
    Pixel const SOME_OTHER_PIXEL = Pixel(2, TracksArray({SOME_TRACK}), 1);
    MessageId const SOME_MESSAGE_ID = 64830;
    MessageId const SOME_OTHER_MESSAGE_ID = 63830;
    SensorId const SOME_SENSOR_ID = 16;
    SensorId const SOME_OTHER_SENSOR_ID = 15;
    typename SensorMessage::Pixels const SOME_PIXELS_ARRAY = {SOME_PIXEL};
    typename SensorMessage::Pixels const SOME_OTHER_PIXELS_ARRAY = {SOME_PIXEL, SOME_OTHER_PIXEL};
};

TEST_F(SensorMessageTest,
       given_aTrackAndAPixelId_when_addingTheTrackToThePixelWithCorrespondingId_then_trackIsAddedCorrectlyAtRightPositionInPixel) {
    SensorMessage sensorMessage;
    Track track;
    auto expectedAddedTrack = track;
    auto trackPosition = 0;

    sensorMessage.addTrackToPixelWithId(SOME_PIXEL_ID, std::move(track));

    auto actualAddedTrack = sensorMessage.getPixels()->at(SOME_PIXEL_ID).getTracks()->at(trackPosition);
    ASSERT_EQ(expectedAddedTrack, actualAddedTrack);
}


TEST_F(SensorMessageTest, given_twoIdenticalSensorMessages_when_checkingIfTheSensorMessagesAreEqual_then_returnsTrue) {
    auto firstSensorMessage = SensorMessage(SOME_MESSAGE_ID, SOME_SENSOR_ID, SOME_PIXELS_ARRAY);
    auto secondSensorMessage = SensorMessage(SOME_MESSAGE_ID, SOME_SENSOR_ID, SOME_PIXELS_ARRAY);

    auto sensorMessagesAreEqual = (firstSensorMessage == secondSensorMessage);
    auto sensorMessagesAreNotEqual = (firstSensorMessage != secondSensorMessage);

    ASSERT_TRUE(sensorMessagesAreEqual);
    ASSERT_FALSE(sensorMessagesAreNotEqual);
}


TEST_F(SensorMessageTest,
       given_twoIdenticalSensorMessagesExceptForTheirMessageId_when_checkingIfTheSensorMessagesAreEqual_then_returnsFalse) {
    auto firstSensorMessage = SensorMessage(SOME_MESSAGE_ID, SOME_SENSOR_ID, SOME_PIXELS_ARRAY);
    auto secondSensorMessage = SensorMessage(SOME_OTHER_MESSAGE_ID, SOME_SENSOR_ID, SOME_PIXELS_ARRAY);

    auto sensorMessagesAreEqual = (firstSensorMessage == secondSensorMessage);
    auto sensorMessagesAreNotEqual = (firstSensorMessage != secondSensorMessage);

    ASSERT_FALSE(sensorMessagesAreEqual);
    ASSERT_TRUE(sensorMessagesAreNotEqual);
}

TEST_F(SensorMessageTest,
       given_twoIdenticalSensorMessagesExceptForTheirSensorId_when_checkingIfTheSensorMessagesAreEqual_then_returnsFalse) {
    auto firstSensorMessage = SensorMessage(SOME_MESSAGE_ID, SOME_SENSOR_ID, SOME_PIXELS_ARRAY);
    auto secondSensorMessage = SensorMessage(SOME_MESSAGE_ID, SOME_OTHER_SENSOR_ID, SOME_PIXELS_ARRAY);

    auto sensorMessagesAreEqual = (firstSensorMessage == secondSensorMessage);
    auto sensorMessagesAreNotEqual = (firstSensorMessage != secondSensorMessage);

    ASSERT_FALSE(sensorMessagesAreEqual);
    ASSERT_TRUE(sensorMessagesAreNotEqual);
}

TEST_F(SensorMessageTest,
       given_twoIdenticalSensorMessagesExceptForTheirPixelsArray_when_checkingIfTheSensorMessagesAreEqual_then_returnsFalse) {
    auto firstSensorMessage = SensorMessage(SOME_MESSAGE_ID, SOME_SENSOR_ID, SOME_PIXELS_ARRAY);
    auto secondSensorMessage = SensorMessage(SOME_MESSAGE_ID, SOME_SENSOR_ID, SOME_OTHER_PIXELS_ARRAY);

    auto sensorMessagesAreEqual = (firstSensorMessage == secondSensorMessage);
    auto sensorMessagesAreNotEqual = (firstSensorMessage != secondSensorMessage);

    ASSERT_FALSE(sensorMessagesAreEqual);
    ASSERT_TRUE(sensorMessagesAreNotEqual);
}

#endif //SENSORGATEWAY_SENSORMESSAGETEST_H
