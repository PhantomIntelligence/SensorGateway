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

#include "TBDSensorNameUSBCommunicationStrategy.h"

using SensorCommunication::TBDSensorNameUSBCommunicationStrategy;
using DataFlow::AWLMessage;

TBDSensorNameUSBCommunicationStrategy::TBDSensorNameUSBCommunicationStrategy() :
        usbConnectionParameters({0x064b, 7823,
                                 (129),
                                 (1),
                                 3000}
        ),
        usbContext(nullptr),
        usbDeviceHandle(nullptr),
        reconnectTime(SteadyClock::now()),
        dataCanBeFetched(false) {

}

TBDSensorNameUSBCommunicationStrategy::~TBDSensorNameUSBCommunicationStrategy() noexcept = default;

void TBDSensorNameUSBCommunicationStrategy::openConnection() {

    libusb_init(&usbContext);
    libusb_set_option(usbContext,
                      libusb_option::LIBUSB_OPTION_LOG_LEVEL,
                      LIBUSB_LOG_LEVEL_INFO);

    usbDeviceHandle = libusb_open_device_with_vid_pid(usbContext,
                                                      usbConnectionParameters.vendorId,
                                                      usbConnectionParameters.productId);
    throwDeviceNotFoundErrorIfNeeded();

    auto errorCode = libusb_claim_interface(usbDeviceHandle, INTERFACE_TO_CLAIM_FROM_DEVICE);
    throwUsbClaimInterfaceErrorIfNeeded(errorCode);


    USBCommandBlock usbCommandBlock{GET_FIRMWARE_VERSION, 0, 0};

    doUSBBulkTransferAndReturnNumberOfByteActuallyTransferred(
            usbConnectionParameters.endpointOut,
            (Byte*) &usbCommandBlock,
            sizeof(usbCommandBlock));

    Byte versionStrings[NUMBER_OF_VERSION_STRINGS][VERSION_STRING_MAX_LENGTH];
    doUSBBulkTransferAndReturnNumberOfByteActuallyTransferred(
            usbConnectionParameters.endpointIn,
            (Byte*) &versionStrings,
            sizeof(versionStrings));
}

TBDSensorNameUSBCommunicationStrategy::super::Messages TBDSensorNameUSBCommunicationStrategy::fetchMessages() {
    USBCommandBlock lidarQueryCommandBlock{LIDAR_QUERY, 0, 0};
    doUSBBulkTransferAndReturnNumberOfByteActuallyTransferred(
            usbConnectionParameters.endpointOut,
            (Byte*) &lidarQueryCommandBlock,
            sizeof(lidarQueryCommandBlock));
    auto numberOfByteReceived = doUSBBulkTransferAndReturnNumberOfByteActuallyTransferred(
            usbConnectionParameters.endpointIn,
            (Byte*) &quantityOfDataThatCanBeFetched,
            sizeof(quantityOfDataThatCanBeFetched));

    dataCanBeFetched.store(numberOfByteReceived == sizeof(quantityOfDataThatCanBeFetched));

    TBDSensorNameUSBCommunicationStrategy::super::Messages messages;
    if (dataCanBeFetched.load() && quantityOfDataThatCanBeFetched.numberOfMessageReady > 0) {
        messages = fetchSensorMessages(static_cast<uint8_t>(quantityOfDataThatCanBeFetched.numberOfMessageReady));
    }
    return messages;
}

TBDSensorNameUSBCommunicationStrategy::super::RawDataCycles
TBDSensorNameUSBCommunicationStrategy::fetchRawDataCycles() {
    super::RawDataCycles rawDataCycles;
    if (dataCanBeFetched.load() && quantityOfDataThatCanBeFetched.numberOfRawDataCyclesReady > 0) {
    }
    return rawDataCycles;
}

void TBDSensorNameUSBCommunicationStrategy::closeConnection() {
    LockGuard lockGuard(closingMutex);

    libusb_close(usbDeviceHandle);
    libusb_exit(usbContext);
    usbDeviceHandle = nullptr;
    reconnectTime = SteadyClock::now();
}

void TBDSensorNameUSBCommunicationStrategy::throwDeviceNotFoundErrorIfNeeded() {
    if (!usbDeviceHandle) {
        throwRuntimeError("TBDSensorName device not found!");
    }
}

void TBDSensorNameUSBCommunicationStrategy::throwUsbClaimInterfaceErrorIfNeeded(int errorCode) {
    if (errorCode < 0) {
        std::ostringstream errorMessage("usb_claim_interface returned an error: ", std::ios_base::ate);
        errorMessage << errorCode;
        throwRuntimeError(errorMessage.str().c_str());
    }
}

int TBDSensorNameUSBCommunicationStrategy::doUSBBulkTransferAndReturnNumberOfByteActuallyTransferred(
        Byte endpoint, Byte* data, int length) {
    throwDeviceNotFoundErrorIfNeeded();
    unsigned int timeout = 0;
    int numberOfByteActuallyTransferred = 0;
    auto errorCode = libusb_bulk_transfer(usbDeviceHandle, endpoint, data, length,
                                          &numberOfByteActuallyTransferred, timeout);
    throwErrorOnLibUSBBulkTransfetErrorCode(errorCode);

    if (numberOfByteActuallyTransferred != length) {
        // TODO: log when the logger has been created
        // TODO: throw an exception?
        std::cout << "An incorrect number of data has been transferred... \n"
                  << "Expected: " << length
                  << " actual: " << numberOfByteActuallyTransferred
                  << std::endl;
        numberOfByteActuallyTransferred = 0;
    }
    return numberOfByteActuallyTransferred;
}

void TBDSensorNameUSBCommunicationStrategy::throwErrorOnLibUSBBulkTransfetErrorCode(int errorCode) {
    if (errorCode == 0) {
        return;
    }

    std::ostringstream errorMessage("ERROR in libusb bulk write: ", std::ios_base::ate);
    errorMessage << errorCode;
    if (errorCode == LIBUSB_ERROR_TIMEOUT) {
        errorMessage << "... transfer timed out.\n";
    } else if (errorCode == LIBUSB_ERROR_PIPE) {
        errorMessage << "... the endpoint halted.\n";
    } else if (errorCode == LIBUSB_ERROR_OVERFLOW) {
        errorMessage << "... the device offered more data than expected.\n";
    } else if (errorCode == LIBUSB_ERROR_NO_DEVICE) {
        errorMessage << "... the device has disconnected.\n";
    } else {
        errorMessage << "... something went wrong.\n";
    }
    throwRuntimeError(errorMessage.str().c_str());
}

TBDSensorNameUSBCommunicationStrategy::super::Messages
TBDSensorNameUSBCommunicationStrategy::fetchSensorMessages(uint8_t numberOfMessagesToFetch) {
    super::Messages fetchedMessages;
    auto const MAX_FETCHABLE_SIZE = fetchedMessages.size();

    if (numberOfMessagesToFetch > MAX_FETCHABLE_SIZE) {
        numberOfMessagesToFetch = MAX_FETCHABLE_SIZE;
        // TODO: log warning?
    }

    USBSensorMessage sensorMessageRequest;
    sensorMessageRequest.id = QUERY_NUMBER_OF_MESSAGES_AND_RAW_DATA_CYCLES_READY_TO_BE_FETCH;
    sensorMessageRequest.data[0] = (Byte) numberOfMessagesToFetch;
    doUSBBulkTransferAndReturnNumberOfByteActuallyTransferred(
            usbConnectionParameters.endpointOut,
            (Byte*) &sensorMessageRequest,
            sizeof(USBSensorMessage));

    USBSensorMessage sensorMessageResponse[MAX_FETCHABLE_SIZE];
    doUSBBulkTransferAndReturnNumberOfByteActuallyTransferred(
            usbConnectionParameters.endpointIn,
            (Byte*) &sensorMessageResponse,
            numberOfMessagesToFetch * sizeof(USBSensorMessage));



    for (uint32_t messageIndex = 0; messageIndex < numberOfMessagesToFetch; ++messageIndex) {
        auto sensorMessage = sensorMessageResponse[messageIndex];
        if (sensorMessage.id != 0) {
            auto message = convertUSBSensorMessageToSensorMessage(&sensorMessage);
            fetchedMessages.at(messageIndex) = message;
        }
    }

    return fetchedMessages;
}

TBDSensorNameUSBCommunicationStrategy::super::Message
TBDSensorNameUSBCommunicationStrategy::convertUSBSensorMessageToSensorMessage(USBSensorMessage* sensorMessage) {
    AWL::DataArray data;
    for (auto dataNumber = 0; dataNumber < sensorMessage->length; ++dataNumber) {
        data[dataNumber] = sensorMessage->data[dataNumber];
    }
    TBDSensorNameUSBCommunicationStrategy::super::Message
            message(sensorMessage->id, sensorMessage->timestamp, sensorMessage->length, data);
    return message;
}
