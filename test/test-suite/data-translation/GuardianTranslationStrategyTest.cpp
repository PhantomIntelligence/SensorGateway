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


#ifndef SENSORGATEWAY_GUARDIANTRANSLATIONSTRATEGYTEST_CPP
#define SENSORGATEWAY_GUARDIANTRANSLATIONSTRATEGYTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "test/utilities/mock/FrameSinkMock.h"
#include "test/utilities/mock/ArbitraryDataSinkMock.hpp"
#include "sensor-gateway/data-translation/GuardianTranslationStrategy.h"

using Defaults::Track::DEFAULT_ACCELERATION;
using Defaults::Track::DEFAULT_DISTANCE;
using Defaults::Track::DEFAULT_SPEED;

using DataTranslation::GuardianTranslationStrategy;
using DataTranslation::GuardianStructures;
using DataTranslation::GuardianSpiritStructures;

using GuardianMessage = GuardianStructures::Message;
using GuardianRawData = GuardianStructures::RawData;
using SpiritMessage = GuardianSpiritStructures::Message;
using SpiritRawData = GuardianSpiritStructures::RawData;

using DataFlow::PixelsArray;
using DataFlow::PixelID;

class GuardianTranslationStrategyTest : public ::testing::Test {

protected:

    using SpiritMessageSinkMock = Mock::FrameSinkMock;
    using SpiritRawDataSinkMock = Mock::ArbitraryDataSinkMock<SpiritRawData>;
    using SpiritMessageProcessingScheduler = Mock::FrameProcessingScheduler;
    using SpiritRawDataProcessingScheduler = DataFlow::DataProcessingScheduler<SpiritRawData, SpiritRawDataSinkMock, 1>;

    int const FRAME_INDEX = 0;
    PixelID const SOME_PIXEL_ID = 11;
    SpiritMessage const BASE_FRAME = SpiritMessage(64829, 16, PixelsArray());
    SpiritMessage const FRAME_AFTER_END_OF_FRAME_MESSAGE_TRANSLATION = BASE_FRAME;
    SpiritMessage const FRAME_AFTER_DETECTION_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION =
            addTrackToSpiritMessage(BASE_FRAME, Track(14291, 96, 379, DEFAULT_ACCELERATION, DEFAULT_DISTANCE,
                                                      DEFAULT_SPEED));
    SpiritMessage const FRAME_AFTER_DETECTION_TRACK_AND_VELOCITY_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION =
            addTrackToSpiritMessage(BASE_FRAME, Track(14291, 96, 379, 256, 106, 0));

    GuardianMessage const SOME_DETECTION_TRACK_AWL_MESSAGE = GuardianMessage(10, 2188169, 8,
                                                                             {211, 55, 0, 11, 0, 96, 123, 1});
    GuardianMessage const SOME_VELOCITY_TRACK_AWL_MESSAGE = GuardianMessage(11, 2188169, 8,
                                                                            {211, 55, 106, 0, 0, 0, 0, 1});
    GuardianMessage const SOME_END_FRAME_AWL_MESSAGE = GuardianMessage(9, 2188170, 8, {61, 253, 16, 0, 0, 0, 0, 0});

    GuardianRawData createOrderedGuardianRawData() const {
        GuardianRawData orderedRawData = GuardianRawData::returnDefaultData();
        auto const NUMBER_OF_DATA = GuardianRawData::RawDataContent::SIZE;
        for (auto dataIndex = 0u; dataIndex < NUMBER_OF_DATA; ++dataIndex) {
            orderedRawData.content[dataIndex] = static_cast<GuardianRawData::RawDataContent::ValueType>(dataIndex);
        }
        return orderedRawData;
    }

private:
    SpiritMessage const addTrackToSpiritMessage(SpiritMessage frame, Track track) const {
        SpiritMessage frameCopy = SpiritMessage(frame);
        frameCopy.addTrackToPixelWithID(SOME_PIXEL_ID, std::move(track));
        return frameCopy;
    }
};

TEST_F(GuardianTranslationStrategyTest,
       given_someEndOfSpiritMessageAWLMessage_when_translatingOnlyThisMessage_then_buildsSpiritMessageWithValidAttributes) {

    auto endOfSpiritMessageAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    auto expectedSpiritMessage = FRAME_AFTER_END_OF_FRAME_MESSAGE_TRANSLATION;
    GuardianTranslationStrategy translationStrategy;
    SpiritMessageSinkMock frameSinkMock(1);
    SpiritMessageProcessingScheduler scheduler(&frameSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(endOfSpiritMessageAWLMessage));

    scheduler.terminateAndJoin();
    auto actualSpiritMessage = frameSinkMock.getConsumedData().at(FRAME_INDEX);
    ASSERT_EQ(expectedSpiritMessage, actualSpiritMessage);
}

TEST_F(GuardianTranslationStrategyTest,
       given_someDetectionTrackAndEndOfSpiritMessageAWLMessages_when_translatingThoseMessagesInGivenOrder_then_buildsSpiritMessageWithValidAttribute) {

    auto detectionTrackAWLMessage = SOME_DETECTION_TRACK_AWL_MESSAGE;
    auto endOfSpiritMessageAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    auto expectedSpiritMessage = FRAME_AFTER_DETECTION_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION;
    GuardianTranslationStrategy translationStrategy;
    SpiritMessageSinkMock frameSinkMock(1);
    SpiritMessageProcessingScheduler scheduler(&frameSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(detectionTrackAWLMessage));
    translationStrategy.translateMessage(std::move(endOfSpiritMessageAWLMessage));

    scheduler.terminateAndJoin();
    auto actualSpiritMessage = frameSinkMock.getConsumedData().at(FRAME_INDEX);
    ASSERT_EQ(expectedSpiritMessage, actualSpiritMessage);
}

TEST_F(GuardianTranslationStrategyTest,
       given_someDetectionTrackAndDetectionVelocityAndEndOfSpiritMessageAWLMessages_when_translatingThoseMessagesInGivenOrder_then_buildsSpiritMessageWithValidAttribute) {

    auto detectionTrackAWLMessage = SOME_DETECTION_TRACK_AWL_MESSAGE;
    auto detectionVelocityAWLMessage = SOME_VELOCITY_TRACK_AWL_MESSAGE;
    auto endOfSpiritMessageAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    auto expectedSpiritMessage = FRAME_AFTER_DETECTION_TRACK_AND_VELOCITY_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION;
    GuardianTranslationStrategy translationStrategy;
    SpiritMessageSinkMock frameSinkMock(1);
    SpiritMessageProcessingScheduler scheduler(&frameSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(detectionTrackAWLMessage));
    translationStrategy.translateMessage(std::move(detectionVelocityAWLMessage));
    translationStrategy.translateMessage(std::move(endOfSpiritMessageAWLMessage));

    scheduler.terminateAndJoin();
    auto actualSpiritMessage = frameSinkMock.getConsumedData().at(FRAME_INDEX);
    ASSERT_EQ(expectedSpiritMessage, actualSpiritMessage);
}

TEST_F(GuardianTranslationStrategyTest,
       given_someEndOfSpiritMessageAWLMessage_when_translatingThisMessage_then_callsProduceOneTime) {

    auto endOfSpiritMessageAWLMessage = SOME_END_FRAME_AWL_MESSAGE;
    GuardianTranslationStrategy translationStrategy;
    SpiritMessageSinkMock frameSinkMock(1);
    SpiritMessageProcessingScheduler scheduler(&frameSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(endOfSpiritMessageAWLMessage));

    scheduler.terminateAndJoin();
    ASSERT_EQ(frameSinkMock.hasBeenCalledExpectedNumberOfTimes(), 1);
}

TEST_F(GuardianTranslationStrategyTest,
       given_aRawData_when_translateRawData_then_theTranslatedDataIsProduced) {
    auto numberOfData = 1u;
    auto orderedGuardianRawData = createOrderedGuardianRawData();
    auto contentCopy = GuardianRawData::RawDataContent::Data(orderedGuardianRawData.content);
    auto expectedSpiritRawData = SpiritRawData(contentCopy);

    GuardianTranslationStrategy translationStrategy;
    SpiritRawDataSinkMock spiritRawDataSink(numberOfData);
    SpiritRawDataProcessingScheduler scheduler(&spiritRawDataSink);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateRawData(std::move(orderedGuardianRawData));

    spiritRawDataSink.waitConsumptionToBeReached();
    scheduler.terminateAndJoin();

    auto translatedRawData = spiritRawDataSink.getConsumedData().back();
    ASSERT_EQ(translatedRawData, expectedSpiritRawData);
}

#endif //SENSORGATEWAY_GUARDIANTRANSLATIONSTRATEGYTEST_CPP

