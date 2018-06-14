//
// Created by samuelbouchard on 11/06/18.
//

#include "MessageInterpretationStrategy.h"


MessageInterpretationStrategy::~MessageInterpretationStrategy() {

}
MessageInterpretationStrategy::MessageInterpretationStrategy() {
    setNewSpiritProtocolFrame();
}


 void MessageInterpretationStrategy::implementMessage(AWLMessage *awlMessage) {
        switch (awlMessage->messageID) {
            case FRAME_DONE:
                implementFrameDoneMessage(awlMessage);
            case DETECTION_TRACK :
                implementDetectionTrackMessage(awlMessage);
            case DETECTION_VELOCITY :
                implementDetectionVelocityMessagee(awlMessage);
        }
    }


    AWLFrameDoneMessage MessageInterpretationStrategy::implementFrameDoneMessage(AWLMessage *awlMessage) {
        AWLFrameDoneMessage awlFrameDoneMessage{};


        this->currentSpiritProtocolFrame.frameID = awlFrameDoneMessage.frameID;
        this->currentSpiritProtocolFrame.systemID = awlFrameDoneMessage.systemID;
        addMessageToFrame(awlFrameDoneMessage, this->currentSpiritProtocolFrame);
        sendCookedFrame(this->currentSpiritProtocolFrame);

    }


    AWLDetectionTrackMessage MessageInterpretationStrategy::implementDetectionTrackMessage(AWLMessage *awlMessage) {
        AWLDetectionTrackMessage awlDetectionTrackMessage{};
        addMessageToFrame(awlDetectionTrackMessage,this->currentSpiritProtocolFrame);
    }

    AWLDetectionVelocityMessage MessageInterpretationStrategy::implementDetectionVelocityMessagee(AWLMessage *awlMessage) {
        AWLDetectionVelocityMessage awlDetectionVelocityMessage{};
        addMessageToFrame(awlDetectionVelocityMessage,this->currentSpiritProtocolFrame);

    }

    void MessageInterpretationStrategy::addMessageToFrame(AWLMessage awlMessage,SpiritProtocolFrame spiritProtocolFrame) {
        spiritProtocolFrame.messageList.push_back(awlMessage);

    }
    void MessageInterpretationStrategy::setNewSpiritProtocolFrame() {
        this->currentSpiritProtocolFrame=SpiritProtocolFrame{};
    }
    void MessageInterpretationStrategy::sendCookedFrame(SpiritProtocolFrame spiritProtocolFrame) {

    }
