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
#include "sensor-gateway/data-translation/AWLTranslationStrategy.h"

using Defaults::Track::DEFAULT_ACCELERATION;
using Defaults::Track::DEFAULT_DISTANCE;
using Defaults::Track::DEFAULT_SPEED;

using DataTranslation::AWLTranslationStrategy;

using AWL16Structures = Sensor::AWL::Structures;
using AWL16SpiritStructures = Sensor::Spirit::Structures<
        AWL16Structures::AWLMessageDefinition,
        AWL16Structures::AWLRawDataDefinition,
        AWL16Structures::AWLCommandDefinition
>;

using AWLMessage = AWL16Structures::Message;
using SensorRawData = AWL16Structures::RawData;
using SpiritMessage = AWL16SpiritStructures::Message;
using SpiritRawData = AWL16SpiritStructures::RawData;

using PixelsArray = typename SpiritMessage::Pixels;
using DataFlow::PixelId;

class AWLTranslationStrategyTest : public ::testing::Test {

protected:


    using SpiritMessageSinkMock = Mock::ArbitraryDataSinkMock<SpiritMessage>;
    using SpiritRawDataSinkMock = Mock::ArbitraryDataSinkMock<SpiritRawData>;
    using SpiritMessageProcessingScheduler = DataFlow::DataProcessingScheduler<SpiritMessage, SpiritMessageSinkMock, 1>;
    using SpiritRawDataProcessingScheduler = DataFlow::DataProcessingScheduler<SpiritRawData, SpiritRawDataSinkMock, 1>;

    int const FRAME_INDEX = 0;
    PixelId const SOME_PIXEL_ID = 11;
    SpiritMessage const BASE_FRAME = SpiritMessage(64829, 16, PixelsArray());
    SpiritMessage const FRAME_AFTER_END_OF_FRAME_MESSAGE_TRANSLATION = BASE_FRAME;
    SpiritMessage const FRAME_AFTER_DETECTION_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION =
            addTrackToSensorMessage(BASE_FRAME, Track(14291, 96, 379, DEFAULT_ACCELERATION, DEFAULT_DISTANCE,
                                                   DEFAULT_SPEED));
    SpiritMessage const FRAME_AFTER_DETECTION_TRACK_AND_VELOCITY_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION =
            addTrackToSensorMessage(BASE_FRAME, Track(14291, 96, 379, 256, 106, 0));

    AWLMessage const SOME_DETECTION_TRACK_AWL_MESSAGE = AWLMessage(10, 2188169, 8, {211, 55, 0, 11, 0, 96, 123, 1});
    AWLMessage const SOME_VELOCITY_TRACK_AWL_MESSAGE = AWLMessage(11, 2188169, 8, {211, 55, 106, 0, 0, 0, 0, 1});
    AWLMessage const SOME_END_FRAME_AWL_MESSAGE = AWLMessage(9, 2188170, 8, {61, 253, 16, 0, 0, 0, 0, 0});

private:
    SpiritMessage const addTrackToSensorMessage(SpiritMessage sensorMessage, Track track) const {
        SpiritMessage sensorMessageCopy = SpiritMessage(std::move(sensorMessage));
        sensorMessageCopy.addTrackToPixelWithId(SOME_PIXEL_ID, std::move(track));
        return sensorMessageCopy;
    }
};

TEST_F(AWLTranslationStrategyTest,
       given_someEndOfSensorMessageAWLMessage_when_translatingOnlyThisMessage_then_buildsSensorMessageWithValidAttributes) {

    auto endOfSensorMessageAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    auto expectedSensorMessage = FRAME_AFTER_END_OF_FRAME_MESSAGE_TRANSLATION;
    AWLTranslationStrategy translationStrategy;
    SpiritMessageSinkMock sensorMessageSinkMock(1);
    SpiritMessageProcessingScheduler scheduler(&sensorMessageSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(endOfSensorMessageAWLMessage));

    scheduler.terminateAndJoin();
    auto actualSensorMessage = sensorMessageSinkMock.getConsumedData().front();
    ASSERT_EQ(expectedSensorMessage, actualSensorMessage);
}

TEST_F(AWLTranslationStrategyTest,
       given_someDetectionTrackAndEndOfSensorMessageAWLMessages_when_translatingThoseMessagesInGivenOrder_then_buildsSensorMessageWithValidAttribute) {

    auto detectionTrackAWLMessage = SOME_DETECTION_TRACK_AWL_MESSAGE;
    auto endOfSensorMessageAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    auto expectedSensorMessage = FRAME_AFTER_DETECTION_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION;
    AWLTranslationStrategy translationStrategy;
    SpiritMessageSinkMock sensorMessageSinkMock(1);
    SpiritMessageProcessingScheduler scheduler(&sensorMessageSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(detectionTrackAWLMessage));
    translationStrategy.translateMessage(std::move(endOfSensorMessageAWLMessage));

    scheduler.terminateAndJoin();
    auto actualSensorMessage = sensorMessageSinkMock.getConsumedData().front();
    ASSERT_EQ(expectedSensorMessage, actualSensorMessage);
}

TEST_F(AWLTranslationStrategyTest,
       given_someDetectionTrackAndDetectionVelocityAndEndOfSensorMessageAWLMessages_when_translatingThoseMessagesInGivenOrder_then_buildsSensorMessageWithValidAttribute) {

    auto detectionTrackAWLMessage = SOME_DETECTION_TRACK_AWL_MESSAGE;
    auto detectionVelocityAWLMessage = SOME_VELOCITY_TRACK_AWL_MESSAGE;
    auto endOfSensorMessageAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    auto expectedSensorMessage = FRAME_AFTER_DETECTION_TRACK_AND_VELOCITY_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION;
    AWLTranslationStrategy translationStrategy;
    SpiritMessageSinkMock sensorMessageSinkMock(1);
    SpiritMessageProcessingScheduler scheduler(&sensorMessageSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(detectionTrackAWLMessage));
    translationStrategy.translateMessage(std::move(detectionVelocityAWLMessage));
    translationStrategy.translateMessage(std::move(endOfSensorMessageAWLMessage));

    scheduler.terminateAndJoin();
    auto actualSensorMessage = sensorMessageSinkMock.getConsumedData().front();
    ASSERT_EQ(expectedSensorMessage, actualSensorMessage);
}

TEST_F(AWLTranslationStrategyTest,
       given_someEndOfSensorMessageAWLMessage_when_translatingThisMessage_then_callsProduceOneTime) {

    auto endOfSensorMessageAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    AWLTranslationStrategy translationStrategy;
    SpiritMessageSinkMock sensorMessageSinkMock(1);
    SpiritMessageProcessingScheduler scheduler(&sensorMessageSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(endOfSensorMessageAWLMessage));

    scheduler.terminateAndJoin();
    ASSERT_EQ(sensorMessageSinkMock.hasBeenCalledExpectedNumberOfTimes(), 1);
}

#endif //SENSORGATEWAY_AWLTRANSLATIONSTRATEGYTEST_CPP

