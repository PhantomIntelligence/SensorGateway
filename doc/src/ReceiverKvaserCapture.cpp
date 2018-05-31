/* ReceiverKvaserCapture.cpp */
/*
	 Some parts Copyright 2014 - 2017 Phantom Intelligence Inc.

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

/*
 * Some portions:
**                         Copyright 1996-98 by KVASER AB
**                   P.O Box 4076 SE-51104 KINNAHULT, SWEDEN
**             E-mail: staff@kvaser.se   WWW: http://www.kvaser.se
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/


#ifndef Q_MOC_RUN

#include <boost/property_tree/ptree.hpp>

#endif


#include <SensorCommunication/ReceiverKvaserCapture.h>
#include <canlib.h>  // Kvaser CAN library


const NumberOfMillisecond DELAY_BEFORE_REOPEN_PORT_ATTEMPT(2000);
// To see if the system reconnects

using awl::ReceiverKvaserCapture;

ReceiverKvaserCapture::ReceiverKvaserCapture(int receiverID, int inReceiverChannelQty, const int inKvaserChannel,
                                             const ReceiverCANCapture::eReceiverCANRate inCANBitRate, int inFrameRate,
                                             ChannelMask& inChannelMask, MessageMask& inMessageMask,
                                             float inRangeOffset, const RegisterSet& inRegistersFPGA,
                                             const RegisterSet& inRegistersADC, const RegisterSet& inRegistersGPIO,
                                             const AlgorithmSet& inParametersAlgos)
        : ReceiverCANCapture(receiverID, inReceiverChannelQty, inCANBitRate, inFrameRate, inChannelMask, inMessageMask,
                             inRangeOffset, inRegistersFPGA, inRegistersADC, inRegistersGPIO, inParametersAlgos),
          canChannelID(inKvaserChannel),
          kvaserHandle(-1),
          closeCANReentryCount(0) {
    ConvertKvaserCANBitRateCode();
}


ReceiverKvaserCapture::ReceiverKvaserCapture(int receiverID, boost::property_tree::ptree& propTree) :
        ReceiverCANCapture(receiverID, propTree),
        canChannelID(-1),
        kvaserHandle(-1),
        closeCANReentryCount(0) {
    // Read the configuration from the configuration file
    ReadConfigFromPropTree(propTree);
    ReadRegistersFromPropTree(propTree);
    ConvertKvaserCANBitRateCode();
}

ReceiverKvaserCapture::~ReceiverKvaserCapture() {
    CloseDebugFile(debugFile);
    EndDistanceLog();
    Stop(); // Stop the thread
}

bool ReceiverKvaserCapture::OpenCANPort() {
//    canStatus status = canOK;

    // What do we do if the port is already opened?
//    CloseCANPort();
//
//    updateReconnectTimeAfterDelay();
//
//    canInitializeLibrary();

    //
    // Open a handle to the CAN circuit.
    // Specifying canOPEN_EXCLUSIVE ensures we get a circuit that no one else is using.
    //
//    int canChannelCount = -1;
//    status = canGetNumberOfChannels(&canChannelCount);
//    if (status < 0) {
//        logIfAnErrorOccurred("canOpenChannel", (canStatus) kvaserHandle);
//        updateReconnectTimeAfterDelay();
//        return (false);
//    }
//
//    kvaserHandle = canOpenChannel(canChannelID, canOPEN_EXCLUSIVE);
//    if (kvaserHandle < 0) {
//        logIfAnErrorOccurred("canOpenChannel", (canStatus) kvaserHandle);
//        updateReconnectTimeAfterDelay();
//        return (false);
//    }

    // Specify the baud rate
    // The bit layout is discussed in depth in most CAN controller data sheets
    // see http://www.kvaser.se
//    status = canSetBusParams(kvaserHandle, canBitRateCode, 0, 0, 0, 0, 0);
//    if (status < 0) {
//        logIfAnErrorOccurred("canSetBusParam", status);
//        updateReconnectTimeAfterDelay();
//        return (false);
//    }
//
    //
    // Then we start the ball rolling.
    //
//    status = canBusOn(kvaserHandle);
//    if (status < 0) {
//        logIfAnErrorOccurred("canBusOn", status);
//        updateReconnectTimeAfterDelay();
//        return (false);
//    }
//
//    bFrameInvalidated = false;
//    return (true);
}

void ReceiverKvaserCapture::updateReconnectTimeAfterDelay() {
//    reconnectTime = highResolutionClock.now() + DELAY_BEFORE_REOPEN_PORT_ATTEMPT;
}

bool ReceiverKvaserCapture::CloseCANPort() {

//    if (closeCANReentryCount > 0) {
//        return (false);
//    }
//
//    closeCANReentryCount++;
//
//    canStatus status;
//    if (kvaserHandle >= 0) {
//        status = canBusOff(kvaserHandle);
//        logIfAnErrorOccurred("canBusOff", status);
//
//        status = canClose(kvaserHandle);
//        logIfAnErrorOccurred("canClose", status);
//    }
//
//    kvaserHandle = -1;
//
//    This will force re-enumeration of the CAN devices on the next canInitializeLibrary()
//    canUnloadLibrary();
//
//    updateReconnectTimeAfterDelay();
//    closeCANReentryCount--;
//    return (true);
}

void ReceiverKvaserCapture::logIfAnErrorOccurred(char* id, canStatus status) {
//    if (status != canOK) {
//        char errorTextBuffer[64];
//        errorTextBuffer[0] = '\0';
//        canGetErrorText(status, errorTextBuffer, sizeof(errorTextBuffer));
//        DebugFilePrintf(debugFile, "%s: failed. status=%d (%s)\n", id, (int) status, errorTextBuffer);
//    }
}

using SensorCommunication::AWL::CAN::NUMBER_OF_DATA_IN_MESSAGE;

void ReceiverKvaserCapture::DoOneThreadIteration() {

//    if (!WasStopped()) {
//    if (kvaserHandle >= 0) {
//        long id;
//        unsigned long timestamp;
//        unsigned int canFlags;
//        unsigned int length;
//        uint8_t data[NUMBER_OF_DATA_IN_MESSAGE];
//        canStatus status = canRead(kvaserHandle, &id, data, &length, &canFlags, &timestamp);
//
//        if (status == canOK) {
//            AWLCANMessage receivedMessage{};
//            receivedMessage.id = static_cast<uData64>(id);
//            receivedMessage.timestamp = timestamp;
//            receivedMessage.length = static_cast<uData8>(length);
//            for (auto i = 0; i < NUMBER_OF_DATA_IN_MESSAGE; ++i) {
//                receivedMessage.data[i] = data[i];
//            }
//
//            ParseMessage(receivedMessage);
//        } else if (status == canERR_NOMSG) {
//            logIfAnErrorOccurred("canRead", status);
//            attemptReconnection();
//            updateReconnectTimeAfterDelay();

//        } else {
//            // canRead returned an error.
//            // This means we have an error on the port.
//            // Let the port reconnect by itself after time-out
//            DebugFilePrintf(debugFile, "Error on canRead.  Resetting CAN Port");
//            CloseCANPort();
//            updateReconnectTimeAfterDelay();
//        }
//    } else if (kvaserHandle <= 0) {
    // Port is not opened.  Try to repoen after a certain delay.
//        attemptReconnection();
//    }

//    }
}

void ReceiverKvaserCapture::attemptReconnection() {
//    if (highResolutionClock.now() > reconnectTime) {
//        DebugFilePrintf(debugFile, "Reconnecting CAN Port");
//        if (OpenCANPort()) {
    WriteCurrentDateTime();
    SetMessageFilters();
//        }
//    }
}

bool ReceiverKvaserCapture::WriteMessage(const AWLCANMessage& inMsg) {

//    if (kvaserHandle < 0) {
//        return (false);
//    }
//    canStatus status = canWrite(kvaserHandle, inMsg.id, (void*) &inMsg.data, 8, 0);
//    logIfAnErrorOccurred("canWrite", status);
//
//    return (true);
}


bool ReceiverKvaserCapture::ReadConfigFromPropTree(boost::property_tree::ptree& propTree) {
    ReceiverCANCapture::ReadConfigFromPropTree(propTree);

    char receiverKeyString[32];
    std::sprintf(receiverKeyString, "config.receivers.receiver%d", receiverID);
    std::string receiverKey = receiverKeyString;

    boost::property_tree::ptree& receiverNode = propTree.get_child(receiverKey);
    // Communication parameters
    canChannelID = receiverNode.get<int>("kvaserChannel");

    return (true);
}

void ReceiverKvaserCapture::ConvertKvaserCANBitRateCode() {
    switch (canRate) {
        case canRate1Mbps: {
            canBitRateCode = canBITRATE_1M;
        }
            break;
        case canRate500kbps: {
            canBitRateCode = canBITRATE_500K;
        }
            break;

        case canRate250kbps: {
            canBitRateCode = canBITRATE_250K;
        }
            break;

        case canRate125kbps: {
            canBitRateCode = canBITRATE_125K;
        }
            break;
        case canRate100kbps: {
            canBitRateCode = canBITRATE_100K;
        }
            break;
        case canRate50kbps: {
            canBitRateCode = canBITRATE_50K;
        }
            break;

        case canRate10kps: {
            canBitRateCode = canBITRATE_10K;
        }
            break;
        default: {
            canBitRateCode = canBITRATE_1M;
        }
            break;
    }
}
