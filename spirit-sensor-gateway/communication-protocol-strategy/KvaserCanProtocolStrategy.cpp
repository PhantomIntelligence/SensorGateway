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
    canReadWait(canCircuitHandle, &canMessage.id, &canMessage.data, &canMessage.length, &canMessage.flags, &canMessage.timestamp, 1000);
    return convertCanMessageToAwlMessage(canMessage);
}

AWLMessage KvaserCanProtocolStrategy::convertCanMessageToAwlMessage(CanMessage canMessage) {
    AWLMessage awlMessage{};

    awlMessage.messageID = (uint8_t)canMessage.id;
    awlMessage.messageTimestamp = (uint64_t)canMessage.timestamp;
    awlMessage.messageLength = (uint8_t)(canMessage.length);

    for (int dataIndex = 0; dataIndex < MESSAGE_DATA_LENGTH_IN_BYTES; ++dataIndex) {
        awlMessage.messageData[dataIndex] = (uint8_t)canMessage.data[dataIndex];
    }

    return awlMessage;
}

void KvaserCanProtocolStrategy::closeConnection() {
    canBusOff(canCircuitHandle);
    canClose(canCircuitHandle);
}
