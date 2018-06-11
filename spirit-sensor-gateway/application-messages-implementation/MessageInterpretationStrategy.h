//
// Created by samuelbouchard on 11/06/18.
//

#ifndef SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H
#define SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H

#include "spirit-sensor-gateway/common/ConstantDefinition.h"
#include <spirit-sensor-gateway/domain/Message.h>

namespace MessageInterpretationStrategy {
    void implementMessage(AWLMessage *awlMessage);

    AWLFrameDoneMessage implementFrameDoneMessage(AWLMessage *awlMessage);

    AWLDetectionTrackMessage implementDetectionTrackMessage(AWLMessage *awlMessage);

    AWLDetectionVelocityMessage implementDetectionVelocityMessagee(AWLMessage *awlMessage);
}

#endif //SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H
