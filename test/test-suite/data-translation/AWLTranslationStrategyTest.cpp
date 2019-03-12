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

#include "test/utilities/mock/ArbitraryDataSinkMock.hpp"
#include "test/utilities/assertion/SensorMessageAssertion.hpp"

#include "sensor-gateway/data-translation/AWLTranslationStrategy.h"

using DataFlow::Defaults::Track::DEFAULT_ACCELERATION;
using DataFlow::Defaults::Track::DEFAULT_DISTANCE;
using DataFlow::Defaults::Track::DEFAULT_SPEED;

class AWLTranslationStrategyTest : public ::testing::Test {

protected:

    using Track = DataFlow::Track;

    using AWL16Structures = Sensor::AWL::Structures;
    using AWL16GatewayStructures = Sensor::Gateway::Structures<
            AWL16Structures::MessageDefinition,
            AWL16Structures::RawDataDefinition,
            AWL16Structures::ControlMessageDefinition
    >;

    using AWLMessage = AWL16Structures::Message;
    using SensorRawData = AWL16Structures::RawData;
    using GatewayMessage = AWL16GatewayStructures::Message;
    using GatewayRawData = AWL16GatewayStructures::RawData;

    using PixelsArray = typename GatewayMessage::Pixels;
    using PixelId = DataFlow::PixelId;

    using GatewayMessageSinkMock = Mock::ArbitraryDataSinkMock<GatewayMessage>;
    using GatewayRawDataSinkMock = Mock::ArbitraryDataSinkMock<GatewayRawData>;
    using GatewayMessageProcessingScheduler = DataFlow::DataProcessingScheduler<GatewayMessage, GatewayMessageSinkMock, ONLY_ONE_PRODUCER>;
    using GatewayRawDataProcessingScheduler = DataFlow::DataProcessingScheduler<GatewayRawData, GatewayRawDataSinkMock, ONLY_ONE_PRODUCER>;

    PixelId const SOME_PIXEL_ID = 11;
    GatewayMessage const BASE_FRAME = GatewayMessage(64829, 16, PixelsArray());
    GatewayMessage const FRAME_AFTER_END_OF_FRAME_MESSAGE_TRANSLATION = BASE_FRAME;
    GatewayMessage const FRAME_AFTER_DETECTION_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION =
            addTrackToSensorMessage(BASE_FRAME,
                                    Track(14291, DEFAULT_DISTANCE,
                                          DataTranslation::AWLTranslationStrategy::convertIntensityToSNR(379),
                                          DEFAULT_SPEED,
                                          DEFAULT_ACCELERATION,
                                          96));
    GatewayMessage const FRAME_AFTER_DETECTION_TRACK_AND_VELOCITY_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION =
            addTrackToSensorMessage(BASE_FRAME,
                                    Track(14291, 1.06,
                                          DataTranslation::AWLTranslationStrategy::convertIntensityToSNR(379),
                                          0, 2.56, 96));
    AWLMessage const SOME_DETECTION_TRACK_AWL_MESSAGE = AWLMessage(10, 2188169, 8, {211, 55, 0, 11, 0, 96, 123, 1});
    AWLMessage const SOME_VELOCITY_TRACK_AWL_MESSAGE = AWLMessage(11, 2188169, 8, {211, 55, 106, 0, 0, 0, 0, 1});
    AWLMessage const SOME_END_FRAME_AWL_MESSAGE = AWLMessage(9, 2188170, 8, {61, 253, 16, 0, 0, 0, 0, 0});

private:
    GatewayMessage const addTrackToSensorMessage(GatewayMessage sensorMessage, Track track) const {
        GatewayMessage sensorMessageCopy = GatewayMessage(std::move(sensorMessage));
        sensorMessageCopy.addTrackToPixelWithId(SOME_PIXEL_ID, std::move(track));
        return sensorMessageCopy;
    }
};

TEST_F(AWLTranslationStrategyTest,
       given_someEndOfSensorMessageAWLMessage_when_translatingOnlyThisMessage_then_buildsSensorMessageWithValidAttributes) {

    auto endOfSensorMessageAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    auto expectedSensorMessage = FRAME_AFTER_END_OF_FRAME_MESSAGE_TRANSLATION;
    DataTranslation::AWLTranslationStrategy translationStrategy;
    GatewayMessageSinkMock sensorMessageSinkMock(1);
    GatewayMessageProcessingScheduler scheduler(&sensorMessageSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(endOfSensorMessageAWLMessage));

    sensorMessageSinkMock.waitConsumptionToBeReached();

    scheduler.terminateAndJoin();

    auto actualSensorMessage = sensorMessageSinkMock.getConsumedData().front();

    ASSERT_TRUE(Assert::sameSensorMessage(expectedSensorMessage, actualSensorMessage));
}

TEST_F(AWLTranslationStrategyTest,
       given_someDetectionTrackAndEndOfSensorMessageAWLMessages_when_translatingThoseMessagesInGivenOrder_then_buildsSensorMessageWithValidAttribute) {

    auto detectionTrackAWLMessage = SOME_DETECTION_TRACK_AWL_MESSAGE;
    auto endOfSensorMessageAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    auto expectedSensorMessage = FRAME_AFTER_DETECTION_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION;
    DataTranslation::AWLTranslationStrategy translationStrategy;
    GatewayMessageSinkMock sensorMessageSinkMock(1);
    GatewayMessageProcessingScheduler scheduler(&sensorMessageSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(detectionTrackAWLMessage));
    translationStrategy.translateMessage(std::move(endOfSensorMessageAWLMessage));

    auto actualSensorMessage = sensorMessageSinkMock.getConsumedData().front();

    scheduler.terminateAndJoin();

    ASSERT_TRUE(Assert::sameSensorMessage(expectedSensorMessage, actualSensorMessage));
}

TEST_F(AWLTranslationStrategyTest,
       given_someDetectionTrackAndDetectionVelocityAndEndOfSensorMessageAWLMessages_when_translatingThoseMessagesInGivenOrder_then_buildsSensorMessageWithValidAttribute) {

    auto detectionTrackAWLMessage = AWLMessage(SOME_DETECTION_TRACK_AWL_MESSAGE);
    auto detectionVelocityAWLMessage = AWLMessage(SOME_VELOCITY_TRACK_AWL_MESSAGE);
    auto endOfSensorMessageAWLMessage = AWLMessage(SOME_END_FRAME_AWL_MESSAGE);
    auto expectedSensorMessage = FRAME_AFTER_DETECTION_TRACK_AND_VELOCITY_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION;
    DataTranslation::AWLTranslationStrategy translationStrategy;
    GatewayMessageSinkMock sensorMessageSinkMock(1);
    GatewayMessageProcessingScheduler scheduler(&sensorMessageSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(detectionTrackAWLMessage));
    translationStrategy.translateMessage(std::move(detectionVelocityAWLMessage));
    translationStrategy.translateMessage(std::move(endOfSensorMessageAWLMessage));

    auto actualSensorMessage = sensorMessageSinkMock.getConsumedData().front();

    scheduler.terminateAndJoin();

    ASSERT_TRUE(Assert::sameSensorMessage(expectedSensorMessage, actualSensorMessage));
}

TEST_F(AWLTranslationStrategyTest,
       given_someEndOfSensorMessageAWLMessage_when_translatingThisMessage_then_callsProduceOneTime) {

    auto endOfSensorMessageAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    DataTranslation::AWLTranslationStrategy translationStrategy;
    GatewayMessageSinkMock sensorMessageSinkMock(1);
    GatewayMessageProcessingScheduler scheduler(&sensorMessageSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(endOfSensorMessageAWLMessage));

    scheduler.terminateAndJoin();
    ASSERT_EQ(sensorMessageSinkMock.hasBeenCalledExpectedNumberOfTimes(), 1);
}

#endif //SENSORGATEWAY_AWLTRANSLATIONSTRATEGYTEST_CPP

