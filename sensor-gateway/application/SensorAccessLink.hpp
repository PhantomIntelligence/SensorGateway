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

    template<class I, class O>
    class SensorAccessLink {
    protected:
        typedef I INPUT_STRUCTURES;
        typedef O OUTPUT;

        using SensorMessage = typename INPUT_STRUCTURES::Message;
        using SensorRawData = typename INPUT_STRUCTURES::RawData;

        using ServerCommunicator = SensorAccessLinkElement::ServerCommunicator<OUTPUT>;
        using ServerCommunicationStrategy = ServerCommunication::ServerCommunicationStrategy<OUTPUT>;
        using ServerCommunicatorScheduler = DataFlow::DataProcessingScheduler<OUTPUT, ServerCommunicator, 1>;

        using MessageTranslator = SensorAccessLinkElement::MessageTranslator<SensorMessage, OUTPUT>;
        using MessageTranslationStrategy = MessageTranslation::MessageTranslationStrategy<SensorMessage, OUTPUT>;
        using TranslatorScheduler = DataFlow::DataProcessingScheduler<SensorMessage, MessageTranslator, 1>;

        using SensorCommunicator = SensorAccessLinkElement::SensorCommunicator<INPUT_STRUCTURES>;
        using SensorCommunicationStrategy = SensorCommunication::SensorCommunicationStrategy<INPUT_STRUCTURES>;


    public:
        explicit SensorAccessLink(ServerCommunicationStrategy* serverCommunicationStrategy,
                                  MessageTranslationStrategy* messageTranslationStrategy,
                                  SensorCommunicationStrategy* sensorCommunicationStrategy) :
                serverCommunicationStrategy(serverCommunicationStrategy),
                messageTranslationStrategy(messageTranslationStrategy),
                sensorCommunicationStrategy(sensorCommunicationStrategy),
                serverCommunicator(serverCommunicationStrategy),
                messageTranslator(messageTranslationStrategy),
                sensorCommunicator(sensorCommunicationStrategy),
                translatorScheduler(&messageTranslator),
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
            messageTranslationStrategy->linkConsumer(&serverCommunicatorScheduler);
            sensorCommunicator.linkConsumer(&translatorScheduler);
        }

        ServerCommunicationStrategy* serverCommunicationStrategy;
        ServerCommunicator serverCommunicator;

        MessageTranslationStrategy* messageTranslationStrategy;
        MessageTranslator messageTranslator;

        SensorCommunicationStrategy* sensorCommunicationStrategy;
        SensorCommunicator sensorCommunicator;

        ServerCommunicatorScheduler serverCommunicatorScheduler;
        TranslatorScheduler translatorScheduler;
    };

}

#endif //SENSORGATEWAY_SENSORACCESSLINK_H
