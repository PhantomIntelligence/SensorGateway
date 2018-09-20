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
    canSetBusParams(communicationChannel, CANLIB_KVASER_CAN_BIT_RATE, CANLIB_TIME_SEGMENT_1, CANLIB_TIME_SEGMENT_2,
                    CANLIB_SYNCHRONIZATION_JUMP_WIDTH, CANLIB_NUMBER_OF_SAMPLING_POINTS, CANLIB_SYNCMODE);
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

void KvaserCanCommunicationStrategy::throwErrorOnLibKvaserCanErrorCode(int errorCode) {
    if (errorCode == 0) {
        return;
    }

    std::ostringstream errorMessage("Generic Error KvaserCan Error : ", std::ios_base::ate);
    errorMessage << errorCode;
    errorMessage << "\n";
    if (errorCode == canERR_PARAM) {
        errorMessage << SensorCommunicationExceptions::ERROR_PARAMETER;
    } else if (errorCode == canERR_NOMSG || errorCode == canERR_TIMEOUT) {
        errorMessage << SensorCommunicationExceptions::ERROR_FUNCTION;
    } else if (errorCode == canERR_NOTFOUND || errorCode == canERR_NOCHANNELS) {
        errorMessage << SensorCommunicationExceptions::ERROR_CHANNEL;
    } else if (errorCode == canERR_NOMEM || errorCode == canERR_MEMO_FAIL) {
        errorMessage << SensorCommunicationExceptions::ERROR_MEMORY;
    } else if (errorCode == canERR_INTERRUPTED) {
        errorMessage << SensorCommunicationExceptions::ERROR_CONNECTION;
    } else if (errorCode == canERR_NOTINITIALIZED || errorCode == canERR_DYNALIB) {
        errorMessage << SensorCommunicationExceptions::ERROR_LIBRARY;
    } else if (errorCode == canERR_NOHANDLES || errorCode == canERR_INVHANDLE) {
        errorMessage << SensorCommunicationExceptions::ERROR_NOHANDLE;
    } else if (errorCode == canERR_DYNALOAD || errorCode == canERR_DRIVER || errorCode == canERR_DRIVERLOAD ||
               errorCode == canERR_DRIVERFAILED || errorCode == canERR_INTERNAL) {
        errorMessage << SensorCommunicationExceptions::ERROR_DRIVER;
    } else if (errorCode == canERR_RESERVED_1 || errorCode == canERR_RESERVED_2 || errorCode == canERR_RESERVED_5 ||
               errorCode == canERR_RESERVED_6 || errorCode == canERR_RESERVED_7 || errorCode == canERR__RESERVED) {
        errorMessage << SensorCommunicationExceptions::ERROR_RESERVED;
    } else if (errorCode == canERR_HARDWARE || errorCode == canERR_NOCARD || errorCode == canERR_DISK) {
        errorMessage << SensorCommunicationExceptions::ERROR_HARDWARE;
    } else if (errorCode == canERR_DYNAINIT) {
        errorMessage << SensorCommunicationExceptions::ERROR_INITIALIZATION;
    } else if (errorCode == canERR_NOT_SUPPORTED) {
        errorMessage << SensorCommunicationExceptions::ERROR_NOT_SUPPORTED;
    } else if (errorCode == canERR_NOCONFIGMGR || errorCode == canERR_CONFIG) {
        errorMessage << SensorCommunicationExceptions::ERROR_CONFIG;
    } else if (errorCode == canERR_REGISTRY) {
        errorMessage << SensorCommunicationExceptions::ERROR_DATA;
    } else if (errorCode == canERR_LICENSE) {
        errorMessage << SensorCommunicationExceptions::ERROR_LICENSE;
    } else if (errorCode == canERR_NO_ACCESS) {
        errorMessage << SensorCommunicationExceptions::ERROR_NO_ACCESS;
    } else if (errorCode == canERR_NOT_IMPLEMENTED) {
        errorMessage << SensorCommunicationExceptions::ERROR_NOT_IMPLEMENTED;
    } else if (errorCode == canERR_DEVICE_FILE || errorCode == canERR_HOST_FILE || errorCode == canERR_INIFILE) {
        errorMessage << SensorCommunicationExceptions::ERROR_FILE;
    } else if (errorCode == canERR_CRC) {
        errorMessage << SensorCommunicationExceptions::ERROR_CRC;
    } else if (errorCode == canERR_SCRIPT_FAIL || errorCode == canERR_SCRIPT_WRONG_VERSION ||
               errorCode == canERR_SCRIPT_TXE_CONTAINER_VERSION || errorCode == canERR_SCRIPT_TXE_CONTAINER_FORMAT) {
        errorMessage << SensorCommunicationExceptions::ERROR_SCRIPT;
    } else if (errorCode == canERR_BUFFER_TOO_SMALL || errorCode == canERR_TXBUFOFL) {
        errorMessage << SensorCommunicationExceptions::ERROR_BUFFER;
    } else {
        errorMessage << "something went wrong, unknown error.\n";
    }
    throwRuntimeError(errorMessage.str().c_str());
}








