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

using CommunicationProtocolStrategy::KvaserCanProtocolStrategy;


KvaserCanProtocolStrategy::KvaserCanProtocolStrategy(): canCircuitHandle(initializeCanConnection()){

}

KvaserCanProtocolStrategy:: ~KvaserCanProtocolStrategy(){
    canBusOff(canCircuitHandle);
    canClose(canCircuitHandle);
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
    canRead(canCircuitHandle, &canMessage.id, canMessage.data, &canMessage.length, &canMessage.flags, &canMessage.timestamp);
    return convertCanMessageToAwlMessage(canMessage);
}

AWLMessage KvaserCanProtocolStrategy::convertCanMessageToAwlMessage(CanMessage canMessage) {
    AWLMessage awlMessage{};

    awlMessage.messageID = static_cast<uint64_t>(canMessage.id);
    awlMessage.messageLength = static_cast<uint64_t>(canMessage.length);
    awlMessage.messageFlags = static_cast<uint64_t>(canMessage.flags);
    awlMessage.messageTimestamp = static_cast<uint64_t>(canMessage.timestamp);

    for (int dataIndex = 0; dataIndex < canMessage.length; ++dataIndex) {
        awlMessage.messageData[dataIndex] = canMessage.data[dataIndex];
    }

    return awlMessage;
}
