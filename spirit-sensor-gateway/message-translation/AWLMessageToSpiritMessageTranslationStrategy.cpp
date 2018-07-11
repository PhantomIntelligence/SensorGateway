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
using Sensor::AWL::END_OF_FRAME;
using Sensor::AWL::DETECTION_TRACK;
using Sensor::AWL::DETECTION_VELOCITY;

AWLMessageToSpiritMessageTranslationStrategy::AWLMessageToSpiritMessageTranslationStrategy() {
    currentOutputMessage = new Frame();
}

void AWLMessageToSpiritMessageTranslationStrategy::translateBasicMessage(AWLMessage&& inputMessage) {
    switch (inputMessage.id) {
        case END_OF_FRAME:
            translateEndOfFrameMessage(&inputMessage);
            break;
        case DETECTION_TRACK :
            translateDetectionTrackMessage(&inputMessage);
            break;
        case DETECTION_VELOCITY :
            translateDetectionVelocityMessage(&inputMessage);
            break;
        default:
            throw UnknownMessageException(&inputMessage);
    }
}

void AWLMessageToSpiritMessageTranslationStrategy::translateEndOfFrameMessage(AWLMessage* awlMessage) {
    auto frameID = convertTwoBytesToUnsignedBigEndian(awlMessage->data[0], awlMessage->data[1]);
    auto systemID = convertTwoBytesToUnsignedBigEndian(awlMessage->data[2], awlMessage->data[3]);
    currentOutputMessage->setFrameID(frameID);
    currentOutputMessage->setSystemID(systemID);
    produce(std::move(*currentOutputMessage));
    currentOutputMessage = new Frame();
}

void AWLMessageToSpiritMessageTranslationStrategy::translateDetectionTrackMessage(AWLMessage* awlMessage) {
    auto pixelID = convertTwoBytesToUnsignedBigEndian(awlMessage->data[3], awlMessage->data[4]);
    auto pixel = Pixel(pixelID);
    currentOutputMessage->addPixel(pixel);
    addTrackInPixel(awlMessage, pixel.getID());
}

void AWLMessageToSpiritMessageTranslationStrategy::addTrackInPixel(AWLMessage* awlMessage, PixelID pixelID) {
    auto trackID = convertTwoBytesToUnsignedBigEndian(awlMessage->data[0], awlMessage->data[1]);
    auto confidenceLevel = awlMessage->data[5];
    auto intensity = convertTwoBytesToUnsignedBigEndian(awlMessage->data[6], awlMessage->data[7]);
    auto track = Track(trackID, confidenceLevel, intensity);
    auto pixel = currentOutputMessage->fetchPixelByID(pixelID);
    pixel->addTrack(track);
};

void AWLMessageToSpiritMessageTranslationStrategy::translateDetectionVelocityMessage(AWLMessage* awlMessage) {
    auto track = fetchTrack(awlMessage);
    auto distance = convertTwoBytesToUnsignedBigEndian(awlMessage->data[2], awlMessage->data[3]);
    auto speed = convertTwoBytesToSignedBigEndian(awlMessage->data[4], awlMessage->data[5]);
    auto acceleration = convertTwoBytesToSignedBigEndian(awlMessage->data[6], awlMessage->data[7]);
    track->setDistance(distance);
    track->setSpeed(speed);
    track->setAcceleration(acceleration);
}


Track* AWLMessageToSpiritMessageTranslationStrategy::fetchTrack(AWLMessage* awlMessage) const {
    auto trackID = convertTwoBytesToUnsignedBigEndian(awlMessage->data[0], awlMessage->data[1]);
    auto pixels = currentOutputMessage->getPixels();
    for (auto i = 0; i < NUMBER_OF_PIXELS_IN_FRAME; ++i) {
        auto pixel = &pixels->at(i);
        if(pixel->doesTrackExist(trackID)){
            return pixel->fetchTrackByID(trackID);
        }
    }
    return nullptr;
}

Frame AWLMessageToSpiritMessageTranslationStrategy::returnDefaultData() {
    return Frame();
}
