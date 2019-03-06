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

#include "GuardianUSBCommunicationStrategy.h"

using SensorCommunication::GuardianUSBCommunicationStrategy;
using DataFlow::AWLMessage;

GuardianUSBCommunicationStrategy::GuardianUSBCommunicationStrategy(USBConnectionParameters const& usbConnectionParameters) :
        usbConnectionParameters(usbConnectionParameters),
        usbContext(nullptr),
        usbDeviceHandle(nullptr),
        reconnectTime(SteadyClock::now()),
        dataCanBeFetched(false),
        numberOfMessagesToFetch(0),
        numberOfRawDataCyclesToFetch(0) {}

GuardianUSBCommunicationStrategy::~GuardianUSBCommunicationStrategy() noexcept = default;

void GuardianUSBCommunicationStrategy::openConnection() {

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

    setupCleanConnection();

}

void GuardianUSBCommunicationStrategy::closeConnection() {
    LockGuard lockGuard(closingMutex);

    libusb_close(usbDeviceHandle);
    libusb_exit(usbContext);
    usbDeviceHandle = nullptr;
    reconnectTime = SteadyClock::now();
}

GuardianUSBCommunicationStrategy::super::Messages GuardianUSBCommunicationStrategy::fetchMessages() {
    USBSensorMessage lidarQueryCommandBlock;
    lidarQueryCommandBlock.id = LIdAR_QUERY;

    doUSBBulkTransferAndReturnNumberOfByteActuallyTransferred(
            usbConnectionParameters.endpointOut,
            (Byte*) &lidarQueryCommandBlock,
            sizeof(lidarQueryCommandBlock));

    USBSensorMessage quantityOfDataThatCanBeFetched;
    auto numberOfByteReceived = doUSBBulkTransferAndReturnNumberOfByteActuallyTransferred(
            usbConnectionParameters.endpointIn,
            (Byte*) &quantityOfDataThatCanBeFetched,
            sizeof(quantityOfDataThatCanBeFetched));

    dataCanBeFetched.store(numberOfByteReceived == sizeof(quantityOfDataThatCanBeFetched));

    GuardianUSBCommunicationStrategy::super::Messages messages;
    if (dataCanBeFetched.load()) {

        numberOfRawDataCyclesToFetch = *((NumberOfDataToFetch*) &quantityOfDataThatCanBeFetched.data[0]);
        numberOfMessagesToFetch = *((NumberOfDataToFetch*) &quantityOfDataThatCanBeFetched.data[4]);

        if (numberOfMessagesToFetch > 0) {
            messages = fetchMessagesOnSensor(numberOfMessagesToFetch);
        }
    }
    return messages;
}

GuardianUSBCommunicationStrategy::super::RawDataCycles
GuardianUSBCommunicationStrategy::fetchRawDataCycles() {
    super::RawDataCycles rawDataCycles;
    if (dataCanBeFetched.load() && numberOfRawDataCyclesToFetch > 0) {
        rawDataCycles = fetchRawDataCyclesOnSensor(numberOfRawDataCyclesToFetch);
    }
    return rawDataCycles;
}

void GuardianUSBCommunicationStrategy::throwDeviceNotFoundErrorIfNeeded() {
    if (!usbDeviceHandle) {
        ErrorHandling::throwLibUSBDeviceNotFound();
    }
}

void GuardianUSBCommunicationStrategy::throwUsbClaimInterfaceErrorIfNeeded(int errorCode) {
    if (errorCode < 0) {
        std::ostringstream origin("usb_claim_interface returned an error: ", std::ios_base::ate);
        origin << errorCode;
        auto libUSBErrorCode = static_cast<libusb_error>(errorCode);
        ErrorHandling::throwGuardianUSBCommunicationError(libUSBErrorCode, origin.str());
    }
}

int GuardianUSBCommunicationStrategy::doUSBBulkTransferAndReturnNumberOfByteActuallyTransferred(
        Byte endpoint, Byte* data, int32_t length) {
    uint32_t timeout = 0;
    return doUSBBulkTransferAndReturnNumberOfByteActuallyTransferredWithTimeout(
            endpoint, data, length, timeout);
}

int GuardianUSBCommunicationStrategy::doUSBBulkTransferAndReturnNumberOfByteActuallyTransferredWithTimeout(
        Byte endpoint, Byte* data, int32_t length, uint32_t timeout) {
    throwDeviceNotFoundErrorIfNeeded();
    int numberOfByteActuallyTransferred = 0;
    auto errorCode = libusb_bulk_transfer(usbDeviceHandle, endpoint, data, length,
                                          &numberOfByteActuallyTransferred, timeout);
    throwErrorOnLibUSBBulkTransferErrorCode(errorCode);

    if (numberOfByteActuallyTransferred != length) {
        std::ostringstream errorMessage("An incorrect number of data has been transferred... \n", std::ios_base::ate);
        errorMessage << "Expected: " << length
                     << " actual: " << numberOfByteActuallyTransferred
                     << std::endl;

        auto libUSBErrorCode = static_cast<libusb_error>(errorCode);
        ErrorHandling::throwGuardianUSBCommunicationError(
                libUSBErrorCode,
                "LibUSB",
                errorMessage.str()
        );

        // TODO: log when the logger has been created

    }
    return numberOfByteActuallyTransferred;
}

void GuardianUSBCommunicationStrategy::setupCleanConnection() noexcept {
    bool hasNotThrownError = true;
    bool hasReceivedData = true;
    uint32_t timeout = 1;
    while (hasNotThrownError && hasReceivedData) {
        try {
            Byte trashDataBuffer[256];
            auto numberOfReturnedData = doUSBBulkTransferAndReturnNumberOfByteActuallyTransferredWithTimeout(
                    usbConnectionParameters.endpointIn,
                    trashDataBuffer,
                    sizeof(Byte[256]),
                    timeout);
            hasReceivedData = numberOfReturnedData != 0;
        } catch (...) {
            hasNotThrownError = false;
        }
    }
}

void GuardianUSBCommunicationStrategy::throwErrorOnLibUSBBulkTransferErrorCode(int errorCode) {
    if (errorCode == 0) {
        return;
    }

    auto libUSBErrorCode = static_cast<libusb_error>(errorCode);
    std::string origin = " in bulk write: ";
    ErrorHandling::throwGuardianUSBCommunicationError(libUSBErrorCode, origin);
}

GuardianUSBCommunicationStrategy::super::Messages
GuardianUSBCommunicationStrategy::fetchMessagesOnSensor(NumberOfDataToFetch numberOfMessagesToFetch) {
    super::Messages fetchedMessages;
    auto const MAX_FETCHABLE_SIZE = fetchedMessages.size();

    if (numberOfMessagesToFetch > MAX_FETCHABLE_SIZE) {
        numberOfMessagesToFetch = static_cast<uint8_t>(MAX_FETCHABLE_SIZE);
        // TODO: log warning?
    }

    USBSensorMessage sensorMessageRequest;
    sensorMessageRequest.id = POLL_MESSAGES;
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


    for (auto messageIndex = 0u; messageIndex < numberOfMessagesToFetch; ++messageIndex) {
        auto sensorMessage = sensorMessageResponse[messageIndex];
        if (sensorMessage.id != 0) {
            auto message = convertUSBSensorMessageToSensorMessage(&sensorMessage);
            fetchedMessages.at(messageIndex) = message;
        }
    }

    return fetchedMessages;
}

GuardianUSBCommunicationStrategy::super::Message
GuardianUSBCommunicationStrategy::convertUSBSensorMessageToSensorMessage(USBSensorMessage* sensorMessage) {
    AWL::MessageDataArray data;
    for (auto dataNumber = 0; dataNumber < sensorMessage->length; ++dataNumber) {
        data[dataNumber] = sensorMessage->data[dataNumber];
    }
    GuardianUSBCommunicationStrategy::super::Message
            message(sensorMessage->id, sensorMessage->timestamp, sensorMessage->length, data);
    return message;
}


GuardianUSBCommunicationStrategy::super::RawDataCycles
GuardianUSBCommunicationStrategy::fetchRawDataCyclesOnSensor(NumberOfDataToFetch numberOfRawDataCyclesToFetch) {
    super::RawDataCycles fetchedRawDataCycles;
    auto const MAX_FETCHABLE_SIZE = fetchedRawDataCycles.size();
    if (numberOfRawDataCyclesToFetch > MAX_FETCHABLE_SIZE) {
        numberOfRawDataCyclesToFetch = static_cast<uint8_t>(MAX_FETCHABLE_SIZE);
        // TODO: log warning?
    }

    USBSensorMessage sensorMessageRequest;
    sensorMessageRequest.id = FETCH_RAW_DATA_CYCLES;
    sensorMessageRequest.data[0] = (Byte) numberOfRawDataCyclesToFetch;
    doUSBBulkTransferAndReturnNumberOfByteActuallyTransferred(
            usbConnectionParameters.endpointOut,
            (Byte*) &sensorMessageRequest,
            sizeof(USBSensorMessage));

    super::RawData::Content rawDataCyclesContent[MAX_FETCHABLE_SIZE];
    doUSBBulkTransferAndReturnNumberOfByteActuallyTransferred(
            usbConnectionParameters.endpointIn,
            (Byte*) &rawDataCyclesContent,
            numberOfRawDataCyclesToFetch * sizeof(super::RawData::Content));


    for (auto rawDataIndex = 0u; rawDataIndex < numberOfRawDataCyclesToFetch; ++rawDataIndex) {
        auto RawDataDefinition = rawDataCyclesContent[rawDataIndex];
        fetchedRawDataCycles.at(rawDataIndex) = super::RawData(RawDataDefinition);
    }

    return fetchedRawDataCycles;
}

void GuardianUSBCommunicationStrategy::sendRequest(Request&& request) {
}

