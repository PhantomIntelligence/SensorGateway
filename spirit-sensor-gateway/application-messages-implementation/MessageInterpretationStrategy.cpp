//
// Created by samuelbouchard on 11/06/18.
//

#include "MessageInterpretationStrategy.h"

using MessageInterpretationStrategy::MessageInterpretationStrategy;

namespace MessageInterpretationStrategy {
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
    MessageInterpretationStrategy::MessageInterpretationStrategy(){
        this->currentSpiritProtocolFrame = SpiritProtocolFrame {};
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
        awlDetectionVelocityMessage.trackingId = awlMessage->messageData[0];
        awlDetectionVelocityMessage.trackDistance = awlMessage->messageData[1];
        awlDetectionVelocityMessage.trackSpeed = awlMessage->messageData[2];
        awlDetectionVelocityMessage.trackAcceleration = awlMessage->messageData[3];
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
}