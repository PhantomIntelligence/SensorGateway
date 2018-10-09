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


#include "LibUSBCommunicationErrorFactory.h"
#include "sensor-gateway/common/data-structure/sensor/GuardianStructures.h"
#include "SensorCommunicationStrategy.hpp"

namespace SensorCommunication {

    using Sensor::AWL::NUMBER_OF_DATA_BYTES;

    /**
     * @warning This class DOES NOT handle thread safety concerns.
     * It is meant to be called and used by a single thread from the SensorCommunicator.
     */
    class GuardianUSBCommunicationStrategy final
            : public SensorCommunicationStrategy<Sensor::Guardian::Structures> {
    protected:

        using SteadyClock = std::chrono::steady_clock;
        using ReconnectTime = std::chrono::time_point<SteadyClock>;

        using super = SensorCommunicationStrategy<Sensor::Guardian::Structures>;

        const int INTERFACE_TO_CLAIM_FROM_DEVICE = 0;
        const int VERSION_STRING_MAX_LENGTH = 32;

    public:

        explicit GuardianUSBCommunicationStrategy();

        ~GuardianUSBCommunicationStrategy() noexcept override;

        GuardianUSBCommunicationStrategy(GuardianUSBCommunicationStrategy const& other) = delete;

        GuardianUSBCommunicationStrategy(GuardianUSBCommunicationStrategy&& other) noexcept = delete;

        GuardianUSBCommunicationStrategy& operator=(GuardianUSBCommunicationStrategy const& other)& = delete;

        GuardianUSBCommunicationStrategy&
        operator=(GuardianUSBCommunicationStrategy&& other)& noexcept = delete;

        void openConnection() override;

        super::Messages fetchMessages() override;

        super::RawDataCycles fetchRawDataCycles() override;

        void closeConnection() override;

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

            USB_MESSAGE_LENGTH = 8,
            SEND_COMMAND = 80,
            FETCH_RAW_DATA_CYCLES = 87,
            POLL_MESSAGES = 88,
            LIDAR_QUERY = 89
        };

        void throwDeviceNotFoundErrorIfNeeded();

        void throwUsbClaimInterfaceErrorIfNeeded(int errorCode);

        void setupCleanConnection() noexcept;

        int doUSBBulkTransferAndReturnNumberOfByteActuallyTransferred(
                Byte endpoint,
                Byte* data,
                int32_t length);

        int doUSBBulkTransferAndReturnNumberOfByteActuallyTransferredWithTimeout(
                Byte endpoint,
                Byte* data,
                int32_t length,
                uint32_t timeout);

        void throwErrorOnLibUSBBulkTransferErrorCode(int errorCode);

        super::Messages fetchMessagesOnSensor(NumberOfDataToFetch numberOfMessagesToFetch);

        super::Message convertUSBSensorMessageToSensorMessage(USBSensorMessage* sensorMessage);

        super::RawDataCycles fetchRawDataCyclesOnSensor(NumberOfDataToFetch numberOfRawDataCyclesToFetch);

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
