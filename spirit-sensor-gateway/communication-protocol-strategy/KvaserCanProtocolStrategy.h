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

#ifndef SPIRITSENSORGATEWAY_KVASERCANPROTOCOLSTRATEGY_H
#define SPIRITSENSORGATEWAY_KVASERCANPROTOCOLSTRATEGY_H

#include <canlib.h>
#include "CommunicationProtocolStrategy.hpp"



namespace CommunicationProtocolStrategy {
    class KvaserCanProtocolStrategy : public CommunicationProtocolStrategy {
        const unsigned long CANLIB_READ_WAIT_INFINITE_DELAY = (unsigned long) -1;
    public:
        KvaserCanProtocolStrategy();
        ~ KvaserCanProtocolStrategy();
        AWLMessage readMessage();
        void closeConnection();

    private:
        struct CanMessage {
            long id;
            unsigned long timestamp;
            unsigned int flags;
            unsigned int length;
            uint8_t data[MAX_DATA_IN_AWL_MESSAGE];
        };
        void initializeCanConnection();
        canHandle communicationChannel;
        AWLMessage convertCanMessageToAwlMessage(CanMessage canMessage);
    };
}
#endif //SPIRITSENSORGATEWAY_KVASERCANPROTOCOLSTRATEGY_H
