//
// Created by samuelbouchard on 11/06/18.
//

#ifndef SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H
#define SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H

#include "spirit-sensor-gateway/common/ConstantDefinition.h"
#include <spirit-sensor-gateway/domain/Message.h>
#include <vector>

    class MessageInterpretationStrategy {
    public:
        void implementMessage(AWLMessage *awlMessage);
        MessageInterpretationStrategy();
        ~ MessageInterpretationStrategy();
            void setNewSpiritProtocolFrame();

    private:
        struct SpiritProtocolFrame {
            std::vector<AWLMessage> messageList;
            uint16_t frameID;
            uint16_t systemID;
            uint32_t systemErrorFlag;
        };
        SpiritProtocolFrame currentSpiritProtocolFrame;

        AWLFrameDoneMessage implementFrameDoneMessage(AWLMessage *awlMessage);

        AWLDetectionTrackMessage implementDetectionTrackMessage(AWLMessage *awlMessage);

        AWLDetectionVelocityMessage implementDetectionVelocityMessagee(AWLMessage *awlMessage);

        void sendCookedFrame(SpiritProtocolFrame spiritProtocolFrame);

        void addMessageToFrame(AWLMessage awlMessage, SpiritProtocolFrame spiritProtocolFrame);



    };



#endif //SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H
