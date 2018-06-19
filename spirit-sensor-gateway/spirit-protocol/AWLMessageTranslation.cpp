//
// Created by samuelbouchard on 11/06/18.
//

#include "AWLMessageTranslation.h"


AWLMessageTranslation::~AWLMessageTranslation() {

}
AWLMessageTranslation::AWLMessageTranslation() {
    setNewSpiritProtocolFrame();
}


    void AWLMessageTranslation::translateMessage(AWLMessage *awlMessage) {
        switch (awlMessage->id) {
            case FRAME_DONE:
                translateFrameDoneMessage(awlMessage);
            case DETECTION_TRACK :
                translateDetectionTrackMessage(awlMessage);
            case DETECTION_VELOCITY :
                translateDetectionVelocityMessagee(awlMessage);
        }
    }


    void AWLMessageTranslation::translateFrameDoneMessage(AWLMessage *awlMessage) {

        this->currentSensorFrame.frameID = awlMessage->data[0];
        this->currentSensorFrame.systemID = awlMessage->data[2];
        sendDoneFrame(this->currentSensorFrame);
        setNewSpiritProtocolFrame();

    }


    void AWLMessageTranslation::translateDetectionTrackMessage(AWLMessage *awlMessage) {
        uint16_t pixelIdToCheck;
        if(checkIfPixelExist(pixelIdToCheck,currentSensorFrame)){
            SensorPixel sensorPixel;
        }
        else{

        }

    }

    void AWLMessageTranslation::translateDetectionVelocityMessagee(AWLMessage *awlMessage) {


}

    bool AWLMessageTranslation::checkIfPixelExist(uint16_t pixelID, SensorFrame sensorFrame) {
        for (int i = 0; i < sensorFrame.pixelList.size(); ++i) {
            if(sensorFrame.pixelList[i].id == pixelID){
                return true;
            }
        }
        return false;
    }

    void AWLMessageTranslation::setNewSpiritProtocolFrame() {
        this->currentSensorFrame=SensorFrame{};
    }

    void AWLMessageTranslation::addTrackToPixel(SensorTrack sensorTrack, uint16_t pixelID) {

    }

    void AWLMessageTranslation::sendDoneFrame(SensorFrame sensorFrame) {

    }

    SensorPixel AWLMessageTranslation::fetchPixel(uint16_t pixelID) {
        for (int i = 0; i < currentSensorFrame.pixelList.size(); ++i) {
            if(currentSensorFrame.pixelList[i].id == pixelID){
                return currentSensorFrame.pixelList[i];
            }
        }
    }

    SensorTrack AWLMessageTranslation::fetchTrack(uint16_t trackingID) {

    }