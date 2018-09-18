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

    // TODO: Using things like `enable_if<>, handle different sensors when there is or not RawData or fetching in bulk`
    template<class SENSOR_STRUCTURES, class SERVER_STRUCTURES>
    class SensorAccessLink {

    protected:

        using SensorMessage = typename SENSOR_STRUCTURES::Message;
        using SensorRawData = typename SENSOR_STRUCTURES::RawData;

        using ServerMessage = typename SERVER_STRUCTURES::Message;
        using ServerRawData = typename SERVER_STRUCTURES::RawData;

        using ServerCommunicator = SensorAccessLinkElement::ServerCommunicator<SERVER_STRUCTURES>;
        using ServerCommunicationStrategy = ServerCommunication::ServerCommunicationStrategy<SERVER_STRUCTURES>;
        using ServerCommunicatorMessageScheduler = DataFlow::DataProcessingScheduler<ServerMessage, ServerCommunicator, 1>;
        using ServerCommunicatorRawDataScheduler = DataFlow::DataProcessingScheduler<ServerRawData, ServerCommunicator, 1>;

        using DataTranslator = SensorAccessLinkElement::DataTranslator<SENSOR_STRUCTURES, SERVER_STRUCTURES>;
        using DataTranslationStrategy = DataTranslation::DataTranslationStrategy<SENSOR_STRUCTURES, SERVER_STRUCTURES>;
        using TranslatorMessageScheduler = DataFlow::DataProcessingScheduler<SensorMessage, DataTranslator, 1>;
        using TranslatorRawDataScheduler = DataFlow::DataProcessingScheduler<SensorRawData, DataTranslator, 1>;

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
                translatorMessageScheduler(&dataTranslator),
                translatorRawDataScheduler(&dataTranslator),
                serverCommunicatorMessageScheduler(&serverCommunicator),
                serverCommunicatorRawDataScheduler(&serverCommunicator) {}

        ~SensorAccessLink() noexcept {
            terminateAndJoin();
        };

        SensorAccessLink(SensorAccessLink const& other) = delete;

        SensorAccessLink(SensorAccessLink&& other) noexcept = delete;

        SensorAccessLink& operator=(SensorAccessLink const& other)& = delete;

        SensorAccessLink& operator=(SensorAccessLink&& other)& noexcept = delete;

        void start(std::string const& serverAddress) {
            linkElements();

            // TODO: implement a while(!allUpAndRunning()) {...
            // Yield and sleep to allow correct connection
            std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            // TODO: implement }

            serverCommunicator.connect(serverAddress);
            sensorCommunicator.start();
        };

        void terminateAndJoin() {
            sensorCommunicator.terminateAndJoin();
            translatorMessageScheduler.terminateAndJoin();
            translatorRawDataScheduler.terminateAndJoin();
            serverCommunicatorMessageScheduler.terminateAndJoin();
            serverCommunicatorRawDataScheduler.terminateAndJoin();
            serverCommunicator.disconnect();
        }

    private:

        void linkElements() {
            dataTranslationStrategy->linkConsumer(&serverCommunicatorMessageScheduler);
            dataTranslationStrategy->linkConsumer(&serverCommunicatorRawDataScheduler);
            sensorCommunicator.linkConsumer(&translatorMessageScheduler);
            sensorCommunicator.linkConsumer(&translatorRawDataScheduler);
        }

        ServerCommunicationStrategy* serverCommunicationStrategy;
        ServerCommunicator serverCommunicator;
        ServerCommunicatorMessageScheduler serverCommunicatorMessageScheduler;
        ServerCommunicatorRawDataScheduler serverCommunicatorRawDataScheduler;

        DataTranslationStrategy* dataTranslationStrategy;
        DataTranslator dataTranslator;
        TranslatorMessageScheduler translatorMessageScheduler;
        TranslatorRawDataScheduler translatorRawDataScheduler;

        SensorCommunicationStrategy* sensorCommunicationStrategy;
        SensorCommunicator sensorCommunicator;

    };
}

#endif //SENSORGATEWAY_SENSORACCESSLINK_H
