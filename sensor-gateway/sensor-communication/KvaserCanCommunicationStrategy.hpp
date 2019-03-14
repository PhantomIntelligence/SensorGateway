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

        static constexpr int32_t const CANLIB_KVASER_CAN_BIT_RATE = canBITRATE_1M;
        static constexpr uint32_t const CANLIB_TIME_SEGMENT_1 = 0;
        static constexpr uint32_t const CANLIB_TIME_SEGMENT_2 = 0;
        static constexpr uint32_t const CANLIB_SYNCHRONIZATION_JUMP_WIdTH = 0;
        static constexpr uint32_t const CANLIB_NUMBER_OF_SAMPLING_POINTS = 0;
        static constexpr uint32_t const CANLIB_SYNCMODE = 0;
        static constexpr int32_t const CANLIB_FLAGS_FOR_CHANNEL = canOPEN_EXCLUSIVE;
        static constexpr uint32_t const CANLIB_CAN_DRIVER_TYPE = canDRIVER_NORMAL;
        static constexpr uint32_t const ONE_SECOND_TIMEOUT = 1000;
        static constexpr uint64_t const CANLIB_READ_WAIT_INFINITE_DELAY = std::numeric_limits<uint64_t>::max(); // WARNING !!! --> NEVER change to infinity because the code CRASH
        static constexpr int32_t const DEFAULT_CHANNEL_ID = 0;
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

        explicit KvaserCanCommunicationStrategy(int32_t channelId = DEFAULT_CHANNEL_ID)
                : communicationChannel(), sensorChannelId(channelId) {
        }

        ~KvaserCanCommunicationStrategy() noexcept final {
            closeConnection(); // TODO: This will need to be removed or made noexcept. The destructor should be noexcept.
        }

        KvaserCanCommunicationStrategy(KvaserCanCommunicationStrategy const& other) = delete;

        KvaserCanCommunicationStrategy(KvaserCanCommunicationStrategy&& other) noexcept = delete;

        KvaserCanCommunicationStrategy& operator=(KvaserCanCommunicationStrategy const& other)& = delete;

        KvaserCanCommunicationStrategy& operator=(KvaserCanCommunicationStrategy&& other)& noexcept = delete;

        void openConnection() final {
            canInitializeLibrary();
            listChannels();
            canHandle communicationChannel = canOpenChannel(sensorChannelId, CANLIB_FLAGS_FOR_CHANNEL);
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
            auto returnCode = canWriteSync(communicationChannel, ONE_SECOND_TIMEOUT);
            throwErrorIfNecessary(returnCode, "canWriteSync");
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

        int32_t const sensorChannelId;
        canHandle communicationChannel;


        static void listChannels() {
            int chanCount = 0;
            int beta;
            char betaString[10];
            char name[256];
            char driverName[256];
            char custChanName[40];
            unsigned int ean[2], fw[2], serial[2];
            unsigned int canlibVersion;
            uint16_t fileVersion[4];

            beta = canGetVersionEx(canVERSION_CANLIB32_BETA);
            if (beta) {
                sprintf(betaString, "BETA");
            } else {
                betaString[0] = '\0';
            }
            canlibVersion = canGetVersionEx(canVERSION_CANLIB32_PRODVER);
            printf("CANlib version %d.%d %s\n",
                   canlibVersion >> 8,
                   canlibVersion & 0xff,
                   betaString);

            memset(name, 0, sizeof(name));
            memset(custChanName, 0, sizeof(custChanName));
            memset(ean, 0, sizeof(ean));
            memset(fw, 0, sizeof(fw));
            memset(serial, 0, sizeof(serial));

            canGetNumberOfChannels(&chanCount);
            printf("Found %d channel(s).\n", chanCount);

            for (auto i = 0u; i < chanCount; i++) {

                canGetChannelData(i, canCHANNELDATA_DRIVER_NAME, &driverName, sizeof(driverName));
                canGetChannelData(i, canCHANNELDATA_DLL_FILE_VERSION, &fileVersion, sizeof(fileVersion));
                canGetChannelData(i, canCHANNELDATA_DEVDESCR_ASCII, &name, sizeof(name));
                if (strcmp(name, "Kvaser Unknown") == 0) {
                    canGetChannelData(i, canCHANNELDATA_CHANNEL_NAME, &name, sizeof(name));
                }
                canGetChannelData(i, canCHANNELDATA_CARD_UPC_NO, &ean, sizeof(ean));
                canGetChannelData(i, canCHANNELDATA_CARD_SERIAL_NO, &serial, sizeof(serial));
                canGetChannelData(i, canCHANNELDATA_CARD_FIRMWARE_REV, &fw, sizeof(fw));
                (void) canGetChannelData(i, canCHANNELDATA_CUST_CHANNEL_NAME, custChanName, sizeof(custChanName));
                printf("\n\nch %2.1d: %-22s\t%x-%05x-%05x-%x, s/n %u, v%u.%u.%u %s (%s v%d.%d.%d)\n\n",
                       i, name,
                       (ean[1] >> 12), ((ean[1] & 0xfff) << 8) | ((ean[0] >> 24) & 0xff),
                       (ean[0] >> 4) & 0xfffff, (ean[0] & 0x0f),
                       serial[0],
                       fw[1] >> 16, fw[1] & 0xffff, fw[0] & 0xffff,
                       custChanName,
                       driverName, fileVersion[3], fileVersion[2], fileVersion[1]);
            }
        }
    };
}
#endif //SENSORGATEWAY_KVASERCANCOMMUNICATIONSTRATEGY_HPP
