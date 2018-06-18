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


KvaserCanProtocolStrategy::KvaserCanProtocolStrategy(): communicationChannel(){
    initializeCanConnection();
}

KvaserCanProtocolStrategy:: ~KvaserCanProtocolStrategy()=default;

void KvaserCanProtocolStrategy::initializeCanConnection() {
    long kvaserCanBitRate =  canBITRATE_1M;
    unsigned int timeSegment1 = 0;
    unsigned int timeSegment2 = 0;
    unsigned int synchronizationJumpWidth =0;
    unsigned int numberOfSamplingPoints = 0;
    unsigned int syncmode = 0;
    int flagsForChannel = canOPEN_EXCLUSIVE;
    const unsigned int canDriverType = canDRIVER_NORMAL;
    canInitializeLibrary();
    canHandle communicationChannel = canOpenChannel(0, flagsForChannel);
    canSetBusParams(communicationChannel, kvaserCanBitRate, timeSegment1, timeSegment2, synchronizationJumpWidth, numberOfSamplingPoints, syncmode);
    canSetBusOutputControl(communicationChannel, canDriverType);
    canBusOn(communicationChannel);
}

AWLMessage KvaserCanProtocolStrategy::readMessages(){
    CanMessage canMessage{};
    canReadWait(communicationChannel, &canMessage.id, &canMessage.data, &canMessage.length, &canMessage.flags, &canMessage.timestamp, CANLIB_READ_WAIT_INFINITE_DELAY);
    return convertCanMessageToAwlMessage(canMessage);
}

AWLMessage KvaserCanProtocolStrategy::convertCanMessageToAwlMessage(CanMessage canMessage) {
    AWLMessage awlMessage{};
    awlMessage.id = canMessage.id;
    awlMessage.timestamp = canMessage.timestamp;
    awlMessage.length = canMessage.length;
    for (auto dataNumber = 0; dataNumber < canMessage.length; ++dataNumber) {
        awlMessage.data[dataNumber] = canMessage.data[dataNumber];
    }

    return awlMessage;
}

void KvaserCanProtocolStrategy::closeConnection() {
    canBusOff(communicationChannel);
    canClose(communicationChannel);
}
