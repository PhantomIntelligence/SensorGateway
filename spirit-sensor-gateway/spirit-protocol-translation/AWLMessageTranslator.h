//
// Created by samuelbouchard on 11/06/18.
//

#ifndef SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H
#define SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H

#include "spirit-sensor-gateway/common/ConstantFunction.h"
#include "spirit-sensor-gateway/domain/AWLMessage.h"
#include "SpiritProtocol.h"
#include <vector>

using namespace MessageID;
    class AWLMessageTranslator {
    public:
        void translateMessage(AWLMessage *awlMessage);
        AWLMessageTranslator();
        ~ AWLMessageTranslator();
        void setNewSpiritProtocolFrame();

    private:
        SensorFrame currentSensorFrame;

        bool checkIfPixelExist(uint16_t pixelID,SensorFrame sensorFrame) const;
        bool checkIfTrackExist(uint16_t trackID, SensorFrame sensorFrame) const;


        void addNewPixelToFrame(SensorFrame sensorFrame,SensorPixel sensorPixel);

        SensorTrack* fetchTrack(uint16_t trackingID) const;

        SensorPixel fetchPixel(uint16_t pixelID) const;

        void translateFrameDoneMessage(AWLMessage *awlMessage);

        void translateDetectionTrackMessage(AWLMessage *awlMessage);

        void translateDetectionVelocityMessagee(AWLMessage *awlMessage);

        std::string translateUnkownMessage(AWLMessage *awlMessage);

        SensorFrame sendDoneFrame(SensorFrame sensorFrame) const;




    };



#endif //SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H
