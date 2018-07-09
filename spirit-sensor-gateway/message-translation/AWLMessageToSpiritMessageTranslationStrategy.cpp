#include "AWLMessageToSpiritMessageTranslationStrategy.h"
#include "UnknownMessageException.h"

using MessageTranslation::AWLMessageToSpiritMessageTranslationStrategy;

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
    currentOutputMessage->setFrameID(convertTwoBytesToUnsignedBigEndian(awlMessage->data[0], awlMessage->data[1]));
    currentOutputMessage->setSystemID(convertTwoBytesToUnsignedBigEndian(awlMessage->data[2], awlMessage->data[3]));
    produce(std::move(*currentOutputMessage));
    currentOutputMessage = new Frame();
}

void AWLMessageToSpiritMessageTranslationStrategy::translateDetectionTrackMessage(AWLMessage* awlMessage) {
    PixelID pixelID = convertTwoBytesToUnsignedBigEndian(awlMessage->data[3], awlMessage->data[4]);
    Pixel pixel = Pixel(pixelID);
    currentOutputMessage->addPixel(pixel);
    addTrackInPixel(awlMessage, pixel.getID());
}

void AWLMessageToSpiritMessageTranslationStrategy::addTrackInPixel(AWLMessage* awlMessage, PixelID pixelID) {
    TrackID trackID = convertTwoBytesToUnsignedBigEndian(awlMessage->data[0], awlMessage->data[1]);
    ConfidenceLevel confidenceLevel = awlMessage->data[5];
    Intensity intensity = convertTwoBytesToUnsignedBigEndian(awlMessage->data[6], awlMessage->data[7]);
    Track track = Track(trackID, confidenceLevel, intensity);
    Pixel* pixel = currentOutputMessage->fetchPixelByID(pixelID);
    pixel->addTrack(track);
};

void AWLMessageToSpiritMessageTranslationStrategy::translateDetectionVelocityMessage(AWLMessage* awlMessage) {
    auto track = fetchTrack(awlMessage);
    track->setDistance(convertTwoBytesToUnsignedBigEndian(awlMessage->data[2], awlMessage->data[3]));
    track->setSpeed(convertTwoBytesToSignedBigEndian(awlMessage->data[4], awlMessage->data[5]));
    track->setAcceleration(convertTwoBytesToSignedBigEndian(awlMessage->data[6], awlMessage->data[7]));
}


Track* AWLMessageToSpiritMessageTranslationStrategy::fetchTrack(AWLMessage* awlMessage) const {
    TrackID trackID = convertTwoBytesToUnsignedBigEndian(awlMessage->data[0], awlMessage->data[1]);
    auto pixels = currentOutputMessage->getPixels();
    for (auto i = 0; i < NUMBER_OF_PIXELS_IN_AWL16_FRAME; ++i) {
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
