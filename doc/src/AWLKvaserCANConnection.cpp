/**
	Copyright 2014-2016 Phantom Intelligence Inc.

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


//#include "SensorCommunication/AWLKvaserCANConnection.h"
#include "SpectreProcessor/include/SensorCommunication/AWLKvaserCANConnection.h"

using SensorCommunication::AWL::CAN::NUMBER_OF_DATA_IN_MESSAGE;
using SensorCommunication::AWLKvaserCANConnection;
//using SensorCommunication::AWLKvaserCANConnection::KvaserCanStatus;

AWLKvaserCANConnection::AWLKvaserCANConnection() :
        INVALID_CONNECTION_HANDLE(-1),
        DELAY_BEFORE_RECONNECTION_ATTEMPT(Millisecond(2000)),
        connected(false),
        kvaserConnectionHandle(INVALID_CONNECTION_HANDLE),
        kvaserCanChannelId(INVALID_CONNECTION_HANDLE),
        kvaserCanBitRate(canBITRATE_1M),
        timePointToReachBeforeReconnectionAttempt(highResolutionClock.now()) {
}

AWLKvaserCANConnection::~AWLKvaserCANConnection() {
    closeConnection();
}

void AWLKvaserCANConnection::openConnection() {
    closeConnection(); // In case it is already open
    updateReconnectionTimePointToNowPlusDelay();
    canInitializeLibrary(); // Initialize the Kvaser CAN driver. It must be called before any other Kvaser function.

    // TODO : figure out the purpose of canChannelCount
    int canChannelCount = -1;
    KvaserCanStatus kvaserCanStatus = canGetNumberOfChannels(&canChannelCount);
//    throwingExitIfTheActionFailed("canGetNumberOfChannels", (KvaserCanStatus) kvaserConnectionHandle);
    // TODO: validate correct behavior;
    // The line above is Legacy behavior
    // The line below is SWAG-ed as corrected by JB ---> Requires testing
    throwingExitIfTheActionFailed("canGetNumberOfChannels", kvaserCanStatus);

    kvaserConnectionHandle = canOpenChannel(kvaserCanChannelId, canOPEN_EXCLUSIVE);
    throwingExitIfTheActionFailed("canOpenChannel", (KvaserCanStatus) kvaserConnectionHandle);

    // Specify the baud rate
    // The bit layout is discussed in depth in most CAN controller data sheets
    // see http://www.kvaser.se
    kvaserCanStatus = canSetBusParams(kvaserConnectionHandle, kvaserCanBitRate, 0, 0, 0, 0, 0);
    throwingExitIfTheActionFailed("canSetBusParams", kvaserCanStatus);

    kvaserCanStatus = canBusOn(kvaserConnectionHandle);
    throwingExitIfTheActionFailed("canBusOn", kvaserCanStatus);

    // TODO: integrate bFrameInvalidated = false;
}

void AWLKvaserCANConnection::closeConnection() {
    LockGuard guard(connectionMutex);
    if (isConnected() && isConnectionValid()) {
        KvaserCanStatus status;

        status = canBusOff(kvaserConnectionHandle);
        logActionIfAnErrorOccurred("canBusOff", status);

        status = canClose(kvaserConnectionHandle);
        logActionIfAnErrorOccurred("canClose", status);

        kvaserConnectionHandle = INVALID_CONNECTION_HANDLE;

        // This will force re-enumeration of the CAN devices on the next canInitializeLibrary()
        canUnloadLibrary();
        updateReconnectionTimePointToNowPlusDelay();
    }

}

void AWLKvaserCANConnection::transmitMessage(Message const& message) {
    if (!isConnectionValid()) {
        throwingExitIfTheActionFailed("Error in transmitting the message, connection is invalid.",
                                      static_cast<KvaserCanStatus>(kvaserConnectionHandle));
    }
    KvaserCanStatus status;
    status = canWrite(kvaserConnectionHandle, message.id, (void*) &message.data, NUMBER_OF_DATA_IN_MESSAGE, 0);
    logActionIfAnErrorOccurred("canWrite", status);
}

auto AWLKvaserCANConnection::receiveMessage() -> Message {
    if (!isConnectionValid()) {
        attemptReconnection();
        throwIllegalActionException("Error in transmitting the message, connection is invalid.");

    }
    long id;
    unsigned long timestamp;
    unsigned int canFlags;
    unsigned int length;
    uint8_t data[NUMBER_OF_DATA_IN_MESSAGE];
    KvaserCanStatus status = canRead(kvaserConnectionHandle, &id, data, &length, &canFlags, &timestamp);

    attemptReconnectionAndThrowIfAnErrorOccurred(status);
    Message receivedMessage{};
    receivedMessage.id = static_cast<uData64>(id);
    receivedMessage.timestamp = timestamp;
    receivedMessage.length = static_cast<uData8>(length);
    for (auto i = 0; i < NUMBER_OF_DATA_IN_MESSAGE; ++i) {
        receivedMessage.data[i] = data[i];
    }

    // TODO: Make sure the result is then --> ReceiverCANCapture::ParseMessage(receivedMessage);
    return receivedMessage;
}

void AWLKvaserCANConnection::attemptReconnection() {
    if (highResolutionClock.now() > timePointToReachBeforeReconnectionAttempt) {
        //TODO: Complete logging
//            log("Attempting reconnection AWL via KvaserCAN connection");
        try {
            openConnection();
            //TODO: undestand what those do and implement them better
//                WriteCurrentDateTime();
//                SetMessageFilters();
        } catch (std::runtime_error runtimeError) {
            //TODO: Complete logging
//                log("Reconnection Attempt failed.");
        }
    }
}

void AWLKvaserCANConnection::attemptReconnectionAndThrowIfAnErrorOccurred(const KvaserCanStatus& status) {
    if (anErrorOccurred(status)) {
        //Equivalent to old code, TODO: make sure this and the new (see below) have the same behavior.
//        //TODO: verify if these two cases aren't actually the same;
//        if (status == canERR_NOMSG) {
//            logActionIfAnErrorOccurred("canRead", status);
//        } else {
//            //TODO: Complete logging and make sure this case isn't redundant
////            log("Error on canRead");
//        }
//        attemptReconnection();
//        updateReconnectionTimePointToNowPlusDelay();
//        throwIllegalActionException("Error on canRead"); //Note: this should be uncommented if old code kept

        //New code
        attemptReconnection();
        throwingExitIfTheActionFailed("canRead", status);
    }
}

Boolean AWLKvaserCANConnection::isConnected() const noexcept {
    return connected.load();
}

Boolean AWLKvaserCANConnection::isConnectionValid() const noexcept {
    return kvaserConnectionHandle != INVALID_CONNECTION_HANDLE;
}

void AWLKvaserCANConnection::logActionIfAnErrorOccurred(string attemptedAction,
                                                            KvaserCanStatus kvaserCanStatus) const noexcept {
    if (anErrorOccurred(kvaserCanStatus)) {
        char errorTextBuffer[64];
        errorTextBuffer[0] = '\0';
        canGetErrorText(kvaserCanStatus, errorTextBuffer, sizeof(errorTextBuffer));
        //TODO: Complete logging
//        DebugFilePrintf(debugFile, "%s: failed. status=%d (%s)\n", id, (int) status, errorTextBuffer);
    }
}

void AWLKvaserCANConnection::throwingExitIfTheActionFailed(string attemptedAction,
                                                               KvaserCanStatus kvaserCanStatus) {
    if (anErrorOccurred(kvaserCanStatus)) {
        logActionIfAnErrorOccurred(attemptedAction, kvaserCanStatus);
        // TODO: Create correct error message from the attempted action
        updateReconnectionTimePointToNowPlusDelay();
        throwIllegalActionException("Failed action.");
    }
}

Boolean AWLKvaserCANConnection::anErrorOccurred(KvaserCanStatus status) const noexcept {
    return status != canOK;
}

void AWLKvaserCANConnection::updateReconnectionTimePointToNowPlusDelay() noexcept {
    timePointToReachBeforeReconnectionAttempt = highResolutionClock.now() + DELAY_BEFORE_RECONNECTION_ATTEMPT;
}


