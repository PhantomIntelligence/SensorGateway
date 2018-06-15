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

#include "KvaserCanProtocolStrategy.h"
#include <mutex>

using CommunicationProtocolStrategy::KvaserCanProtocolStrategy;


KvaserCanProtocolStrategy::KvaserCanProtocolStrategy(): canCircuitHandle(){
    initializeCanConnection();
}

KvaserCanProtocolStrategy:: ~KvaserCanProtocolStrategy()=default;

void KvaserCanProtocolStrategy::initializeCanConnection() {
    canInitializeLibrary();
    canHandle canCircuitHandle = canOpenChannel(0, canOPEN_EXCLUSIVE);
    canSetBusParams(canCircuitHandle, canBITRATE_1M, 0, 0, 0, 0, 0);
    canSetBusOutputControl(canCircuitHandle, canDRIVER_SILENT);
    canBusOn(canCircuitHandle);
}

AWLMessage KvaserCanProtocolStrategy::readMessage(){
CanMessage canMessage{};
    long id;
    unsigned long timestamp;
    unsigned int flags;
    unsigned int length;
    uint8_t data[MESSAGE_DATA_LENGTH_IN_MESSAGE];
    canReadWait(canCircuitHandle, &id, data, &length, &flags, &timestamp,READ_WAIT_INFINITE);

    canMessage.id =id;
    canMessage.length=length;
    canMessage.flags=flags;
    canMessage.timestamp=timestamp;
    for (auto i = 0; i < MESSAGE_DATA_LENGTH_IN_MESSAGE; ++i) {
        canMessage.data[i] = data[i];
    }

    return convertCanMessageToAwlMessage(canMessage);
}

AWLMessage KvaserCanProtocolStrategy::convertCanMessageToAwlMessage(CanMessage canMessage) {

    AWLMessage awlMessage{};


    awlMessage.id = static_cast<uint64_t >(canMessage.id);
    awlMessage.timestamp = canMessage.timestamp;
    awlMessage.lenght = static_cast<uint8_t >(canMessage.length);
    awlMessage.flags= static_cast<uint64_t >(canMessage.flags);
    for (auto i = 0; i < MESSAGE_DATA_LENGTH_IN_MESSAGE; ++i) {
        awlMessage.data[i] = canMessage.data[i];
    }

    return awlMessage;
}

void KvaserCanProtocolStrategy::closeConnection() {
    canBusOff(canCircuitHandle);
    canClose(canCircuitHandle);
}
