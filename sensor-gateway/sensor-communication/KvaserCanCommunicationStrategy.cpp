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
using ErrorHandling::KvaserCommunicationError;

int32_t const CANLIB_KVASER_CAN_BIT_RATE = canBITRATE_1M;
uint32_t const CANLIB_TIME_SEGMENT_1 = 0;
uint32_t const CANLIB_TIME_SEGMENT_2 = 0;
uint32_t const CANLIB_SYNCHRONIZATION_JUMP_WIDTH = 0;
uint32_t const CANLIB_NUMBER_OF_SAMPLING_POINTS = 0;
uint32_t const CANLIB_SYNCMODE = 0;
int32_t const CANLIB_FLAGS_FOR_CHANNEL = canOPEN_EXCLUSIVE;
uint32_t const CANLIB_CAN_DRIVER_TYPE = canDRIVER_NORMAL;
int32_t const CANLIB_CHANNEL_ID = 0;

KvaserCanCommunicationStrategy::KvaserCanCommunicationStrategy() : communicationChannel() {
}

KvaserCanCommunicationStrategy::~KvaserCanCommunicationStrategy() {
    closeConnection(); // TODO: This will need to be removed. The destructor should be noexcept, this is not.
}

void KvaserCanCommunicationStrategy::openConnection() {
    canInitializeLibrary();
    canHandle communicationChannel = canOpenChannel(CANLIB_CHANNEL_ID, CANLIB_FLAGS_FOR_CHANNEL);
    auto ret = canSetBusParams(communicationChannel, CANLIB_KVASER_CAN_BIT_RATE, CANLIB_TIME_SEGMENT_1,
                               CANLIB_TIME_SEGMENT_2,
                               CANLIB_SYNCHRONIZATION_JUMP_WIDTH, CANLIB_NUMBER_OF_SAMPLING_POINTS, CANLIB_SYNCMODE);

    if (ret != 0) {
        KvaserCommunicationError::throwKvaserCommunicationError(ret, "canSetBusParams");
    }
    canSetBusOutputControl(communicationChannel, CANLIB_CAN_DRIVER_TYPE);
    canBusOn(communicationChannel);
}

KvaserCanCommunicationStrategy::super::Messages KvaserCanCommunicationStrategy::fetchMessages() {
    CanMessage canMessage{};
    canReadWait(communicationChannel, &canMessage.id, &canMessage.data, &canMessage.length, &canMessage.flags,
                &canMessage.timestamp, CANLIB_READ_WAIT_INFINITE_DELAY);
    auto message = convertCanMessageToSensorMessage(canMessage);
    super::Messages messages = {message};
    return messages;
}

KvaserCanCommunicationStrategy::super::RawDataCycles KvaserCanCommunicationStrategy::fetchRawDataCycles() {
    super::RawDataCycles rawDataCycles;
    return rawDataCycles;
}

KvaserCanCommunicationStrategy::super::Message
KvaserCanCommunicationStrategy::convertCanMessageToSensorMessage(CanMessage canMessage) {
    AWL::MessageDataArray data;
    for (auto dataNumber = 0; dataNumber < canMessage.length; ++dataNumber) {
        data[dataNumber] = canMessage.data[dataNumber];
    }

    KvaserCanCommunicationStrategy::super::Message
            message(canMessage.id, canMessage.timestamp, canMessage.length, data);

    return message;
}

void KvaserCanCommunicationStrategy::closeConnection() {
    canBusOff(communicationChannel);
    canClose(communicationChannel);
}

