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

#include "AWLTranslationStrategy.h"
#include "TranslationErrorFactory.h"

using DataTranslation::AWLTranslationStrategy;
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

void AWLTranslationStrategy::translateMessage(SensorMessage&& sensorMessage) {
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
            ErrorHandling::throwAWLTranslationError("translateMessage",
                                                    ErrorHandling::GatewayErrorCode::UNRECOGNIZED_AWL_MESSAGE_ID);
    }
}

void AWLTranslationStrategy::translateRawData(SensorRawData&& serverRawData) {
    // TODO
}

void AWLTranslationStrategy::translateEndOfFrameMessage(SensorMessage&& sensorMessage) {
    MessageId messageId = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[0], sensorMessage.data[1]);
    SensorId sensorId = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[2], sensorMessage.data[3]);
    currentOutputMessage.messageId = messageId;
    currentOutputMessage.sensorId = sensorId;
    MessageSource::produce(std::move(currentOutputMessage));
    currentOutputMessage = ServerMessage::returnDefaultData();
}

void AWLTranslationStrategy::translateDetectionTrackMessage(SensorMessage&& sensorMessage) {
    PixelId pixelId = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[3], sensorMessage.data[4]);
    addTrackInPixel(std::move(sensorMessage), pixelId);
}

void AWLTranslationStrategy::addTrackInPixel(SensorMessage&& sensorMessage, PixelId pixelId) {
    TrackId trackId = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[0], sensorMessage.data[1]);
    ConfidenceLevel confidenceLevel = sensorMessage.data[5];
    Intensity intensity = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[6], sensorMessage.data[7]);
    Track track;
    track.id = trackId;
    track.confidenceLevel = confidenceLevel;
    track.intensity = intensity;
    currentOutputMessage.addTrackToPixelWithId(pixelId, std::move(track));
};

void AWLTranslationStrategy::translateDetectionVelocityMessage(SensorMessage&& sensorMessage) {
    Distance distance = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[2], sensorMessage.data[3]);
    Speed speed = convertTwoBytesToSignedBigEndian(sensorMessage.data[4], sensorMessage.data[5]);
    Acceleration acceleration = convertTwoBytesToSignedBigEndian(sensorMessage.data[6], sensorMessage.data[7]);
    TrackId trackId = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[0], sensorMessage.data[1]);
    auto track = fetchTrack(trackId);
    track->distance = distance;
    track->speed = speed;
    track->acceleration = acceleration;
}


Track* AWLTranslationStrategy::fetchTrack(TrackId const& trackId) {
    auto pixels = currentOutputMessage.getPixels();
    for (auto i = 0; i < NUMBER_OF_PIXELS; ++i) {
        auto pixel = &pixels->at(i);
        if (pixel->doesTrackExist(trackId)) {
            return pixel->fetchTrackById(trackId);
        }
    }
    return nullptr;
}
