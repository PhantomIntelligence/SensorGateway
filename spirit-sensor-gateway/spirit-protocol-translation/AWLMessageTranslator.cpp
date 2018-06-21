#include "AWLMessageTranslator.h"
#include "UnknownMessageException.h"


AWLMessageTranslator::~AWLMessageTranslator() {

    }
    AWLMessageTranslator::AWLMessageTranslator() {
        setNewSpiritProtocolFrame();
    }

    void AWLMessageTranslator::translateBasicMessage(AWLMessage *awlMessage) {
        switch (awlMessage->id) {
            case FRAME_DONE:
                translateFrameDoneMessage(awlMessage);
            case DETECTION_TRACK :
                translateDetectionTrackMessage(awlMessage);
            case DETECTION_VELOCITY :
                translateDetectionVelocityMessage(awlMessage);
            default:
                throw UnknownMessageException(awlMessage);
        }
    }

    std::vector<SensorFrame> AWLMessageTranslator::returnDoneFrameVector(SensorFrame sensorFrame) const{
        return doneFrameList;
    };

    void AWLMessageTranslator::translateFrameDoneMessage(AWLMessage *awlMessage) {

        this->currentSensorFrame.frameID = awlMessage->data[0];
        this->currentSensorFrame.systemID = awlMessage->data[2];
        addDoneFrame(this->currentSensorFrame);
        setNewSpiritProtocolFrame();

    }

    void AWLMessageTranslator::translateDetectionTrackMessage(AWLMessage *awlMessage) {
            SensorPixel sensorPixel = SensorPixel{};
            SensorTrack sensorTrack = SensorTrack{};

            sensorTrack.id = convertTwoBytesToBigEndian(awlMessage->data[0],awlMessage->data[1]);
            sensorTrack.confidenceLevel = awlMessage->data[5];
            sensorTrack.intensity = convertTwoBytesToBigEndian(awlMessage->data[6],awlMessage->data[7]);

            sensorPixel.id = convertTwoBytesToBigEndian(awlMessage->data[3],awlMessage->data[4]);
            sensorPixel.trackList.push_back(sensorTrack);

            currentSensorFrame.pixelList.push_back(sensorPixel);

        }

    void AWLMessageTranslator::translateDetectionVelocityMessage(AWLMessage *awlMessage) {
        auto trackId = convertTwoBytesToBigEndian(awlMessage->data[0], awlMessage->data[1]);
        SensorTrack* sensorTrack = fetchPointerToTrack(trackId);

        sensorTrack->distance = convertTwoBytesToBigEndian(awlMessage->data[2],awlMessage->data[3]);
        sensorTrack->speed = convertTwoBytesToBigEndian(awlMessage->data[4],awlMessage->data[5]);
        sensorTrack->acceleration = convertTwoBytesToBigEndian(awlMessage->data[6],awlMessage->data[7]);
    }


    void AWLMessageTranslator::setNewSpiritProtocolFrame() {
        this->currentSensorFrame=SensorFrame{};
    }

    void AWLMessageTranslator::addDoneFrame(SensorFrame sensorFrame) {
        doneFrameList.push_back(sensorFrame);
    }

    SensorTrack* AWLMessageTranslator::fetchPointerToTrack(uint16_t trackingID) {
        SensorTrack* pointerToTrack;
        for (int i = 0; i < currentSensorFrame.pixelList.size(); ++i) {
            for (int j = 0; j < currentSensorFrame.pixelList[i].trackList.size(); ++j) {
                if (currentSensorFrame.pixelList[i].trackList[j].id == trackingID){
                    pointerToTrack = &currentSensorFrame.pixelList[i].trackList[j];
                    break;
                }
            }
        }
        return pointerToTrack;
    }