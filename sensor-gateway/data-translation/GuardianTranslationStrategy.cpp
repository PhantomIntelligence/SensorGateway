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

#include "GuardianTranslationStrategy.h"
#include "TranslationErrorFactory.h"

using DataTranslation::GuardianTranslationStrategy;
using DataFlow::PixelId;
using DataFlow::MessageId;
using DataFlow::SensorId;
using DataFlow::TrackId;
using DataFlow::Intensity;
using DataFlow::Distance;
using DataFlow::Acceleration;
using DataFlow::Speed;
using DataFlow::ConfidenceLevel;
using Sensor::AWL::END_OF_FRAME;
using Sensor::AWL::DETECTION_TRACK;
using Sensor::AWL::DETECTION_VELOCITY;
using Sensor::AWL::_16::NUMBER_OF_PIXELS_PER_LAYER;
using Sensor::AWL::_16::NUMBER_OF_PIXELS;
using Sensor::AWL::_16::HORIZONTAL_FIELD_OF_VIEW;
using Sensor::AWL::_16::ANGLE_RANGE;

void GuardianTranslationStrategy::translateMessage(SensorMessage&& sensorMessage) {
    switch (sensorMessage.id) {
        case END_OF_FRAME:
            translateEndOfFrameMessage(std::move(sensorMessage));
            break;
        case DETECTION_TRACK :
            translateDetectionTrackMessage(std::move(sensorMessage));
            break;
        case DETECTION_VELOCITY :
            translateDetectionVelocityMessage(std::move(sensorMessage));
            break;
        default:
            ErrorHandling::throwGuardianTranslationError(
                    "translateMessage", ErrorHandling::GatewayErrorCode::UNRECOGNIZED_GUARDIAN_MESSAGE_ID);
    }
}

void GuardianTranslationStrategy::translateRawData(SensorRawData&& sensorRawData) {
    orderRawData(&sensorRawData);

    // Guardian sends little-endian raw data. They should become big-endian for Spirit
    reverseRawDataDefinitionEndianness(&sensorRawData);

    super::ServerRawData translatedRawData(sensorRawData.content);
    RawDataSource::produce(std::move(translatedRawData));
}

void GuardianTranslationStrategy::translateEndOfFrameMessage(SensorMessage&& sensorMessage) {
    MessageId messageId = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[0], sensorMessage.data[1]);
    SensorId sensorId = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[2], sensorMessage.data[3]);
    currentOutputMessage.sensorId = sensorId;
    currentOutputMessage.messageId = messageId;
    MessageSource::produce(std::move(currentOutputMessage));
    currentOutputMessage = ServerMessage::returnDefaultData();
}

void GuardianTranslationStrategy::translateDetectionTrackMessage(SensorMessage&& sensorMessage) {
    PixelId pixelId = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[3], sensorMessage.data[4]);
    addTrackInPixel(std::move(sensorMessage), pixelId);
}

void GuardianTranslationStrategy::addTrackInPixel(SensorMessage&& sensorMessage, PixelId pixelId) {
    TrackId trackId = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[0], sensorMessage.data[1]);
    ConfidenceLevel confidenceLevel = sensorMessage.data[5];
    Intensity intensity = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[6], sensorMessage.data[7]);
    Track track;
    track.id = trackId;
    track.confidenceLevel = confidenceLevel;
    track.intensity = intensity;
    currentOutputMessage.addTrackToPixelWithId(pixelId, std::move(track));
};

void GuardianTranslationStrategy::translateDetectionVelocityMessage(SensorMessage&& sensorMessage) {
    Distance distance = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[2], sensorMessage.data[3]);
    Speed speed = convertTwoBytesToSignedBigEndian(sensorMessage.data[4], sensorMessage.data[5]);
    Acceleration acceleration = convertTwoBytesToSignedBigEndian(sensorMessage.data[6], sensorMessage.data[7]);
    TrackId trackId = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[0], sensorMessage.data[1]);
    auto track = fetchTrack(trackId);
    track->distance = distance;
    track->speed = speed;
    track->acceleration = acceleration;
}

Track* GuardianTranslationStrategy::fetchTrack(TrackId const& trackId) {
    auto pixels = currentOutputMessage.getPixels();
    for (auto i = 0; i < NUMBER_OF_PIXELS; ++i) {
        auto pixel = &pixels->at(i);
        if (pixel->doesTrackExist(trackId)) {
            return pixel->fetchTrackById(trackId);
        }
    }
    return nullptr;
}

void GuardianTranslationStrategy::reverseRawDataDefinitionEndianness(SensorRawData* sensorRawData) {
    auto originalContent = ServerRawData::Content(sensorRawData->content);
    auto const NUMBER_OF_DATA = SensorRawData::Definitions::SIZE;
    for (auto contentIndex = 0u; contentIndex < NUMBER_OF_DATA; ++contentIndex) {
        sensorRawData->content[contentIndex] = reverseEndiannessOfInt16(originalContent[contentIndex]);
    }
}

void GuardianTranslationStrategy::orderRawData(SensorRawData* sensorRawData) {
    auto const NUMBER_OF_SAMPLES_PER_CHANNEL = SensorRawData::Definitions::NUMBER_OF_SAMPLES_PER_CHANNEL;
    auto const NUMBER_OF_CHANNELS = SensorRawData::Definitions::NUMBER_OF_CHANNELS;
    auto unorderedContent = ServerRawData::Content(sensorRawData->content);
    for (auto ordinalChannelIndex = 0u; ordinalChannelIndex < NUMBER_OF_CHANNELS; ++ordinalChannelIndex) {
        auto channelPositionIndex = SensorStructures::CHANNEL_POSITIONS[ordinalChannelIndex];
        auto originStartPosition = unorderedContent.begin() + channelPositionIndex * NUMBER_OF_SAMPLES_PER_CHANNEL;
        auto destinationStartPosition =
                sensorRawData->content.begin() + ordinalChannelIndex * NUMBER_OF_SAMPLES_PER_CHANNEL;
        std::copy_n(originStartPosition, NUMBER_OF_SAMPLES_PER_CHANNEL, destinationStartPosition);
    }
}

