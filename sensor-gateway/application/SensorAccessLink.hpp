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

#ifndef SENSORGATEWAY_SENSORACCESSLINK_HPP
#define SENSORGATEWAY_SENSORACCESSLINK_HPP

#include "sensor-gateway/parameter-control/SensorParameterController.hpp"

namespace SensorGateway {

    // TODO: Use `type_index` to facilitate the mapping to the correct SensorAccessLink type : https://en.cppreference.com/w/cpp/types/type_index
    // TODO: Using things like `enable_if<>, handle different sensors when there is or not RawData or fetching in bulk`
    template<class SENSOR_STRUCTURES, class SERVER_STRUCTURES>
    class SensorAccessLink : public DataFlow::DataSink<ErrorHandling::SensorAccessLinkError> {

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

        using SensorParameterController = SensorAccessLinkElement::SensorParameterController<SENSOR_STRUCTURES, SERVER_STRUCTURES>;
        using RequestHandler = SensorAccessLinkElement::RequestHandler<SENSOR_STRUCTURES, SERVER_STRUCTURES>;

        using SensorCommunicator = SensorAccessLinkElement::SensorCommunicator<SENSOR_STRUCTURES>;
        using SensorCommunicationStrategy = SensorCommunication::SensorCommunicationStrategy<SENSOR_STRUCTURES>;

        using Error = ErrorHandling::SensorAccessLinkError;
        using ThisClass = SensorAccessLink<SENSOR_STRUCTURES, SERVER_STRUCTURES>;
        using ErrorScheduler = DataFlow::DataProcessingScheduler<Error, ThisClass, 5>;

    public:
        explicit SensorAccessLink(ServerCommunicationStrategy* serverCommunicationStrategy,
                                  DataTranslationStrategy* dataTranslationStrategy,
                                  SensorCommunicationStrategy* sensorCommunicationStrategy) :
                serverCommunicationStrategy(serverCommunicationStrategy),
                dataTranslationStrategy(dataTranslationStrategy),
                sensorCommunicationStrategy(sensorCommunicationStrategy),
                serverCommunicator(serverCommunicationStrategy,
                                   std::bind(&RequestHandler::handleGetParameterValueRequest, &requestHandler,
                                             std::placeholders::_1)),
                requestHandler(&serverCommunicator, &sensorParameterController),
                sensorParameterController(&requestHandler, &dataTranslator),
                dataTranslator(dataTranslationStrategy),
                sensorCommunicator(sensorCommunicationStrategy),
                translatorMessageScheduler(&dataTranslator),
                translatorRawDataScheduler(&dataTranslator),
                serverCommunicatorMessageScheduler(&serverCommunicator),
                serverCommunicatorRawDataScheduler(&serverCommunicator),
                errorScheduler(this) // TODO : Change "this" for the SensorAccessLinkErrorHandler
        {}

        ~SensorAccessLink() noexcept {
            terminateAndJoin();
        };

        SensorAccessLink(SensorAccessLink const& other) = delete;

        SensorAccessLink(SensorAccessLink&& other) noexcept = delete;

        SensorAccessLink& operator=(SensorAccessLink const& other)& = delete;

        SensorAccessLink& operator=(SensorAccessLink&& other)& noexcept = delete;

        void start(std::string const& serverAddress) {
            linkElements();

            serverCommunicator.openConnection(serverAddress);
            sensorCommunicator.start();
        };

        void terminateAndJoin() {
            sensorCommunicator.terminateAndJoin();
            translatorMessageScheduler.terminateAndJoin();
            translatorRawDataScheduler.terminateAndJoin();
            sensorParameterController.terminateAndJoin();
            serverCommunicatorMessageScheduler.terminateAndJoin();
            serverCommunicatorRawDataScheduler.terminateAndJoin();
            serverCommunicator.terminateAndJoin();

            errorScheduler.terminateAndJoin();
        }

        // TODO : extract this in a SensorAccessLinkErrorHandler, test behavior then.
        void consume(Error&& error) override {

            // NOTE: To allow Gateway stability on RP3, this is good enough, but untested.
            // TODO: THE ARCHITECTURAL LOCATION WILL HAVE TO BE RETHOUGHT WHEN IMPLEMENTING SPIRIT PROTOCOL

            std::cout << error.fetchDetailedMessage() << std::endl;
            if (error.isFatalForGateway()) {
                // TODO : shutdown SensorAccessLink instance + kill whole gateway, throw
                DetachableThread(&SensorAccessLink::terminateAndJoin, this);
                // Avoid deadlock in errorScheduler::terminateAndJoin
            } else if (error.isFatalForSensorAccess()) {
                // TODO : sendMessage to server + shutdown SensorAccessLink instance
                DetachableThread(&SensorAccessLink::terminateAndJoin, this);
                // Avoid deadlock in errorScheduler::terminateAndJoin
            } else {
                // TODO : sendErrorMessageToServer();
            }
        }

    private:

        void linkElements() {
            dataTranslationStrategy->linkConsumer(&serverCommunicatorMessageScheduler);
            dataTranslationStrategy->linkConsumer(&serverCommunicatorRawDataScheduler);
            sensorCommunicator.linkConsumer(&translatorMessageScheduler);
            sensorCommunicator.linkConsumer(&translatorRawDataScheduler);
            sensorParameterController.linkElements();

            sensorCommunicator.linkConsumer(&errorScheduler);
            dataTranslator.linkConsumer(&errorScheduler);
            serverCommunicator.linkConsumer(&errorScheduler);
            requestHandler.linkConsumer(&errorScheduler);
            sensorParameterController.linkConsumer(&errorScheduler);

            // TODO: implement a while(!allUpAndRunning()) {...
            // Yield and sleep to allow correct connection
            std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            // TODO: implement }
        }

        ServerCommunicationStrategy* serverCommunicationStrategy;
        ServerCommunicator serverCommunicator;
        ServerCommunicatorMessageScheduler serverCommunicatorMessageScheduler;
        ServerCommunicatorRawDataScheduler serverCommunicatorRawDataScheduler;

        DataTranslationStrategy* dataTranslationStrategy;
        DataTranslator dataTranslator;
        TranslatorMessageScheduler translatorMessageScheduler;
        TranslatorRawDataScheduler translatorRawDataScheduler;

        SensorParameterController sensorParameterController;
        RequestHandler requestHandler;

        SensorCommunicationStrategy* sensorCommunicationStrategy;
        SensorCommunicator sensorCommunicator;

        ErrorScheduler errorScheduler;
    };
}

#endif //SENSORGATEWAY_SENSORACCESSLINK_HPP
