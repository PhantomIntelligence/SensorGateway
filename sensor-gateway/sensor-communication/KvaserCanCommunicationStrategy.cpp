/**
	Copyright 2014-2019 Phantom Intelligence Inc.

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
#include "KvaserCommunicationErrorFactory.h"

using SensorCommunication::KvaserCanCommunicationStrategy;

int32_t const CANLIB_KVASER_CAN_BIT_RATE = canBITRATE_1M;
uint32_t const CANLIB_TIME_SEGMENT_1 = 0;
uint32_t const CANLIB_TIME_SEGMENT_2 = 0;
uint32_t const CANLIB_SYNCHRONIZATION_JUMP_WIdTH = 0;
uint32_t const CANLIB_NUMBER_OF_SAMPLING_POINTS = 0;
uint32_t const CANLIB_SYNCMODE = 0;
int32_t const CANLIB_FLAGS_FOR_CHANNEL = canOPEN_EXCLUSIVE;
uint32_t const CANLIB_CAN_DRIVER_TYPE = canDRIVER_NORMAL;
int32_t const CANLIB_CHANNEL_ID = 0;

KvaserCanCommunicationStrategy::KvaserCanCommunicationStrategy(int32_t const& channelId) : communicationChannel() {
}

KvaserCanCommunicationStrategy::~KvaserCanCommunicationStrategy() {
    closeConnection(); // TODO: This will need to be removed. The destructor should be noexcept, this is not.
}

void KvaserCanCommunicationStrategy::openConnection() {
    canInitializeLibrary();
    canHandle communicationChannel = canOpenChannel(CANLIB_CHANNEL_ID, CANLIB_FLAGS_FOR_CHANNEL);
    auto returnCode = canSetBusParams(communicationChannel, CANLIB_KVASER_CAN_BIT_RATE, CANLIB_TIME_SEGMENT_1,
                                      CANLIB_TIME_SEGMENT_2,
                                      CANLIB_SYNCHRONIZATION_JUMP_WIdTH, CANLIB_NUMBER_OF_SAMPLING_POINTS,
                                      CANLIB_SYNCMODE);
    throwErrorIfNecessary(returnCode, "canSetBusParams");
    returnCode = canSetBusOutputControl(communicationChannel, CANLIB_CAN_DRIVER_TYPE);
    throwErrorIfNecessary(returnCode, "canSetBusOutputControl");
    returnCode = canBusOn(communicationChannel);
    throwErrorIfNecessary(returnCode, "canBusOn");
}

void KvaserCanCommunicationStrategy::closeConnection() {
    auto returnCode = canBusOff(communicationChannel);
    throwErrorIfNecessary(returnCode, "canBusOff");
    returnCode = canClose(communicationChannel);
    throwErrorIfNecessary(returnCode, "canClose");
}

KvaserCanCommunicationStrategy::super::Messages KvaserCanCommunicationStrategy::fetchMessages() {
    CanMessage canMessage{};
    auto returnCode = canReadWait(communicationChannel, &canMessage.id, &canMessage.data, &canMessage.length,
                                  &canMessage.flags,
                                  &canMessage.timestamp, CANLIB_READ_WAIT_INFINITE_DELAY);

    throwErrorIfNecessary(returnCode, "canReadWait");
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

void KvaserCanCommunicationStrategy::sendRequest(Request&& request) {
    CanMessage canMessage;
}

void KvaserCanCommunicationStrategy::writeMessage(CanMessage const& message) {
    auto returnCode = canWriteWait(communicationChannel,
                                   message.id,
                                   (void*) message.data,
                                   message.length,
                                   0, // Why? --> Need to confirm with JY
//                                   &message.flags,
                                   CANLIB_READ_WAIT_INFINITE_DELAY);
}

void KvaserCanCommunicationStrategy::throwErrorIfNecessary(canStatus const& errorCode, std::string const& callOrigin) {
    if (CANSTATUS_FAILURE(errorCode)) {
        ErrorHandling::throwKvaserCommunicationError(errorCode, callOrigin);
    }
}
