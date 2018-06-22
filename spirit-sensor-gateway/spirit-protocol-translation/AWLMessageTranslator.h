/**
	Copyright 2014-2018 Phantom Intelligence Inc.
	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at
		http://www.apache.org/licenses/LICENSE-2.0
	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#ifndef SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H
#define SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H

#include "spirit-sensor-gateway/common/ConstantFunction.h"
#include "spirit-sensor-gateway/domain/AWLMessage.h"
#include "spirit-sensor-gateway/domain/SpiritFrame.h"
#include <vector>
#include <stdexcept>

using namespace MessageID;

    class AWLMessageTranslator {
    public:
        void translateBasicMessage(AWLMessage *awlMessage);
        AWLMessageTranslator();
        ~ AWLMessageTranslator();
        std::vector <SpiritFrame> getSpiritFrames() const;

    private:
        std::vector<SpiritFrame> spiritFrames;
        SpiritFrame * currentSpiritFrame;
        void translateEndOfFrameMessage(AWLMessage *awlMessage);
        void translateDetectionTrackMessage(AWLMessage *awlMessage);
        void translateDetectionVelocityMessage(AWLMessage *awlMessage);
        void addTrackInPixel(AWLMessage *awlMessage, uint16_t spiritPixelID);
        SpiritTrack * fetchSpiritTrack(AWLMessage *awlMessage);
    };

#endif //SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H
