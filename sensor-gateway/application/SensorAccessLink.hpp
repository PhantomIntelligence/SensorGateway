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

#include "sensor-gateway/common/data-structure/gateway/GatewayStructures.h"
#include "sensor-gateway/parameter-control/SensorParameterController.hpp"

namespace SensorGateway {

    namespace Details {

        // TODO: Using things like `enable_if<>, handle different sensors when there is or not RawData or fetching in bulk`
        // TODO: Use `declval` to create a stream line for Message and RawData, abstracting what actual type is processed (scheduler, emplacement in each of SALE.) https://en.cppreference.com/w/cpp/utility/declval
        template<class SENSOR_STRUCTURES, class SERVER_STRUCTURES>
        class SensorAccessLink : public DataFlow::DataSink<ErrorHandling::SensorAccessLinkError> {

        protected:

            using SensorMessage = typename SENSOR_STRUCTURES::Message;
            using SensorRawData = typename SENSOR_STRUCTURES::RawData;

            using ServerMessage = typename SERVER_STRUCTURES::Message;
            using ServerRawData = typename SERVER_STRUCTURES::RawData;

            using ServerCommunicator = SensorAccessLinkElement::ServerCommunicator<SERVER_STRUCTURES>;
            using ServerCommunicatorMessageScheduler = DataFlow::DataProcessingScheduler<ServerMessage, ServerCommunicator, ONLY_ONE_PRODUCER>;
            using ServerCommunicatorRawDataScheduler = DataFlow::DataProcessingScheduler<ServerRawData, ServerCommunicator, ONLY_ONE_PRODUCER>;

            using DataTranslator = SensorAccessLinkElement::DataTranslator<SENSOR_STRUCTURES, SERVER_STRUCTURES>;
            using TranslatorMessageScheduler = DataFlow::DataProcessingScheduler<SensorMessage, DataTranslator, ONLY_ONE_PRODUCER>;
            using TranslatorRawDataScheduler = DataFlow::DataProcessingScheduler<SensorRawData, DataTranslator, ONLY_ONE_PRODUCER>;

            using SensorParameterController = SensorAccessLinkElement::SensorParameterController<SENSOR_STRUCTURES, SERVER_STRUCTURES>;
            using RequestHandler = SensorAccessLinkElement::RequestHandler<SENSOR_STRUCTURES, SERVER_STRUCTURES>;

            using SensorCommunicator = SensorAccessLinkElement::SensorCommunicator<SENSOR_STRUCTURES>;
            using SensorCommunicatorRequestScheduler = typename SensorCommunicator::RequestProcessingScheduler;

            using Error = ErrorHandling::SensorAccessLinkError;
            using ThisClass = SensorAccessLink<SENSOR_STRUCTURES, SERVER_STRUCTURES>;
            using ErrorScheduler = DataFlow::DataProcessingScheduler<Error, ThisClass, 5>;

        public:

            using ServerCommunicationStrategy = typename ServerCommunicator::ServerCommunicationStrategy;
            using DataTranslationStrategy = typename DataTranslator::DataTranslationStrategy;
            using SensorCommunicationStrategy = typename SensorCommunicator::SensorCommunicationStrategy;

            explicit SensorAccessLink(ServerCommunicationStrategy* serverCommunicationStrategy,
                                      DataTranslationStrategy* dataTranslationStrategy,
                                      SensorCommunicationStrategy* sensorCommunicationStrategy) :
                    serverCommunicationStrategy(serverCommunicationStrategy),
                    dataTranslationStrategy(dataTranslationStrategy),
                    sensorCommunicationStrategy(sensorCommunicationStrategy),
                    serverCommunicator(serverCommunicationStrategy,
                                       std::bind(&RequestHandler::handleGetParameterValueRequest, &requestHandler,
                                                 std::placeholders::_1)),
                    requestHandler(&serverCommunicator,
                                   std::bind(&SensorParameterController::process, &sensorParameterController,
                                             std::placeholders::_1)),
                    sensorParameterController(&requestHandler, &dataTranslator),
                    dataTranslator(dataTranslationStrategy),
                    sensorCommunicator(sensorCommunicationStrategy),
                    sensorCommunicatorRequestScheduler(&sensorCommunicator),
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
                sensorCommunicatorRequestScheduler.terminateAndJoin();
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
                // TODO: THE ARCHITECTURAL LOCATION WILL HAVE TO BE RETHOUGHT WHEN IMPLEMENTING GATEWAY PROTOCOL

                std::cout << "Error handling (SensorAccessLink) : " << error.fetchDetailedMessage() << std::endl;
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
                dataTranslator.linkConsumer(&sensorCommunicatorRequestScheduler);
                dataTranslationStrategy->linkConsumer(&serverCommunicatorMessageScheduler);
                dataTranslationStrategy->linkConsumer(&serverCommunicatorRawDataScheduler);
                sensorCommunicator.linkConsumer(&translatorMessageScheduler);
                sensorCommunicator.linkConsumer(&translatorRawDataScheduler);
                sensorParameterController.linkSensorControlMessageResponseSource(dataTranslationStrategy);

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
            SensorCommunicatorRequestScheduler sensorCommunicatorRequestScheduler;

            ErrorScheduler errorScheduler;
        };
    }

    template<typename S>
    struct SensorAccessLinkFactory {

        using GatewayStructures = typename Sensor::Gateway::Structures<
                typename S::MessageDefinition,
                typename S::RawDataDefinition,
                typename S::ControlMessageDefinition,
                typename S::Parameters>;
        using AccessLink = Details::SensorAccessLink<S, GatewayStructures>;

    };
}

#endif //SENSORGATEWAY_SENSORACCESSLINK_HPP
