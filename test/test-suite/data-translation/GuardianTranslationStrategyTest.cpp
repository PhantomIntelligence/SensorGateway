/**
	Copyright 2014-2019 Phantom Intelligence Inc.

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

#include "test/utilities/mock/ArbitraryDataSinkMock.hpp"
#include "test/utilities/assertion/SensorMessageAssertion.hpp"

#include "sensor-gateway/data-translation/GuardianTranslationStrategy.hpp"
#include "sensor-gateway/common/data-structure/sensor/GuardianStructures.h"

using DataFlow::Defaults::Track::DEFAULT_ACCELERATION;
using DataFlow::Defaults::Track::DEFAULT_DISTANCE;
using DataFlow::Defaults::Track::DEFAULT_SPEED;

class GuardianTranslationStrategyTest : public ::testing::Test {

protected:

    using Track = DataFlow::Track;

    using DataStructures = Sensor::Guardian::Structures;
    using GatewayStructures = Sensor::Gateway::Structures<
            DataStructures::MessageDefinition,
            DataStructures::RawDataDefinition,
            DataStructures::ControlMessageDefinition
    >;

    using GuardianMessage = DataStructures::Message;
    using GuardianRawData = DataStructures::RawData;
    using GatewayMessage = GatewayStructures::Message;
    using GatewayRawData = GatewayStructures::RawData;

    using PixelsArray = typename GatewayMessage::Pixels;

    using GatewayMessageSinkMock = Mock::ArbitraryDataSinkMock<GatewayMessage>;
    using GatewayRawDataSinkMock = Mock::ArbitraryDataSinkMock<GatewayRawData>;
    using GatewayMessageProcessingScheduler = DataFlow::DataProcessingScheduler<GatewayMessage, GatewayMessageSinkMock, ONLY_ONE_PRODUCER>;
    using GatewayRawDataProcessingScheduler = DataFlow::DataProcessingScheduler<GatewayRawData, GatewayRawDataSinkMock, ONLY_ONE_PRODUCER>;

    DataFlow::PixelId const SOME_PIXEL_ID = 11;
    GatewayMessage const BASE_FRAME = GatewayMessage(64829, 16, PixelsArray());
    GatewayMessage const FRAME_AFTER_END_OF_FRAME_MESSAGE_TRANSLATION = BASE_FRAME;
    GatewayMessage const FRAME_AFTER_DETECTION_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION =
            addTrackToGatewayMessage(BASE_FRAME,
                                     Track(14291, DEFAULT_DISTANCE, 379, DEFAULT_SPEED, DEFAULT_ACCELERATION, 96));
    GatewayMessage const FRAME_AFTER_DETECTION_TRACK_AND_VELOCITY_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION =
            addTrackToGatewayMessage(BASE_FRAME, Track(14291, 106, 379, 0, 256, 96));

    GuardianMessage const SOME_DETECTION_TRACK_AWL_MESSAGE = GuardianMessage(10, 2188169, 8,
                                                                             {211, 55, 0, 11, 0, 96, 123, 1});
    GuardianMessage const SOME_VELOCITY_TRACK_AWL_MESSAGE = GuardianMessage(11, 2188169, 8,
                                                                            {211, 55, 106, 0, 0, 0, 0, 1});
    GuardianMessage const SOME_END_FRAME_AWL_MESSAGE = GuardianMessage(9, 2188170, 8, {61, 253, 16, 0, 0, 0, 0, 0});

    GuardianRawData createIdenticalNonNullValuedGuardianRawData() const {
        auto const arbitraryNonNullValue = 42;
        GuardianRawData rawData = GuardianRawData::returnDefaultData();
        auto const NUMBER_OF_DATA = GuardianRawData::Definitions::SIZE;
        rawData.content.fill(arbitraryNonNullValue);
        return rawData;
    }

    GuardianRawData createOrdinalGuardianRawData() const {
        GuardianRawData ordinalRawData = GuardianRawData::returnDefaultData();
        std::iota(ordinalRawData.content.begin(), ordinalRawData.content.end(), 0);
        return ordinalRawData;
    }

    auto reverseDefinitionsEndianness(GuardianRawData::Definitions::Data&& content) const {
        GuardianRawData::Definitions::Data reversedContent;
        auto const NUMBER_OF_DATA = GuardianRawData::Definitions::SIZE;
        for (auto contentIndex = 0u; contentIndex < NUMBER_OF_DATA; ++contentIndex) {
            reversedContent[contentIndex] = reverseEndiannessOfInt16(content[contentIndex]);
        }
        return reversedContent;
    }

    auto orderRawDataAccordingToGuardianChannelPositions(GuardianRawData::Definitions::Data&& content) const {
        auto const NUMBER_OF_SAMPLES_PER_CHANNEL = GuardianRawData::Definitions::NUMBER_OF_SAMPLES_PER_CHANNEL;
        auto const NUMBER_OF_CHANNELS = GuardianRawData::Definitions::NUMBER_OF_CHANNELS;
        GuardianRawData orderedRawData;
        GuardianRawData::Definitions::Data orderedContent = orderedRawData.content;
        for (auto ordinalChannelIndex = 0u; ordinalChannelIndex < NUMBER_OF_CHANNELS; ++ordinalChannelIndex) {
            auto channelPositionIndex = DataStructures::CHANNEL_POSITIONS[ordinalChannelIndex];
            auto originStartPosition = content.begin() + channelPositionIndex * NUMBER_OF_SAMPLES_PER_CHANNEL;
            auto destinationStartPosition =
                    orderedContent.begin() + ordinalChannelIndex * NUMBER_OF_SAMPLES_PER_CHANNEL;
            std::copy_n(originStartPosition, NUMBER_OF_SAMPLES_PER_CHANNEL, destinationStartPosition);
        }

        return orderedContent;
    }

private:
    GatewayMessage const addTrackToGatewayMessage(GatewayMessage sensorMessage, Track track) const {
        GatewayMessage sensorMessageCopy = GatewayMessage(sensorMessage);
        sensorMessageCopy.addTrackToPixelWithId(SOME_PIXEL_ID, std::move(track));
        return sensorMessageCopy;
    }
};

TEST_F(GuardianTranslationStrategyTest,
       given_someEndOfGatewayMessageAWLMessage_when_translatingOnlyThisMessage_then_buildsGatewayMessageWithValidAttributes) {

    auto endOfGatewayMessageAWLMessage = GuardianMessage(SOME_END_FRAME_AWL_MESSAGE);
    auto expectedGatewayMessage = GatewayMessage(FRAME_AFTER_END_OF_FRAME_MESSAGE_TRANSLATION);
    DataTranslation::GuardianTranslationStrategy translationStrategy;
    GatewayMessageSinkMock sensorMessageSinkMock(1);
    GatewayMessageProcessingScheduler scheduler(&sensorMessageSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(endOfGatewayMessageAWLMessage));

    scheduler.terminateAndJoin();
    sensorMessageSinkMock.waitConsumptionToBeReached();
    auto actualGatewayMessage = sensorMessageSinkMock.getConsumedData().front();
    ASSERT_TRUE(Assert::sameSensorMessage(expectedGatewayMessage, actualGatewayMessage));
}

TEST_F(GuardianTranslationStrategyTest,
       given_someDetectionTrackAndEndOfGatewayMessageAWLMessages_when_translatingThoseMessagesInGivenOrder_then_buildsGatewayMessageWithValidAttribute) {

    auto detectionTrackAWLMessage = GuardianMessage(SOME_DETECTION_TRACK_AWL_MESSAGE);
    auto endOfGatewayMessageAWLMessage = GuardianMessage(SOME_END_FRAME_AWL_MESSAGE);
    auto expectedGatewayMessage = GatewayMessage(FRAME_AFTER_DETECTION_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION);
    DataTranslation::GuardianTranslationStrategy translationStrategy;
    GatewayMessageSinkMock sensorMessageSinkMock(1);
    GatewayMessageProcessingScheduler scheduler(&sensorMessageSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(detectionTrackAWLMessage));
    translationStrategy.translateMessage(std::move(endOfGatewayMessageAWLMessage));

    scheduler.terminateAndJoin();
    sensorMessageSinkMock.waitConsumptionToBeReached();
    auto actualGatewayMessage = sensorMessageSinkMock.getConsumedData().front();
    ASSERT_TRUE(Assert::sameSensorMessage(expectedGatewayMessage, actualGatewayMessage));
}

TEST_F(GuardianTranslationStrategyTest,
       given_someDetectionTrackAndDetectionVelocityAndEndOfGatewayMessageAWLMessages_when_translatingThoseMessagesInGivenOrder_then_buildsGatewayMessageWithValidAttribute) {

    auto detectionTrackAWLMessage = GuardianMessage(SOME_DETECTION_TRACK_AWL_MESSAGE);
    auto detectionVelocityAWLMessage = GuardianMessage(SOME_VELOCITY_TRACK_AWL_MESSAGE);
    auto endOfGatewayMessageAWLMessage = GuardianMessage(SOME_END_FRAME_AWL_MESSAGE);
    auto expectedGatewayMessage = GatewayMessage(FRAME_AFTER_DETECTION_TRACK_AND_VELOCITY_TRACK_AND_END_OF_FRAME_MESSAGES_TRANSLATION);
    DataTranslation::GuardianTranslationStrategy translationStrategy;
    GatewayMessageSinkMock sensorMessageSinkMock(1);
    GatewayMessageProcessingScheduler scheduler(&sensorMessageSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(detectionTrackAWLMessage));
    translationStrategy.translateMessage(std::move(detectionVelocityAWLMessage));
    translationStrategy.translateMessage(std::move(endOfGatewayMessageAWLMessage));

    scheduler.terminateAndJoin();
    sensorMessageSinkMock.waitConsumptionToBeReached();
    auto actualGatewayMessage = sensorMessageSinkMock.getConsumedData().front();
    ASSERT_TRUE(Assert::sameSensorMessage(expectedGatewayMessage, actualGatewayMessage));
}

TEST_F(GuardianTranslationStrategyTest,
       given_someEndOfGatewayMessageAWLMessage_when_translatingThisMessage_then_callsProduceOneTime) {

    auto endOfGatewayMessageAWLMessage = GuardianMessage(SOME_END_FRAME_AWL_MESSAGE);
    DataTranslation::GuardianTranslationStrategy translationStrategy;
    GatewayMessageSinkMock sensorMessageSinkMock(1);
    GatewayMessageProcessingScheduler scheduler(&sensorMessageSinkMock);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateMessage(std::move(endOfGatewayMessageAWLMessage));

    scheduler.terminateAndJoin();
    ASSERT_EQ(sensorMessageSinkMock.hasBeenCalledExpectedNumberOfTimes(), 1);
}

TEST_F(GuardianTranslationStrategyTest,
       given_aRawData_when_translateRawData_then_translatedDataIsProduced) {
    auto numberOfDataToProduce = 1u;
    auto rawData = createOrdinalGuardianRawData();

    DataTranslation::GuardianTranslationStrategy translationStrategy;
    GatewayRawDataSinkMock gatewayRawDataSink(numberOfDataToProduce);
    GatewayRawDataProcessingScheduler scheduler(&gatewayRawDataSink);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateRawData(std::move(rawData));

    gatewayRawDataSink.waitConsumptionToBeReached();
    scheduler.terminateAndJoin();

    auto numberOfProducedData = gatewayRawDataSink.getNumberOfConsumptions();
    ASSERT_EQ(numberOfProducedData, numberOfDataToProduce);
}

TEST_F(GuardianTranslationStrategyTest,
       given_aRawData_when_translateRawData_then_theEndiannessOfTheDataUnitsIsFlipped) {
    auto numberOfData = 1u;
    auto guardianRawData = createIdenticalNonNullValuedGuardianRawData();
    auto contentCopy = GuardianRawData::Definitions::Data(guardianRawData.content);

    DataTranslation::GuardianTranslationStrategy translationStrategy;
    GatewayRawDataSinkMock gatewayRawDataSink(numberOfData);
    GatewayRawDataProcessingScheduler scheduler(&gatewayRawDataSink);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateRawData(std::move(guardianRawData));

    auto reversedContent = reverseDefinitionsEndianness(std::move(contentCopy));
    auto expectedGatewayRawData = GatewayRawData(reversedContent);

    gatewayRawDataSink.waitConsumptionToBeReached();
    scheduler.terminateAndJoin();

    auto translatedRawData = gatewayRawDataSink.getConsumedData().back();
    ASSERT_EQ(translatedRawData, expectedGatewayRawData);
}

TEST_F(GuardianTranslationStrategyTest,
       given_aRawData_when_translateRawData_then_theDataIsOrderedAccordingToTheGuardianChannelPositions) {
    auto numberOfData = 1u;
    auto ordinalRawData = createOrdinalGuardianRawData();
    auto contentCopy = GuardianRawData::Definitions::Data(ordinalRawData.content);

    DataTranslation::GuardianTranslationStrategy translationStrategy;
    GatewayRawDataSinkMock gatewayRawDataSink(numberOfData);
    GatewayRawDataProcessingScheduler scheduler(&gatewayRawDataSink);
    translationStrategy.linkConsumer(&scheduler);

    translationStrategy.translateRawData(std::move(ordinalRawData));

    auto reversedContent = reverseDefinitionsEndianness(std::move(contentCopy));
    auto orderedContent = orderRawDataAccordingToGuardianChannelPositions(std::move(reversedContent));
    auto expectedGatewayRawData = GatewayRawData(orderedContent);

    gatewayRawDataSink.waitConsumptionToBeReached();
    scheduler.terminateAndJoin();

    auto translatedRawData = gatewayRawDataSink.getConsumedData().back();
    ASSERT_EQ(translatedRawData, expectedGatewayRawData);
}

#endif //SENSORGATEWAY_GUARDIANTRANSLATIONSTRATEGYTEST_CPP

