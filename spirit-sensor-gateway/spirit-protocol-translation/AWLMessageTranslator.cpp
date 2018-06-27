#include "AWLMessageTranslator.h"
#include "UnknownMessageException.h"


AWLMessageTranslator::AWLMessageTranslator() {
    currentFrame = new Frame();
}

AWLMessageTranslator::~AWLMessageTranslator() {
}

std::vector<Frame> AWLMessageTranslator::getFrames() const{
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
    currentFrame->setFrameID(convertTwoBytesToBigEndian(awlMessage->data[0],awlMessage->data[1]));
    currentFrame->setSystemID(convertTwoBytesToBigEndian(awlMessage->data[2],awlMessage->data[3]));
    frames.push_back(*currentFrame);
    currentFrame = new Frame();
}

void AWLMessageTranslator::translateDetectionTrackMessage(AWLMessage* awlMessage) {
    PixelID pixelID = convertTwoBytesToBigEndian(awlMessage->data[3],awlMessage->data[4]);
    Pixel pixel = Pixel(pixelID);
    currentFrame->addPixel(pixel);
    addTrackInPixel(awlMessage, pixelID);
}

void AWLMessageTranslator::addTrackInPixel(AWLMessage* awlMessage, PixelID pixelID){
    TrackID trackID = convertTwoBytesToBigEndian(awlMessage->data[0],awlMessage->data[1]);
    ConfidenceLevel confidenceLevel = awlMessage->data[5];
    Intensity intensity = convertTwoBytesToBigEndian(awlMessage->data[6],awlMessage->data[7]);
    Track track = Track(trackID, confidenceLevel, intensity);
    Pixel* pixel = currentFrame-> fetchPixelByID(pixelID);
    pixel->addTrack(track);
};

void AWLMessageTranslator::translateDetectionVelocityMessage(AWLMessage* awlMessage) {
    Track* track = fetchTrack(awlMessage);
    track->setDistance(convertTwoBytesToBigEndian(awlMessage->data[2],awlMessage->data[3]));
    track->setSpeed(convertTwoBytesToBigEndian(awlMessage->data[4],awlMessage->data[5]));
    track->setAcceleration(convertTwoBytesToBigEndian(awlMessage->data[6],awlMessage->data[7]));
}


Track* AWLMessageTranslator::fetchTrack(AWLMessage* awlMessage) {
    Track* fetchedTrack = nullptr;
    TrackID trackID = convertTwoBytesToBigEndian(awlMessage->data[0], awlMessage->data[1]);
    for (auto pixel : currentFrame->getPixels()) {
        bool trackExists = pixel.second.doesTrackExist(trackID);
        if (trackExists){
            Track* track = pixel.second.fetchTrackByID(trackID);
            fetchedTrack = track;
        }
    }
    return fetchedTrack;
}
