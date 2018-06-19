//
// Created by samuelbouchard on 11/06/18.
//

#ifndef SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H
#define SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H

#include "spirit-sensor-gateway/common/ConstantDefinition.h"
#include <spirit-sensor-gateway/domain/Message.h>
#include "SpiritProtocol.h"
#include <vector>

    class MessageTranslation {
    public:
        void translateMessage(AWLMessage *awlMessage);
        MessageTranslation();
        ~ MessageTranslation();
        void setNewSpiritProtocolFrame();

    private:
        SensorFrame currentSensorFrame;

        AWLFrameDoneMessage implementFrameDoneMessage(AWLMessage *awlMessage);

        AWLDetectionTrackMessage implementDetectionTrackMessage(AWLMessage *awlMessage);

        AWLDetectionVelocityMessage implementDetectionVelocityMessagee(AWLMessage *awlMessage);

        void sendDoneFrame(SensorFrame sensorFrame);

        void addMessageToFrame(AWLMessage awlMessage, SensorFrame sensorFrame);



    };



#endif //SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H
