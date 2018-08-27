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

#ifndef SENSORGATEWAY_SENSORACCESSLINK_H
#define SENSORGATEWAY_SENSORACCESSLINK_H

#include "sensor-gateway/server-communication/ServerCommunicator.hpp"

namespace SensorGateway {

    template<class SENSOR_STRUCTURES, class SERVER_STRUCTURES>
    class SensorAccessLink {

    protected:

        using SensorMessage = typename SENSOR_STRUCTURES::Message;
        using SensorRawData = typename SENSOR_STRUCTURES::RawData;

        using ServerMessage = typename SERVER_STRUCTURES::Message;
        using ServerRawData = typename SERVER_STRUCTURES::RawData;

        using ServerCommunicator = SensorAccessLinkElement::ServerCommunicator<ServerMessage>;
        using ServerCommunicationStrategy = ServerCommunication::ServerCommunicationStrategy<ServerMessage>;
        using ServerCommunicatorScheduler = DataFlow::DataProcessingScheduler<ServerMessage, ServerCommunicator, 1>;

        using DataTranslator = SensorAccessLinkElement::DataTranslator<SENSOR_STRUCTURES, SERVER_STRUCTURES>;
        using DataTranslationStrategy = DataTranslation::DataTranslationStrategy<SENSOR_STRUCTURES, SERVER_STRUCTURES>;
        using TranslatorScheduler = DataFlow::DataProcessingScheduler<SensorMessage, DataTranslator, 1>;

        using SensorCommunicator = SensorAccessLinkElement::SensorCommunicator<SENSOR_STRUCTURES>;
        using SensorCommunicationStrategy = SensorCommunication::SensorCommunicationStrategy<SENSOR_STRUCTURES>;

    public:
        explicit SensorAccessLink(ServerCommunicationStrategy* serverCommunicationStrategy,
                                  DataTranslationStrategy* dataTranslationStrategy,
                                  SensorCommunicationStrategy* sensorCommunicationStrategy) :
                serverCommunicationStrategy(serverCommunicationStrategy),
                dataTranslationStrategy(dataTranslationStrategy),
                sensorCommunicationStrategy(sensorCommunicationStrategy),
                serverCommunicator(serverCommunicationStrategy),
                dataTranslator(dataTranslationStrategy),
                sensorCommunicator(sensorCommunicationStrategy),
                translatorScheduler(&dataTranslator),
                serverCommunicatorScheduler(&serverCommunicator) {
        }

        ~SensorAccessLink() noexcept {
            terminateAndJoin();
        };

        SensorAccessLink(SensorAccessLink const& other) = delete;

        SensorAccessLink(SensorAccessLink&& other) noexcept = delete;

        SensorAccessLink& operator=(SensorAccessLink const& other)& = delete;

        SensorAccessLink& operator=(SensorAccessLink&& other)& noexcept = delete;

        void start(std::string const& serverAddress) {
            linkElements();

            // Yield and sleep to allow correct connection
            std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));

            serverCommunicator.connect(serverAddress);
            sensorCommunicator.start();
        };

        void terminateAndJoin() {
            sensorCommunicator.terminateAndJoin();
            translatorScheduler.terminateAndJoin();
            serverCommunicatorScheduler.terminateAndJoin();
            serverCommunicator.disconnect();
        }

    private:

        void linkElements() {
            dataTranslationStrategy->linkConsumer(&serverCommunicatorScheduler);
            sensorCommunicator.linkConsumer(&translatorScheduler);
        }

        ServerCommunicationStrategy* serverCommunicationStrategy;
        ServerCommunicator serverCommunicator;

        DataTranslationStrategy* dataTranslationStrategy;
        DataTranslator dataTranslator;

        SensorCommunicationStrategy* sensorCommunicationStrategy;
        SensorCommunicator sensorCommunicator;

        ServerCommunicatorScheduler serverCommunicatorScheduler;
        TranslatorScheduler translatorScheduler;
    };
}

#endif //SENSORGATEWAY_SENSORACCESSLINK_H
