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
#include <limits>

using Sensor::AWL::MAXIMUM_NUMBER_OF_DATA_IN_MESSAGE;

namespace SensorCommunication {
    using DataFlow::AWLMessage;

    class KvaserCanProtocolStrategy : public CommunicationProtocolStrategy<AWLMessage> {

        const unsigned long CANLIB_READ_WAIT_INFINITE_DELAY = std::numeric_limits<int>::infinity();

    public:
        KvaserCanProtocolStrategy();

        ~ KvaserCanProtocolStrategy();

        void openConnection();

        AWLMessage readMessage();

        void closeConnection();

    private:

        struct CanMessage {
            long id;
            unsigned long timestamp;
            unsigned int flags;
            unsigned int length;
            uint8_t data[MAXIMUM_NUMBER_OF_DATA_IN_MESSAGE];
        };
        canHandle communicationChannel;

        AWLMessage convertCanMessageToAwlMessage(CanMessage canMessage);
    };
}
#endif //SPIRITSENSORGATEWAY_KVASERCANPROTOCOLSTRATEGY_H
