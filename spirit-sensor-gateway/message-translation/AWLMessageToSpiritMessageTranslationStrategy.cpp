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

#include "AWLMessageToSpiritMessageTranslationStrategy.h"
#include "UnknownMessageException.h"

using MessageTranslation::AWLMessageToSpiritMessageTranslationStrategy;
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

AWLMessageToSpiritMessageTranslationStrategy::AWLMessageToSpiritMessageTranslationStrategy() :
        super() {}

void AWLMessageToSpiritMessageTranslationStrategy::translateMessage(AWLMessage&& inputMessage) {
    switch (inputMessage.id) {
        case END_OF_FRAME:
            translateEndOfFrameMessage(std::forward<INPUT>(inputMessage));
            break;
        case DETECTION_TRACK :
            translateDetectionTrackMessage(std::forward<INPUT>(inputMessage));
            break;
        case DETECTION_VELOCITY :
            translateDetectionVelocityMessage(std::forward<INPUT>(inputMessage));
            break;
        default:
            throw UnknownMessageException(std::forward<INPUT>(inputMessage));
    }
}

void AWLMessageToSpiritMessageTranslationStrategy::translateEndOfFrameMessage(AWLMessage&& awlMessage) {
    FrameID frameID = convertTwoBytesToUnsignedBigEndian(awlMessage.data[0], awlMessage.data[1]);
    SystemID systemID = convertTwoBytesToUnsignedBigEndian(awlMessage.data[2], awlMessage.data[3]);
    currentOutputMessage.systemID = systemID;
    currentOutputMessage.frameID = frameID;
    produce(std::move(currentOutputMessage));
    currentOutputMessage = Frame::returnDefaultData();
}

void AWLMessageToSpiritMessageTranslationStrategy::translateDetectionTrackMessage(AWLMessage&& awlMessage) {
    PixelID pixelID = convertTwoBytesToUnsignedBigEndian(awlMessage.data[3], awlMessage.data[4]);
    addTrackInPixel(std::forward<INPUT>(awlMessage), pixelID);
}

void AWLMessageToSpiritMessageTranslationStrategy::addTrackInPixel(AWLMessage&& awlMessage, PixelID pixelID) {
    TrackID trackID = convertTwoBytesToUnsignedBigEndian(awlMessage.data[0], awlMessage.data[1]);
    ConfidenceLevel confidenceLevel = awlMessage.data[5];
    Intensity intensity = convertTwoBytesToUnsignedBigEndian(awlMessage.data[6], awlMessage.data[7]);
    Track track;
    track.ID = trackID;
    track.confidenceLevel = confidenceLevel;
    track.intensity = intensity;
    currentOutputMessage.addTrackToPixelWithID(pixelID, std::move(track));
};

void AWLMessageToSpiritMessageTranslationStrategy::translateDetectionVelocityMessage(AWLMessage&& awlMessage) {
    Distance distance = convertTwoBytesToUnsignedBigEndian(awlMessage.data[2], awlMessage.data[3]);
    Speed speed = convertTwoBytesToSignedBigEndian(awlMessage.data[4], awlMessage.data[5]);
    Acceleration acceleration = convertTwoBytesToSignedBigEndian(awlMessage.data[6], awlMessage.data[7]);
    TrackID trackID = convertTwoBytesToUnsignedBigEndian(awlMessage.data[0], awlMessage.data[1]);
    auto track = fetchTrack(trackID);
    track->distance = distance;
    track->speed = speed;
    track->acceleration = acceleration;
}


Track* AWLMessageToSpiritMessageTranslationStrategy::fetchTrack(TrackID const& trackID) {
    auto pixels = currentOutputMessage.getPixels();
    for (auto i = 0; i < NUMBER_OF_PIXELS_IN_FRAME; ++i) {
        auto pixel = &pixels->at(i);
        if (pixel->doesTrackExist(trackID)) {
            return pixel->fetchTrackByID(trackID);
        }
    }
    return nullptr;
}
