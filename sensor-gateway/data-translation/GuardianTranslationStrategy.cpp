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
#include "UnknownMessageException.h"

using DataTranslation::GuardianTranslationStrategy;
using DataFlow::PixelID;
using DataFlow::FrameID;
using DataFlow::SystemID;
using DataFlow::TrackID;
using DataFlow::Intensity;
using DataFlow::Distance;
using DataFlow::Acceleration;
using DataFlow::Speed;
using DataFlow::ConfidenceLevel;
using Sensor::AWL::END_OF_FRAME;
using Sensor::AWL::DETECTION_TRACK;
using Sensor::AWL::DETECTION_VELOCITY;
using Sensor::AWL::_16::NUMBER_OF_PIXELS_IN_LAYER;
using Sensor::AWL::_16::NUMBER_OF_PIXELS_IN_FRAME;
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
            auto message = UnknownMessageException(std::move(sensorMessage));
            throw std::runtime_error(message.getMessage());
    }
}

void GuardianTranslationStrategy::translateRawData(SensorRawData&& sensorRawData) {
    orderRawData(&sensorRawData);

    // Guardian sends little-endian raw data. They should become big-endian for Spirit
    reverseRawDataContentEndianness(&sensorRawData);

    super::ServerRawData translatedRawData(sensorRawData.content);
    RawDataSource::produce(std::move(translatedRawData));
}

void GuardianTranslationStrategy::translateEndOfFrameMessage(SensorMessage&& sensorMessage) {
    FrameID frameID = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[0], sensorMessage.data[1]);
    SystemID systemID = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[2], sensorMessage.data[3]);
    currentOutputMessage.systemID = systemID;
    currentOutputMessage.frameID = frameID;
    MessageSource::produce(std::move(currentOutputMessage));
    currentOutputMessage = ServerMessage::returnDefaultData();
}

void GuardianTranslationStrategy::translateDetectionTrackMessage(SensorMessage&& sensorMessage) {
    PixelID pixelID = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[3], sensorMessage.data[4]);
    addTrackInPixel(std::move(sensorMessage), pixelID);
}

void GuardianTranslationStrategy::addTrackInPixel(SensorMessage&& sensorMessage, PixelID pixelID) {
    TrackID trackID = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[0], sensorMessage.data[1]);
    ConfidenceLevel confidenceLevel = sensorMessage.data[5];
    Intensity intensity = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[6], sensorMessage.data[7]);
    Track track;
    track.ID = trackID;
    track.confidenceLevel = confidenceLevel;
    track.intensity = intensity;
    currentOutputMessage.addTrackToPixelWithID(pixelID, std::move(track));
};

void GuardianTranslationStrategy::translateDetectionVelocityMessage(SensorMessage&& sensorMessage) {
    Distance distance = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[2], sensorMessage.data[3]);
    Speed speed = convertTwoBytesToSignedBigEndian(sensorMessage.data[4], sensorMessage.data[5]);
    Acceleration acceleration = convertTwoBytesToSignedBigEndian(sensorMessage.data[6], sensorMessage.data[7]);
    TrackID trackID = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[0], sensorMessage.data[1]);
    auto track = fetchTrack(trackID);
    track->distance = distance;
    track->speed = speed;
    track->acceleration = acceleration;
}

Track* GuardianTranslationStrategy::fetchTrack(TrackID const& trackID) {
    auto pixels = currentOutputMessage.getPixels();
    for (auto i = 0; i < NUMBER_OF_PIXELS_IN_FRAME; ++i) {
        auto pixel = &pixels->at(i);
        if (pixel->doesTrackExist(trackID)) {
            return pixel->fetchTrackByID(trackID);
        }
    }
    return nullptr;
}

void GuardianTranslationStrategy::reverseRawDataContentEndianness(SensorRawData* sensorRawData) {
    auto originalContent = ServerRawData::Content(sensorRawData->content);
    auto const NUMBER_OF_DATA = SensorRawData::RawDataContent::SIZE;
    for (auto contentIndex = 0u; contentIndex < NUMBER_OF_DATA; ++contentIndex) {
        sensorRawData->content[contentIndex] = reverseEndiannessOfInt16(originalContent[contentIndex]);
    }
}

void GuardianTranslationStrategy::orderRawData(SensorRawData* sensorRawData) {
    auto const NUMBER_OF_SAMPLES_PER_CHANNEL = SensorRawData::RawDataContent::NUMBER_OF_SAMPLES_PER_CHANNEL;
    auto const NUMBER_OF_CHANNELS = SensorRawData::RawDataContent::NUMBER_OF_CHANNELS;
    auto unorderedContent = ServerRawData::Content(sensorRawData->content);
    for (auto ordinalChannelIndex = 0u; ordinalChannelIndex < NUMBER_OF_CHANNELS; ++ordinalChannelIndex) {
        auto channelPositionIndex = sensorRawData->CHANNEL_POSITIONS[ordinalChannelIndex];
        auto originStartPosition = unorderedContent.begin() + channelPositionIndex * NUMBER_OF_SAMPLES_PER_CHANNEL;
        auto destinationStartPosition =
                sensorRawData->content.begin() + ordinalChannelIndex * NUMBER_OF_SAMPLES_PER_CHANNEL;
        std::copy_n(originStartPosition, NUMBER_OF_SAMPLES_PER_CHANNEL, destinationStartPosition);
    }
}

