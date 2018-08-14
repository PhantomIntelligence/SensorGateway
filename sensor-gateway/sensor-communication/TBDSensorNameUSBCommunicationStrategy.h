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

#ifndef SENSORGATEWAY_TBDSENSORNAMEUSBCOMMUNICATIONSTRATEGY_H
#define SENSORGATEWAY_TBDSENSORNAMEUSBCOMMUNICATIONSTRATEGY_H

#include <libusb-1.0/libusb.h>
#include "SensorCommunicationStrategy.hpp"

namespace SensorCommunication {

    // NOTE: DWORD == uint32_t

    using DataFlow::AWLMessage;

    class TBDSensorNameUSBCommunicationStrategy final : public SensorCommunicationStrategy<AWLMessage> {
    protected:

        using SteadyClock = std::chrono::steady_clock;
        using ReconnectTime = std::chrono::time_point<SteadyClock>;

        using super = SensorCommunicationStrategy<AWLMessage>;
        using super::DATA;

        const int INTERFACE_TO_CLAIM_FROM_DEVICE = 0;
        const int VERSION_STRING_MAX_LENGTH = 32;

    public:

        explicit TBDSensorNameUSBCommunicationStrategy();

        ~TBDSensorNameUSBCommunicationStrategy() noexcept override;

        TBDSensorNameUSBCommunicationStrategy(TBDSensorNameUSBCommunicationStrategy const& other) = delete;

        TBDSensorNameUSBCommunicationStrategy(TBDSensorNameUSBCommunicationStrategy&& other) noexcept = delete;

        TBDSensorNameUSBCommunicationStrategy& operator=(TBDSensorNameUSBCommunicationStrategy const& other)& = delete;

        TBDSensorNameUSBCommunicationStrategy&
        operator=(TBDSensorNameUSBCommunicationStrategy&& other)& noexcept = delete;

        void openConnection() override;

        DATA readMessage() override;

        void closeConnection() override;


    private:

        void throwDeviceNotFoundErrorIfNeeded();

        void throwUsbClaimInterfaceErrorIfNeeded(int errorCode);

        int doUSBBulkTransferAndReturnNumberOfByteActuallyTransferred(
                Byte endpoint,
                Byte* data,
                int length);

        void throwErrorOnLibUSBBulkTransfetErrorCode(int errorCode);

        typedef struct {
            uint16_t vendorId;
            uint16_t productId;
            uint8_t endpointIn;
            uint8_t endpointOut;
            uint16_t timeout;
        } USBConnectionParameters;

        enum VersionString {
            FIRMWARE_BUILD_DATE,
            FIRMWARE_BUILD_TIME,
            FIRMWARE_VERSION_NUMBER,
            FIRMWARE_TARGET_PROCESSOR,
            FIRMWARE_APPLICATION_NAME,

            NUMBER_OF_VERSION_STRINGS
        };

        enum USBCommandCode {
            DO_NOTHING,
            GET_FIRMWARE_VERSION,
            QUERY_FOR_SUPPORT,
            QUERY_REPLY,

            QUERY_ON_WHICH_USB_PORT_THE_CONNECTION_IS_MADE,

            MEMORY_READ,
            MEMORY_WRITE,

            LIDAR_QUERY,
            LIDAR_GET_DATA,

            USBIO_START,
            USBIO_STOP,
            USBIO_OPEN_FILE_ON_HOST,
            USBIO_CLOSE_FILE_ON_HOST,
            USBIO_READ_FILE_ON_HOST,
            USBIO_READ_REPLY_FROM_HOST,
            USBIO_WRITE_FILE_ON_HOST,
            USBIO_WRITE_REPLY_TO_HOST,
            USBIO_SEEK_FROM_CURRENT_POSITION_IN_FILE_ON_HOST,
            USBIO_SEEK_FROM_END_OF_FILE_ON_HOST,
            USBIO_SEEK_FROM_START_OF_FILE_ON_HOST,
            USBIO_SEEK_REPLY_FROM_HOST,
            USBIO_SEND_FILE_POINTER,

            CUSTOM_COMMAND,

            REPEAT_OUT = 100,
            REPEAT_IN
        };

        typedef struct {
            uint32_t command;
            uint32_t numberOfByteToTransfer;
            uint32_t data;
        } USBCommandBlock;

        typedef struct {
            uint32_t command;
            uint32_t numberOfCyclesReady;
            uint32_t numberOfBytes;
            uint32_t nextMessageLength;
        } ADIBulkLoopbackLidarQueryResponse;

        USBConnectionParameters usbConnectionParameters;

        libusb_context* usbContext;

        libusb_device_handle* usbDeviceHandle;

        ReconnectTime reconnectTime;

        Mutex closingMutex;
    };

}

#endif //SENSORGATEWAY_TBDSENSORNAMEUSBCOMMUNICATIONSTRATEGY_H
