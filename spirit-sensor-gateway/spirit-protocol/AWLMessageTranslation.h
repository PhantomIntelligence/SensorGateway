//
// Created by samuelbouchard on 11/06/18.
//

#ifndef SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H
#define SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H

#include "spirit-sensor-gateway/common/ConstantDefinition.h"
#include <spirit-sensor-gateway/domain/Message.h>
#include "SpiritProtocol.h"
#include <vector>

    class AWLMessageTranslation {
    public:
        void translateMessage(AWLMessage *awlMessage);
        AWLMessageTranslation();
        ~ AWLMessageTranslation();
        void setNewSpiritProtocolFrame();

    private:
        SensorFrame currentSensorFrame;

        bool checkIfPixelExist(uint16_t pixelID,SensorFrame sensorFrame);

        void addNewPixelToFrame(SensorFrame sensorFrame,SensorPixel sensorPixel);

        SensorTrack fetchTrack(uint16_t trackingID);

        SensorPixel fetchPixel(uint16_t pixelID);

        void addTrackToPixel(SensorTrack sensorTrack,uint16_t pixelID);

        void translateFrameDoneMessage(AWLMessage *awlMessage);

        void translateDetectionTrackMessage(AWLMessage *awlMessage);

        void translateDetectionVelocityMessagee(AWLMessage *awlMessage);

        void sendDoneFrame(SensorFrame sensorFrame);




    };



#endif //SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H
