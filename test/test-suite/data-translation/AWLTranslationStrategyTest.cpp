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


#ifndef SENSORGATEWAY_AWLTRANSLATIONSTRATEGYTEST_CPP
#define SENSORGATEWAY_AWLTRANSLATIONSTRATEGYTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "test/utilities/mock/SensorMessageSinkMock.h"
#include "sensor-gateway/data-translation/AWLTranslationStrategy.h"

using Defaults::Track::DEFAULT_ACCELERATION;
using Defaults::Track::DEFAULT_DISTANCE;
using Defaults::Track::DEFAULT_SPEED;
using DataTranslation::AWLTranslationStrategy;
using Mock::SensorMessageProcessingScheduler;
using Mock::SensorMessageSinkMock;
using DataFlow::PixelsArray;
using DataFlow::PixelId;
using DataFlow::SensorMessage;
using DataFlow::AWLMessage;



class AWLTranslationStrategyTest : public ::testing::Test {

protected:
    int const FRAME_INDEX = 0;
    PixelId const SOME_PIXEL_ID = 11;
    SensorMessage const BASE_FRAME = SensorMessage(64829, 16, PixelsArray());
    SensorMessage const FRAME_AFTER_END_OF_FRAME_MESSAGE_TRANSLATION = BASE_FRAME;
    SensorMessage const FRAME_AFTER_DETECTION_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION =
            addTrackToSensorMessage(BASE_FRAME, Track(14291, 96, 379, DEFAULT_ACCELERATION, DEFAULT_DISTANCE,
                                                   DEFAULT_SPEED));
    SensorMessage const FRAME_AFTER_DETECTION_TRACK_AND_VELOCITY_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION =
            addTrackToSensorMessage(BASE_FRAME, Track(14291, 96, 379, 256, 106, 0));

    AWLMessage const SOME_DETECTION_TRACK_AWL_MESSAGE = AWLMessage(10, 2188169, 8, {211, 55, 0, 11, 0, 96, 123, 1});
    AWLMessage const SOME_VELOCITY_TRACK_AWL_MESSAGE = AWLMessage(11, 2188169, 8, {211, 55, 106, 0, 0, 0, 0, 1});
    AWLMessage const SOME_END_FRAME_AWL_MESSAGE = AWLMessage(9, 2188170, 8, {61, 253, 16, 0, 0, 0, 0, 0});

private:
    SensorMessage const addTrackToSensorMessage(SensorMessage sensorMessage, Track track) const {
        SensorMessage sensorMessageCopy = SensorMessage(sensorMessage);
        sensorMessageCopy.addTrackToPixelWithId(SOME_PIXEL_ID, std::move(track));
        return sensorMessageCopy;
    }
};

TEST_F(AWLTranslationStrategyTest,
       given_someEndOfSensorMessageAWLMessage_when_translatingOnlyThisMessage_then_buildsSensorMessageWithValidAttributes) {

    auto endOfSensorMessageAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    auto expectedSensorMessage = FRAME_AFTER_END_OF_FRAME_MESSAGE_TRANSLATION;
    AWLTranslationStrategy translationStrategy;
    SensorMessageSinkMock sensorMessageSinkMock(1);
    SensorMessageProcessingScheduler scheduler(&sensorMessageSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(endOfSensorMessageAWLMessage));

    scheduler.terminateAndJoin();
    auto actualSensorMessage = sensorMessageSinkMock.getConsumedData().at(FRAME_INDEX);
    ASSERT_EQ(expectedSensorMessage, actualSensorMessage);
}

TEST_F(AWLTranslationStrategyTest,
       given_someDetectionTrackAndEndOfSensorMessageAWLMessages_when_translatingThoseMessagesInGivenOrder_then_buildsSensorMessageWithValidAttribute) {

    auto detectionTrackAWLMessage = SOME_DETECTION_TRACK_AWL_MESSAGE;
    auto endOfSensorMessageAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    auto expectedSensorMessage = FRAME_AFTER_DETECTION_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION;
    AWLTranslationStrategy translationStrategy;
    SensorMessageSinkMock sensorMessageSinkMock(1);
    SensorMessageProcessingScheduler scheduler(&sensorMessageSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(detectionTrackAWLMessage));
    translationStrategy.translateMessage(std::move(endOfSensorMessageAWLMessage));

    scheduler.terminateAndJoin();
    auto actualSensorMessage = sensorMessageSinkMock.getConsumedData().at(FRAME_INDEX);
    ASSERT_EQ(expectedSensorMessage, actualSensorMessage);
}

TEST_F(AWLTranslationStrategyTest,
       given_someDetectionTrackAndDetectionVelocityAndEndOfSensorMessageAWLMessages_when_translatingThoseMessagesInGivenOrder_then_buildsSensorMessageWithValidAttribute) {

    auto detectionTrackAWLMessage = SOME_DETECTION_TRACK_AWL_MESSAGE;
    auto detectionVelocityAWLMessage = SOME_VELOCITY_TRACK_AWL_MESSAGE;
    auto endOfSensorMessageAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    auto expectedSensorMessage = FRAME_AFTER_DETECTION_TRACK_AND_VELOCITY_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION;
    AWLTranslationStrategy translationStrategy;
    SensorMessageSinkMock sensorMessageSinkMock(1);
    SensorMessageProcessingScheduler scheduler(&sensorMessageSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(detectionTrackAWLMessage));
    translationStrategy.translateMessage(std::move(detectionVelocityAWLMessage));
    translationStrategy.translateMessage(std::move(endOfSensorMessageAWLMessage));

    scheduler.terminateAndJoin();
    auto actualSensorMessage = sensorMessageSinkMock.getConsumedData().at(FRAME_INDEX);
    ASSERT_EQ(expectedSensorMessage, actualSensorMessage);
}

TEST_F(AWLTranslationStrategyTest,
       given_someEndOfSensorMessageAWLMessage_when_translatingThisMessage_then_callsProduceOneTime) {

    auto endOfSensorMessageAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    AWLTranslationStrategy translationStrategy;
    SensorMessageSinkMock sensorMessageSinkMock(1);
    SensorMessageProcessingScheduler scheduler(&sensorMessageSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(endOfSensorMessageAWLMessage));

    scheduler.terminateAndJoin();
    ASSERT_EQ(sensorMessageSinkMock.hasBeenCalledExpectedNumberOfTimes(), 1);
}

#endif //SENSORGATEWAY_AWLTRANSLATIONSTRATEGYTEST_CPP

