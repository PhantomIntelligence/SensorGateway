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
        usbConnectionParameters({0x0483, 0xFFFF,
                                 (LIBUSB_ENDPOINT_IN | 1),
                                 (LIBUSB_ENDPOINT_OUT | 2),
                                 3000}
        ),
        usbContext(nullptr),
        usbDeviceHandle(nullptr),
        reconnectTime(SteadyClock::now()) {

}

TBDSensorNameUSBCommunicationStrategy::~TBDSensorNameUSBCommunicationStrategy() noexcept {

}

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


    USBCommandBlock usbCommandBlock;
    usbCommandBlock.command = GET_FIRMWARE_VERSION;
    usbCommandBlock.numberOfByteToTransfer = 0;
    usbCommandBlock.data = 0;
    unsigned int timeout = 0;

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

AWLMessage TBDSensorNameUSBCommunicationStrategy::readMessage() {
    return AWLMessage::returnDefaultData();
}

void TBDSensorNameUSBCommunicationStrategy::closeConnection() {
    LockGuard lockGuard(closingMutex);

    libusb_close(usbDeviceHandle);
    libusb_exit(nullptr); // TODO: ask Daniel if this shouldn't be:
//    libusb_exit(usbContext); // TODO: this?
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

