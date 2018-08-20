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

#include "KvaserCanCommunicationStrategy.h"

using SensorCommunication::KvaserCanCommunicationStrategy;

const long CANLIB_KVASER_CAN_BIT_RATE = canBITRATE_1M;
const unsigned int CANLIB_TIME_SEGMENT_1 = 0;
const unsigned int CANLIB_TIME_SEGMENT_2 = 0;
const unsigned int CANLIB_SYNCHRONIZATION_JUMP_WIDTH = 0;
const unsigned int CANLIB_NUMBER_OF_SAMPLING_POINTS = 0;
const unsigned int CANLIB_SYNCMODE = 0;
const int CANLIB_FLAGS_FOR_CHANNEL = canOPEN_EXCLUSIVE;
const unsigned int CANLIB_CAN_DRIVER_TYPE = canDRIVER_NORMAL;
const int CANLIB_CHANNEL_ID = 0;

using DataFlow::AWLMessage;

KvaserCanCommunicationStrategy::KvaserCanCommunicationStrategy() : communicationChannel() {
}

KvaserCanCommunicationStrategy::~KvaserCanCommunicationStrategy() {
    closeConnection(); // TODO: This will need to be removed. The destructor should be noexcept, this is not.
}

void KvaserCanCommunicationStrategy::openConnection() {
    canInitializeLibrary();
    canHandle communicationChannel = canOpenChannel(CANLIB_CHANNEL_ID, CANLIB_FLAGS_FOR_CHANNEL);
    canSetBusParams(communicationChannel, CANLIB_KVASER_CAN_BIT_RATE, CANLIB_TIME_SEGMENT_1, CANLIB_TIME_SEGMENT_2,
                    CANLIB_SYNCHRONIZATION_JUMP_WIDTH, CANLIB_NUMBER_OF_SAMPLING_POINTS, CANLIB_SYNCMODE);
    canSetBusOutputControl(communicationChannel, CANLIB_CAN_DRIVER_TYPE);
    canBusOn(communicationChannel);
}

AWLMessage KvaserCanCommunicationStrategy::readMessage() {
    CanMessage canMessage{};
    canReadWait(communicationChannel, &canMessage.id, &canMessage.data, &canMessage.length, &canMessage.flags,
                &canMessage.timestamp, CANLIB_READ_WAIT_INFINITE_DELAY);
    return convertCanMessageToAwlMessage(canMessage);
}

AWLMessage KvaserCanCommunicationStrategy::convertCanMessageToAwlMessage(CanMessage canMessage) {
    AWL::DataArray data;
    for (auto dataNumber = 0; dataNumber < canMessage.length; ++dataNumber) {
        data[dataNumber] = canMessage.data[dataNumber];
    }

    AWLMessage awlMessage(canMessage.id, canMessage.timestamp, canMessage.length, data);

    return awlMessage;
}

void KvaserCanCommunicationStrategy::closeConnection() {
    canBusOff(communicationChannel);
    canClose(communicationChannel);
}
