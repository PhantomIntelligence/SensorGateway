/**
	Copyright 2014-2018 Phantom Intelligence Inc.
	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at
		http://www.apache.org/licenses/LICENSE-2.0
	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "sensor-gateway/common/data-structure/gateway/Pixel.hpp"


class TrackTest : public ::testing::Test {

protected:

    using Track = DataFlow::Track;
    using TrackId = decltype(Track::id);
    using ConfidenceLevel = decltype(Track::confidenceLevel);
    using Intensity = decltype(Track::intensity);
    using Acceleration = decltype(Track::acceleration);
    using Distance = decltype(Track::distance);
    using Speed = decltype(Track::speed);

    TrackId const SOME_ID = 14275;
    TrackId const SOME_OTHER_ID = 13275;
    ConfidenceLevel const SOME_CONFIDENCE_LEVEL = 56;
    ConfidenceLevel const SOME_OTHER_CONFIDENCE_LEVEL = 96;
    Intensity const SOME_INTENSITY = 139;
    Intensity const SOME_OTHER_INTENSITY = 135;
    Acceleration const SOME_ACCELERATION = 200;
    Acceleration const SOME_OTHER_ACCELERATION = 310;
    Distance const SOME_DISTANCE = 109;
    Distance const SOME_OTHER_DISTANCE = 110;
    Speed const SOME_SPEED = 30;
    Speed const SOME_OTHER_SPEED = 25;

};

TEST_F(TrackTest,given_twoIdenticalTracks_when_checkingIfTheTracksAreEqual_then_returnsTrue) {
    auto firstTrack = Track(SOME_ID, SOME_DISTANCE, SOME_INTENSITY, SOME_SPEED, SOME_ACCELERATION,
                            SOME_CONFIDENCE_LEVEL);
    auto secondTrack = Track(SOME_ID, SOME_DISTANCE, SOME_INTENSITY, SOME_SPEED, SOME_ACCELERATION,
                             SOME_CONFIDENCE_LEVEL);

    auto tracksAreEqual = (firstTrack == secondTrack);
    auto tracksAreNotEqual = (firstTrack != secondTrack);

    ASSERT_TRUE(tracksAreEqual);
    ASSERT_FALSE(tracksAreNotEqual);
}

TEST_F(TrackTest,given_twoIdenticalTracksExceptForTheirId_when_checkingIfTheTracksAreEqual_then_returnsFalse) {
    auto firstTrack = Track(SOME_ID, SOME_DISTANCE, SOME_INTENSITY, SOME_SPEED, SOME_ACCELERATION,
                            SOME_CONFIDENCE_LEVEL);
    auto secondTrack = Track(SOME_OTHER_ID, SOME_DISTANCE, SOME_INTENSITY, SOME_SPEED, SOME_ACCELERATION,
                             SOME_CONFIDENCE_LEVEL);

    auto tracksAreEqual = (firstTrack == secondTrack);
    auto tracksAreNotEqual = (firstTrack != secondTrack);

    ASSERT_FALSE(tracksAreEqual);
    ASSERT_TRUE(tracksAreNotEqual);
}

TEST_F(TrackTest,given_twoIdenticalTracksExceptForTheirConfidenceLevel_when_checkingIfTheTracksAreEqual_then_returnsFalse) {
    auto firstTrack = Track(SOME_ID, SOME_DISTANCE, SOME_INTENSITY, SOME_SPEED, SOME_ACCELERATION,
                            SOME_CONFIDENCE_LEVEL);
    auto secondTrack = Track(SOME_ID, SOME_DISTANCE, SOME_INTENSITY, SOME_SPEED, SOME_ACCELERATION,
                             SOME_OTHER_CONFIDENCE_LEVEL);

    auto tracksAreEqual = (firstTrack == secondTrack);
    auto tracksAreNotEqual = (firstTrack != secondTrack);

    ASSERT_FALSE(tracksAreEqual);
    ASSERT_TRUE(tracksAreNotEqual);
}

TEST_F(TrackTest,given_twoIdenticalTracksExceptForTheirIntensity_when_checkingIfTheTracksAreEqual_then_returnsFalse) {
    auto firstTrack = Track(SOME_ID, SOME_DISTANCE, SOME_INTENSITY, SOME_SPEED, SOME_ACCELERATION,
                            SOME_CONFIDENCE_LEVEL);
    auto secondTrack = Track(SOME_ID, SOME_DISTANCE, SOME_OTHER_INTENSITY, SOME_SPEED, SOME_ACCELERATION,
                             SOME_CONFIDENCE_LEVEL);

    auto tracksAreEqual = (firstTrack == secondTrack);
    auto tracksAreNotEqual = (firstTrack != secondTrack);

    ASSERT_FALSE(tracksAreEqual);
    ASSERT_TRUE(tracksAreNotEqual);
}

TEST_F(TrackTest,given_twoIdenticalTracksExceptForTheirAcceleration_when_checkingIfTheTracksAreEqual_then_returnsFalse) {
    auto firstTrack = Track(SOME_ID, SOME_DISTANCE, SOME_INTENSITY, SOME_SPEED, SOME_ACCELERATION,
                            SOME_CONFIDENCE_LEVEL);
    auto secondTrack = Track(SOME_ID, SOME_DISTANCE, SOME_INTENSITY, SOME_SPEED, SOME_OTHER_ACCELERATION,
                             SOME_CONFIDENCE_LEVEL);

    auto tracksAreEqual = (firstTrack == secondTrack);
    auto tracksAreNotEqual = (firstTrack != secondTrack);

    ASSERT_FALSE(tracksAreEqual);
    ASSERT_TRUE(tracksAreNotEqual);
}

TEST_F(TrackTest,given_twoIdenticalTracksExceptForTheirDistance_when_checkingIfTheTracksAreEqual_then_returnsFalse) {
    auto firstTrack = Track(SOME_ID, SOME_DISTANCE, SOME_INTENSITY, SOME_SPEED, SOME_ACCELERATION,
                            SOME_CONFIDENCE_LEVEL);
    auto secondTrack = Track(SOME_ID, SOME_OTHER_DISTANCE, SOME_INTENSITY, SOME_SPEED, SOME_ACCELERATION,
                             SOME_CONFIDENCE_LEVEL);

    auto tracksAreEqual = (firstTrack == secondTrack);
    auto tracksAreNotEqual = (firstTrack != secondTrack);

    ASSERT_FALSE(tracksAreEqual);
    ASSERT_TRUE(tracksAreNotEqual);
}


TEST_F(TrackTest,given_twoIdenticalTracksExceptForTheirSpeed_when_checkingIfTheTracksAreEqual_then_returnsFalse) {
    auto firstTrack = Track(SOME_ID, SOME_DISTANCE, SOME_INTENSITY, SOME_SPEED, SOME_ACCELERATION,
                            SOME_CONFIDENCE_LEVEL);
    auto secondTrack = Track(SOME_ID, SOME_DISTANCE, SOME_INTENSITY, SOME_OTHER_SPEED, SOME_ACCELERATION,
                             SOME_CONFIDENCE_LEVEL);

    auto tracksAreEqual = (firstTrack == secondTrack);
    auto tracksAreNotEqual = (firstTrack != secondTrack);

    ASSERT_FALSE(tracksAreEqual);
    ASSERT_TRUE(tracksAreNotEqual);
}




