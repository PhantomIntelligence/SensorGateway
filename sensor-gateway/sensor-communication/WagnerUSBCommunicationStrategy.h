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

#ifndef SENSORGATEWAY_WAGNERUSBCOMMUNICATIONSTRATEGY_H
#define SENSORGATEWAY_WAGNERUSBCOMMUNICATIONSTRATEGY_H

#include <libusb-1.0/libusb.h>
#include "SensorCommunicationStrategy.hpp"

namespace SensorCommunication {

    // NOTE: DWORD == uint32_t

    using DataFlow::AWLMessage;

    class WagnerUSBCommunicationStrategy final : public SensorCommunicationStrategy<AWLMessage> {
    protected:

    public:

        explicit WagnerUSBCommunicationStrategy();

        ~WagnerUSBCommunicationStrategy() noexcept;

        WagnerUSBCommunicationStrategy(WagnerUSBCommunicationStrategy const& other) = delete;

        WagnerUSBCommunicationStrategy(WagnerUSBCommunicationStrategy&& other) noexcept = delete;

        WagnerUSBCommunicationStrategy& operator=(WagnerUSBCommunicationStrategy const& other)& = delete;

        WagnerUSBCommunicationStrategy& operator=(WagnerUSBCommunicationStrategy&& other)& noexcept = delete;

        void openConnection() override;

        DATA readMessage() override;

        void closeConnection() override;
    };

}

#endif //SENSORGATEWAY_WAGNERUSBCOMMUNICATIONSTRATEGY_H
