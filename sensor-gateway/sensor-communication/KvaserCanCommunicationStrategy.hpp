/**
	Copyright 2014-2019 Phantom Intelligence Inc.

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
#ifndef SENSORGATEWAY_KVASERCANCOMMUNICATIONSTRATEGY_HPP
#define SENSORGATEWAY_KVASERCANCOMMUNICATIONSTRATEGY_HPP

#include <canlib.h>

#include "sensor-gateway/common/data-structure/sensor/AWLStructures.h"
#include "SensorCommunicationStrategy.hpp"
#include "KvaserCommunicationErrorFactory.h"

namespace SensorCommunication {

    using Sensor::AWL::NUMBER_OF_DATA_BYTES;

    template<typename AWLStructures>
    class KvaserCanCommunicationStrategy final : public SensorCommunicationStrategy<AWLStructures> {

    protected:

        unsigned long const CANLIB_READ_WAIT_INFINITE_DELAY = -1; // WARNING !!! --> NEVER change -1 with infinity because the code CRASH
        using super = SensorCommunicationStrategy<AWLStructures>;

        using Message = typename super::Message;
        using Messages = typename super::Messages;
        using RawData = typename super::RawData;
        using RawDataCycles = typename super::RawDataCycles;
        using Request = typename super::Message;

        typedef struct {
            int64_t id;
            uint64_t timestamp;
            uint32_t flags;
            uint32_t length;
            uint8_t data[NUMBER_OF_DATA_BYTES];
        } CanMessage;

    public:

        explicit KvaserCanCommunicationStrategy(int32_t const& channelId) : communicationChannel() {
        }

        ~KvaserCanCommunicationStrategy() noexcept final {
            closeConnection(); // TODO: This will need to be removed or made noexcept. The destructor should be noexcept.
        }

        KvaserCanCommunicationStrategy(KvaserCanCommunicationStrategy const& other) = delete;

        KvaserCanCommunicationStrategy(KvaserCanCommunicationStrategy&& other) noexcept = delete;

        KvaserCanCommunicationStrategy& operator=(KvaserCanCommunicationStrategy const& other)& = delete;

        KvaserCanCommunicationStrategy& operator=(KvaserCanCommunicationStrategy&& other)& noexcept = delete;

        void openConnection() final {
            static constexpr int32_t const CANLIB_KVASER_CAN_BIT_RATE = canBITRATE_1M;
            static constexpr uint32_t const CANLIB_TIME_SEGMENT_1 = 0;
            static constexpr uint32_t const CANLIB_TIME_SEGMENT_2 = 0;
            static constexpr uint32_t const CANLIB_SYNCHRONIZATION_JUMP_WIdTH = 0;
            static constexpr uint32_t const CANLIB_NUMBER_OF_SAMPLING_POINTS = 0;
            static constexpr uint32_t const CANLIB_SYNCMODE = 0;
            static constexpr int32_t const CANLIB_FLAGS_FOR_CHANNEL = canOPEN_EXCLUSIVE;
            static constexpr uint32_t const CANLIB_CAN_DRIVER_TYPE = canDRIVER_NORMAL;
            static constexpr int32_t const CANLIB_CHANNEL_ID = 0;
            canInitializeLibrary();
            canHandle communicationChannel = canOpenChannel(CANLIB_CHANNEL_ID, CANLIB_FLAGS_FOR_CHANNEL);
            auto returnCode = canSetBusParams(communicationChannel,
                                              CANLIB_KVASER_CAN_BIT_RATE,
                                              CANLIB_TIME_SEGMENT_1,
                                              CANLIB_TIME_SEGMENT_2,
                                              CANLIB_SYNCHRONIZATION_JUMP_WIdTH,
                                              CANLIB_NUMBER_OF_SAMPLING_POINTS,
                                              CANLIB_SYNCMODE);
            throwErrorIfNecessary(returnCode, "canSetBusParams");
            returnCode = canSetBusOutputControl(communicationChannel, CANLIB_CAN_DRIVER_TYPE);
            throwErrorIfNecessary(returnCode, "canSetBusOutputControl");
            returnCode = canBusOn(communicationChannel);
            throwErrorIfNecessary(returnCode, "canBusOn");
        }

        void closeConnection() final {
            auto returnCode = canBusOff(communicationChannel);
            throwErrorIfNecessary(returnCode, "canBusOff");
            returnCode = canClose(communicationChannel);
            throwErrorIfNecessary(returnCode, "canClose");
        }

        Messages fetchMessages() final {
            CanMessage canMessage{};
            auto returnCode = canReadWait(communicationChannel,
                                          &canMessage.id,
                                          &canMessage.data,
                                          &canMessage.length,
                                          &canMessage.flags,
                                          &canMessage.timestamp,
                                          CANLIB_READ_WAIT_INFINITE_DELAY);

            throwErrorIfNecessary(returnCode, "canReadWait");
            auto message = convertCanMessageToSensorMessage(canMessage);
            Messages messages = {message};
            return messages;
        }

        RawDataCycles fetchRawDataCycles() final {
            RawDataCycles rawDataCycles;
            return rawDataCycles;
        }

        void sendRequest(Request&& request) final {
            CanMessage canMessage;
        }

    private:

        void throwErrorIfNecessary(canStatus const& errorCode, std::string const& callOrigin) {
            if (CANSTATUS_FAILURE(errorCode)) {
                ErrorHandling::throwKvaserCommunicationError(errorCode, callOrigin);
            }
        }

        Message convertCanMessageToSensorMessage(CanMessage canMessage) {
            AWL::MessageDataArray data;
            for (auto dataIndex = 0; dataIndex < canMessage.length; ++dataIndex) {
                data[dataIndex] = canMessage.data[dataIndex];
            }

            Message message(canMessage.id, canMessage.timestamp, canMessage.length, data);

            return message;
        }

        CanMessage convertSensorMessageToCanMessage(Message message) {
            CanMessage canMessage;
            for (auto dataIndex = 0; dataIndex < canMessage.length; ++dataIndex) {
                canMessage.data[dataIndex] = message.data[dataIndex];
            }

            canMessage.id = message.id;
            canMessage.timestamp = message.timestamp;
            canMessage.flags = 0;
            canMessage.length = message.length;

            return canMessage;
        }

        void writeMessage(CanMessage const& message) {
            auto returnCode = canWriteWait(communicationChannel,
                                           message.id,
                                           (void*) message.data,
                                           message.length,
                                           0,
                                           1000);
            throwErrorIfNecessary(returnCode, "canWriteWait");
        }

        canHandle communicationChannel;
    };
}
#endif //SENSORGATEWAY_KVASERCANCOMMUNICATIONSTRATEGY_HPP
