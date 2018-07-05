#include "AWLMessageTranslator.h"
#include "UnknownMessageException.h"


AWLMessageTranslator::AWLMessageTranslator() {
    currentFrame = new Frame();
}

AWLMessageTranslator::~AWLMessageTranslator() {
}

std::vector<Frame> AWLMessageTranslator::getFrames() const {
    return frames;
};

void AWLMessageTranslator::translateBasicMessage(AWLMessage* awlMessage) {
    switch (awlMessage->id) {
        case FRAME_DONE:
            translateEndOfFrameMessage(awlMessage);
            break;
        case DETECTION_TRACK :
            translateDetectionTrackMessage(awlMessage);
            break;
        case DETECTION_VELOCITY :
            translateDetectionVelocityMessage(awlMessage);
            break;
        default:
            throw UnknownMessageException(awlMessage);
    }
}

void AWLMessageTranslator::translateEndOfFrameMessage(AWLMessage* awlMessage) {
    currentFrame->setFrameID(convertTwoBytesToUnsignedBigEndian(awlMessage->data[0], awlMessage->data[1]));
    currentFrame->setSystemID(convertTwoBytesToUnsignedBigEndian(awlMessage->data[2], awlMessage->data[3]));
    frames.push_back(*currentFrame);
    currentFrame = new Frame();
}

void AWLMessageTranslator::translateDetectionTrackMessage(AWLMessage* awlMessage) {
    PixelID pixelID = convertTwoBytesToUnsignedBigEndian(awlMessage->data[3], awlMessage->data[4]);
    Pixel pixel = Pixel(pixelID);
    currentFrame->addPixel(pixel);
    addTrackInPixel(awlMessage, pixel.getID());
}

void AWLMessageTranslator::addTrackInPixel(AWLMessage* awlMessage, PixelID pixelID) {
    TrackID trackID = convertTwoBytesToUnsignedBigEndian(awlMessage->data[0], awlMessage->data[1]);
    ConfidenceLevel confidenceLevel = awlMessage->data[5];
    Intensity intensity = convertTwoBytesToUnsignedBigEndian(awlMessage->data[6], awlMessage->data[7]);
    Track track = Track(trackID, confidenceLevel, intensity);
    Pixel* pixel = currentFrame->fetchPixelByID(pixelID);
    pixel->addTrack(track);
};

void AWLMessageTranslator::translateDetectionVelocityMessage(AWLMessage* awlMessage) {
    auto track = fetchTrack(awlMessage);
    track->setDistance(convertTwoBytesToUnsignedBigEndian(awlMessage->data[2], awlMessage->data[3]));
    track->setSpeed(convertTwoBytesToSignedBigEndian(awlMessage->data[4], awlMessage->data[5]));
    track->setAcceleration(convertTwoBytesToSignedBigEndian(awlMessage->data[6], awlMessage->data[7]));
}


Track* AWLMessageTranslator::fetchTrack(AWLMessage* awlMessage) const {
    TrackID trackID = convertTwoBytesToUnsignedBigEndian(awlMessage->data[0], awlMessage->data[1]);
    auto pixels = currentFrame->getPixelsAddress();
    for (auto i = 0; i < NUMBER_OF_PIXELS_IN_AWL16_FRAME; ++i) {
        auto pixel = &pixels->at(i);
        if(pixel->doesTrackExist(trackID)){
            return pixel->fetchTrackByID(trackID);
        }
    }
    return nullptr;
}
