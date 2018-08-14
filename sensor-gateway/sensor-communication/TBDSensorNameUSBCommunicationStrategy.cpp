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
}

AWLMessage TBDSensorNameUSBCommunicationStrategy::readMessage() {
    return AWLMessage::returnDefaultData();
}

void TBDSensorNameUSBCommunicationStrategy::closeConnection() {

}

void TBDSensorNameUSBCommunicationStrategy::throwDeviceNotFoundErrorIfNeeded() {
    if (!usbDeviceHandle) {
        throwRuntimeError("TBDSensorName device not found!");
    }
}

void TBDSensorNameUSBCommunicationStrategy::throwUsbClaimInterfaceErrorIfNeeded(int errorCode) {
    if (errorCode < 0) {
        throwRuntimeError("");
    }
}
