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

#ifndef SPIRITSENSORGATEWAY_COMMUNICATIONPROTOCOLSTRATEGY_H
#define SPIRITSENSORGATEWAY_COMMUNICATIONPROTOCOLSTRATEGY_H

#include "spirit-sensor-gateway/domain/AWLMessage.h"


namespace SensorMessageTranslation {

    class CommunicationProtocolStrategy {


    public:
        virtual AWLMessage readMessage() = 0;

        virtual void openConnection()=0;

        virtual void closeConnection()=0;

    };
}

#endif //SPIRITSENSORGATEWAY_COMMUNICATIONPROTOCOLSTRATEGY_H
