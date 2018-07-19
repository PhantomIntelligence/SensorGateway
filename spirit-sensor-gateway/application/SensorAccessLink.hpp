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

#ifndef SPIRITSENSORGATEWAY_SENSORACCESSLINK_H
#define SPIRITSENSORGATEWAY_SENSORACCESSLINK_H

#include "spirit-sensor-gateway/sensor-communication/SensorCommunicator.hpp"
#include "spirit-sensor-gateway/message-translation/MessageTranslator.hpp"
#include "spirit-sensor-gateway/server-communication/ServerCommunicator.hpp"


namespace SpiritSensorGateway {

    template<class I, class O>
    class SensorAccessLink {
    protected:
        typedef I INPUT;
        typedef O OUTPUT;

        using SensorCommunicator = SensorAccessLinkElement::SensorCommunicator<INPUT>;
        using SensorCommunicationStrategy = SensorCommunication::SensorCommunicationStrategy<INPUT>;

        using MessageTranslator = SensorAccessLinkElement::MessageTranslator<INPUT, OUTPUT>;
        using MessageTranslationStrategy = MessageTranslation::MessageTranslationStrategy<INPUT, OUTPUT>;
        using TranslatorScheduler = DataFlow::DataProcessingScheduler<INPUT, MessageTranslator, 1>;

        using ServerCommunicator = SensorAccessLinkElement::ServerCommunicator<OUTPUT>;
        using ServerCommunicationStrategy = ServerCommunication::ServerCommunicationStrategy<OUTPUT>;
        using ServerCommunicatorScheduler = DataFlow::DataProcessingScheduler<OUTPUT, ServerCommunicator, 1>;

    public:
        explicit SensorAccessLink(ServerCommunicationStrategy* serverCommunicationStrategy,
                                  MessageTranslationStrategy* messageTranslationStrategy,
                                  SensorCommunicationStrategy* sensorCommunicationStrategy) :
                serverCommunicator(serverCommunicationStrategy),
                messageTranslator(messageTranslationStrategy),
                sensorCommunicator(sensorCommunicationStrategy),
                translatorScheduler(&messageTranslator),
                serverCommunicatorScheduler(&serverCommunicator) {
            messageTranslationStrategy->linkConsumer(&serverCommunicatorScheduler);
            sensorCommunicator.linkConsumer(&translatorScheduler);
        }

        ~SensorAccessLink() noexcept {};

        SensorAccessLink(SensorAccessLink const& other) = delete;

        SensorAccessLink(SensorAccessLink&& other) noexcept = delete;

        SensorAccessLink& operator=(SensorAccessLink const& other)& = delete;

        SensorAccessLink& operator=(SensorAccessLink&& other)& noexcept = delete;

        void connect() {
            serverCommunicator.connect();
            sensorCommunicator.connect();
        };

        void disconnect() {
            sensorCommunicator.disconnect();
            translatorScheduler.terminateAndJoin();
            serverCommunicatorScheduler.terminateAndJoin();
            serverCommunicator.disconnect();
        }

    private:

        ServerCommunicator serverCommunicator;
        MessageTranslator messageTranslator;
        SensorCommunicator sensorCommunicator;

        ServerCommunicatorScheduler serverCommunicatorScheduler;
        TranslatorScheduler translatorScheduler;
    };

}

#endif //SPIRITSENSORGATEWAY_SENSORACCESSLINK_H