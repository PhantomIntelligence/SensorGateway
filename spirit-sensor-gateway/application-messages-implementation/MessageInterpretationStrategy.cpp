//
// Created by samuelbouchard on 11/06/18.
//

#include "MessageInterpretationStrategy.h"

namespace MessageInterpretationStrategy {
    void implementMessage(AWLMessage *awlMessage) {
        switch (awlMessage->messageID) {
            case FRAME_DONE:
                implementFrameDoneMessage(awlMessage);
            case DETECTION_TRACK :
                implementDetectionTrackMessage(awlMessage);
            case DETECTION_VELOCITY :
                implementDetectionVelocityMessagee(awlMessage);
        }
    }

    AWLFrameDoneMessage implementFrameDoneMessage(AWLMessage *awlMessage) {
        AWLFrameDoneMessage awlFrameDoneMessage{};
    }


    AWLDetectionTrackMessage implementDetectionTrackMessage(AWLMessage *awlMessage) {
        AWLDetectionTrackMessage awlDetectionTrackMessage{};
    }

    AWLDetectionVelocityMessage implementDetectionVelocityMessagee(AWLMessage *awlMessage) {
        AWLDetectionVelocityMessage awlDetectionVelocityMessage{};
    }
}