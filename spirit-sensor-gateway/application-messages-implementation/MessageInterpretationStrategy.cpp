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

    AWLFrameDoneMessage MessageInterpretationStrategy::implementFrameDoneMessage(AWLMessage *awlMessage) {
        AWLFrameDoneMessage awlFrameDoneMessage{};

        SpiritProtocolFrame spiritProtocolFrame{};
    }


    AWLDetectionTrackMessage MessageInterpretationStrategy::implementDetectionTrackMessage(AWLMessage *awlMessage) {
        AWLDetectionTrackMessage awlDetectionTrackMessage{};
    }

    AWLDetectionVelocityMessage MessageInterpretationStrategy::implementDetectionVelocityMessagee(AWLMessage *awlMessage) {
        AWLDetectionVelocityMessage awlDetectionVelocityMessage{};
    }

    void MessageInterpretationStrategy::addMessageToFrame(AWLMessage awlMessage,SpiritProtocolFrame spiritProtocolFrame) {
        spiritProtocolFrame.messageList.push_back(awlMessage);

    }
    void MessageInterpretationStrategy::sendCookedFrame(SpiritProtocolFrame spiritProtocolFrame) {

    }
}