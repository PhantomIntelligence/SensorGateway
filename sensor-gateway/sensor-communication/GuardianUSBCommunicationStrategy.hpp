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

#ifndef SENSORGATEWAY_GUARDIANUSBCOMMUNICATIONSTRATEGY_H
#define SENSORGATEWAY_GUARDIANUSBCOMMUNICATIONSTRATEGY_H


#include "sensor-gateway/common/data-structure/sensor/GuardianStructures.h"
#include "SensorCommunicationStrategy.hpp"
#include "LibUSBCommunicationErrorFactory.h"

namespace SensorCommunication {

    typedef struct {
        uint16_t vendorId;
        uint16_t productId;
        uint8_t endpointIn;
        uint8_t endpointOut;
        uint16_t timeout;
    } USBConnectionParameters;

    using Sensor::AWL::NUMBER_OF_DATA_BYTES;

    /**
     * @warning This class DOES NOT handle thread safety concerns.
     * It is meant to be called and used by a single thread from the SensorCommunicator.
     */
    template<typename GuardianStructures>
    class GuardianUSBCommunicationStrategy final : public SensorCommunicationStrategy<GuardianStructures> {
    protected:

        using SteadyClock = std::chrono::steady_clock;
        using ReconnectTime = std::chrono::time_point<SteadyClock>;

        using super = SensorCommunicationStrategy<GuardianStructures>;
        using Message = typename super::Message;
        using Messages = typename super::Messages;
        using RawData = typename super::RawData;
        using RawDataCycles = typename super::RawDataCycles;
        using Request = typename super::Message;

        const int INTERFACE_TO_CLAIM_FROM_DEVICE = 0;
        const int VERSION_STRING_MAX_LENGTH = 32;

    public:

        explicit GuardianUSBCommunicationStrategy(USBConnectionParameters const& usbConnectionParameters) :
                usbConnectionParameters(usbConnectionParameters),
                usbContext(nullptr),
                usbDeviceHandle(nullptr),
                reconnectTime(SteadyClock::now()),
                dataCanBeFetched(false),
                numberOfMessagesToFetch(0),
                numberOfRawDataCyclesToFetch(0) {}

        ~GuardianUSBCommunicationStrategy() noexcept override = default;

        GuardianUSBCommunicationStrategy(GuardianUSBCommunicationStrategy const& other) = delete;

        GuardianUSBCommunicationStrategy(GuardianUSBCommunicationStrategy&& other) noexcept = delete;

        GuardianUSBCommunicationStrategy& operator=(GuardianUSBCommunicationStrategy const& other)& = delete;

        GuardianUSBCommunicationStrategy&
        operator=(GuardianUSBCommunicationStrategy&& other)& noexcept = delete;

        void openConnection() final {

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

        void closeConnection() final {
            LockGuard lockGuard(closingMutex);

            libusb_close(usbDeviceHandle);
            libusb_exit(usbContext);
            usbDeviceHandle = nullptr;
            reconnectTime = SteadyClock::now();
        }

        Messages fetchMessages() final {
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

            Messages messages;
            if (dataCanBeFetched.load()) {

                numberOfRawDataCyclesToFetch = *((NumberOfDataToFetch * ) & quantityOfDataThatCanBeFetched.data[0]);
                numberOfMessagesToFetch = *((NumberOfDataToFetch * ) & quantityOfDataThatCanBeFetched.data[4]);

                if (numberOfMessagesToFetch > 0) {
                    messages = fetchMessagesOnSensor(numberOfMessagesToFetch);
                }
            }
            return messages;
        }

        RawDataCycles fetchRawDataCycles() final {
            RawDataCycles rawDataCycles;
            if (dataCanBeFetched.load() && numberOfRawDataCyclesToFetch > 0) {
                rawDataCycles = fetchRawDataCyclesOnSensor(numberOfRawDataCyclesToFetch);
            }
            return rawDataCycles;
        }

        void sendRequest(Request&& request) final {
        }

    private:

        typedef uint32_t NumberOfDataToFetch;

        typedef struct {
            uint32_t id;
            uint32_t timestamp;
            uint8_t flags;
            uint8_t length;
            Byte data[NUMBER_OF_DATA_BYTES];
            uint16_t padding;
        } USBSensorMessage;

        enum VersionString {
            FIRMWARE_BUILD_DATE,
            FIRMWARE_BUILD_TIME,
            FIRMWARE_VERSION_NUMBER,
            FIRMWARE_TARGET_PROCESSOR,
            FIRMWARE_APPLICATION_NAME,

            NUMBER_OF_VERSION_STRINGS
        };

        enum USBCommandCode : uint16_t {
            DO_NOTHING,

            USB_MESSAGE_LENGTH = 8,
            SEND_COMMAND = 80,
            FETCH_RAW_DATA_CYCLES = 87,
            POLL_MESSAGES = 88,
            LIdAR_QUERY = 89
        };

        void throwDeviceNotFoundErrorIfNeeded() {
            if (!usbDeviceHandle) {
                ErrorHandling::throwLibUSBDeviceNotFound();
            }
        }

        void throwUsbClaimInterfaceErrorIfNeeded(int errorCode) {
            if (errorCode < 0) {
                std::ostringstream origin("usb_claim_interface returned an error: ", std::ios_base::ate);
                origin << errorCode;
                auto libUSBErrorCode = static_cast<libusb_error>(errorCode);
                ErrorHandling::throwGuardianUSBCommunicationError(libUSBErrorCode, origin.str());
            }
        }

        void setupCleanConnection() noexcept {
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

        int doUSBBulkTransferAndReturnNumberOfByteActuallyTransferred(
                Byte endpoint,
                Byte* data,
                int32_t length) {
            uint32_t timeout = 0;
            return doUSBBulkTransferAndReturnNumberOfByteActuallyTransferredWithTimeout(
                    endpoint, data, length, timeout);
        }

        int doUSBBulkTransferAndReturnNumberOfByteActuallyTransferredWithTimeout(
                Byte endpoint,
                Byte* data,
                int32_t length,
                uint32_t timeout) {
            throwDeviceNotFoundErrorIfNeeded();
            int numberOfByteActuallyTransferred = 0;
            auto errorCode = libusb_bulk_transfer(usbDeviceHandle, endpoint, data, length,
                                                  &numberOfByteActuallyTransferred, timeout);
            throwErrorOnLibUSBBulkTransferErrorCode(errorCode);

            if (numberOfByteActuallyTransferred != length) {
                std::ostringstream errorMessage("An incorrect number of data has been transferred... \n",
                                                std::ios_base::ate);
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

        void throwErrorOnLibUSBBulkTransferErrorCode(int errorCode) {
            if (errorCode == 0) {
                return;
            }

            auto libUSBErrorCode = static_cast<libusb_error>(errorCode);
            std::string origin = " in bulk write: ";
            ErrorHandling::throwGuardianUSBCommunicationError(libUSBErrorCode, origin);
        }

        Messages fetchMessagesOnSensor(NumberOfDataToFetch numberOfMessagesToFetch) {
            Messages fetchedMessages;
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

        Message convertUSBSensorMessageToSensorMessage(USBSensorMessage* sensorMessage) {
            AWL::MessageDataArray data;
            for (auto dataNumber = 0; dataNumber < sensorMessage->length; ++dataNumber) {
                data[dataNumber] = sensorMessage->data[dataNumber];
            }
            Message
                    message(sensorMessage->id, sensorMessage->timestamp, sensorMessage->length, data);
            return message;
        }

        RawDataCycles fetchRawDataCyclesOnSensor(NumberOfDataToFetch numberOfRawDataCyclesToFetch) {
            RawDataCycles fetchedRawDataCycles;
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

            typename RawData::Content rawDataCyclesContent[MAX_FETCHABLE_SIZE];
            doUSBBulkTransferAndReturnNumberOfByteActuallyTransferred(
                    usbConnectionParameters.endpointIn,
                    (Byte*) &rawDataCyclesContent,
                    numberOfRawDataCyclesToFetch * sizeof(typename RawData::Content));


            for (auto rawDataIndex = 0u; rawDataIndex < numberOfRawDataCyclesToFetch; ++rawDataIndex) {
                auto RawDataDefinition = rawDataCyclesContent[rawDataIndex];
                fetchedRawDataCycles.at(rawDataIndex) = RawData(RawDataDefinition);
            }

            return fetchedRawDataCycles;
        }

        USBConnectionParameters usbConnectionParameters;

        libusb_context* usbContext;

        libusb_device_handle* usbDeviceHandle;

        ReconnectTime reconnectTime;

        Mutex closingMutex;

        AtomicFlag dataCanBeFetched;

        NumberOfDataToFetch numberOfMessagesToFetch;
        NumberOfDataToFetch numberOfRawDataCyclesToFetch;
    };

}

#endif //SENSORGATEWAY_GUARDIANUSBCOMMUNICATIONSTRATEGY_H
