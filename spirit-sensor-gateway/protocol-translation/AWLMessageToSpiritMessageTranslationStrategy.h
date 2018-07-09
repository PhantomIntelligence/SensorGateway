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

#include <vector>

#include "spirit-sensor-gateway/domain/Frame.h"
#include "spirit-sensor-gateway/domain/AWLMessage.h"
#include "MessageTranslationStrategy.hpp"


using namespace MessageID;
using namespace DataFlow;

namespace ProtocolTranslation {

    class AWLMessageToSpiritMessageTranslationStrategy final: public MessageTranslationStrategy<AWLMessage, Frame> {
    public:

        AWLMessageToSpiritMessageTranslationStrategy();

        ~ AWLMessageToSpiritMessageTranslationStrategy();

        std::vector<Frame> getFrames() const;

        Frame returnDefaultData();

        void translateBasicMessage(AWLMessage&& inputMessage) override;

    private:
        std::vector<Frame> frames;

        void addTrackInPixel(AWLMessage* awlMessage, PixelID pixelID);

        Track* fetchTrack(AWLMessage* awlMessage) const;

        void translateDetectionTrackMessage(AWLMessage* awlMessage);

        void translateDetectionVelocityMessage(AWLMessage* awlMessage);

        void translateEndOfFrameMessage(AWLMessage* awlMessage);
    };
}
#endif //SPIRITSENSORGATEWAY_MESSAGEIMPLEMENTATIONSTRATEGY_H
