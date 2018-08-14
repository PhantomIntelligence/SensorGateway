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

        typedef struct {
            uint16_t vendorId;
            uint16_t productId;
            uint16_t endpointIn;
            uint16_t endpointOut;
            uint16_t timeout;
        } USBConnectionParameters;

        USBConnectionParameters usbConnectionParameters;

        libusb_context* usbContext;

        libusb_device_handle* usbDeviceHandle;

        ReconnectTime reconnectTime;

    };

}

#endif //SENSORGATEWAY_TBDSENSORNAMEUSBCOMMUNICATIONSTRATEGY_H
