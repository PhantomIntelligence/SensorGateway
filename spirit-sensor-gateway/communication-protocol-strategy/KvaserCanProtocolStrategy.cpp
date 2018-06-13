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


KvaserCanProtocolStrategy::KvaserCanProtocolStrategy(): canCircuitHandle(initializeCanConnection()){

}

KvaserCanProtocolStrategy:: ~KvaserCanProtocolStrategy(){

}

canHandle KvaserCanProtocolStrategy::initializeCanConnection() {
    canInitializeLibrary();
    canHandle canCircuitHandle = canOpenChannel(0, canOPEN_EXCLUSIVE);
    canSetBusParams(canCircuitHandle, canBITRATE_1M, 0, 0, 0, 0, 0);
    canSetBusOutputControl(canCircuitHandle, canDRIVER_SILENT);
    canBusOn(canCircuitHandle);
    return canCircuitHandle;
}

AWLMessage KvaserCanProtocolStrategy::unwrapMessage(){
CanMessage canMessage{};
    long id;
    unsigned long timestamp;
    unsigned int flags;
    unsigned int length;
    uint8_t data[MESSAGE_DATA_LENGTH];
    canRead(canCircuitHandle, &id, data, &length, &flags, &timestamp);

    canMessage.id =id;
    canMessage.length=length;
    canMessage.flags=flags;
    canMessage.timestamp=timestamp;
    for (auto i = 0; i < MESSAGE_DATA_LENGTH; ++i) {
        canMessage.data[i] = data[i];
    }

    //canReadWait(canCircuitHandle,&canMessage.id,&canMessage.data,&canMessage.length,&canMessage.flags,&canMessage.timestamp,READ_WAIT_INFINITE); // permet de lire les messages indefinement mais pas de possibilite de close les bus sauf en cas  d erreur
    return convertCanMessageToAwlMessage(canMessage);
            // return convertCanMessageToAwlMessage(canMessage);
}

AWLMessage KvaserCanProtocolStrategy::convertCanMessageToAwlMessage(CanMessage canMessage) {

    AWLMessage awlMessage{};


    awlMessage.messageID = static_cast<uint64_t >(canMessage.id);
    awlMessage.messageTimestamp = canMessage.timestamp;
    awlMessage.messageLength = static_cast<uint8_t >(canMessage.length);
    awlMessage.messageFlags= static_cast<uint64_t >(canMessage.flags);
    for (auto i = 0; i < MESSAGE_DATA_LENGTH; ++i) {
        awlMessage.messageData[i] = canMessage.data[i];
    }

    return awlMessage;
}

void KvaserCanProtocolStrategy::closeConnection() {
    canBusOff(canCircuitHandle);
    canClose(canCircuitHandle);
}
