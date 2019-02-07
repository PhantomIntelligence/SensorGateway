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
#ifndef SENSORGATEWAY_KVASERCANCOMMUNICATIONSTRATEGY_H
#define SENSORGATEWAY_KVASERCANCOMMUNICATIONSTRATEGY_H

#include <canlib.h>

#include "sensor-gateway/common/data-structure/sensor/AWLStructures.h"
#include "SensorCommunicationStrategy.hpp"

namespace SensorCommunication {

    using Sensor::AWL::NUMBER_OF_DATA_BYTES;

    class KvaserCanCommunicationStrategy final : public SensorCommunicationStrategy<Sensor::AWL::Structures> {

        unsigned long const CANLIB_READ_WAIT_INFINITE_DELAY = -1; // WARNING !!! --> NEVER change -1 with infinity because the code CRASH
        using super = SensorCommunicationStrategy<Sensor::AWL::Structures>;

    public:
        explicit KvaserCanCommunicationStrategy();

        ~KvaserCanCommunicationStrategy() noexcept final;

        KvaserCanCommunicationStrategy(KvaserCanCommunicationStrategy const& other) = delete;

        KvaserCanCommunicationStrategy(KvaserCanCommunicationStrategy&& other) noexcept = delete;

        KvaserCanCommunicationStrategy& operator=(KvaserCanCommunicationStrategy const& other)& = delete;

        KvaserCanCommunicationStrategy&
        operator=(KvaserCanCommunicationStrategy&& other)& noexcept = delete;

        void openConnection() final;

        super::Messages fetchMessages() final;

        super::RawDataCycles fetchRawDataCycles() final;

        void closeConnection() final;

    private:
        void throwErrorIfNecessary(canStatus const& errorCode, std::string const& callOrigin);

        typedef struct {
            int64_t id;
            uint64_t timestamp;
            uint32_t flags;
            uint32_t length;
            uint8_t data[NUMBER_OF_DATA_BYTES];
        } CanMessage;

        super::Message convertCanMessageToSensorMessage(CanMessage canMessage);

        canHandle communicationChannel;
    };
}
#endif //SENSORGATEWAY_KVASERCANCOMMUNICATIONSTRATEGY_H
