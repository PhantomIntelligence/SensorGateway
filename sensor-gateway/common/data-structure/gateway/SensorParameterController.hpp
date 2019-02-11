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

#ifndef SENSORGATEWAY_SENSORPARAMETERCONTROLLER_HPP
#define SENSORGATEWAY_SENSORPARAMETERCONTROLLER_HPP

#include "sensor-gateway/server-communication/ServerCommunicator.hpp"

namespace SensorAccessLinkElement {

    template<class SENSOR_STRUCTURES, class SERVER_STRUCTURES>
    class SensorParameterController : public DataFlow::DataSink<typename SENSOR_STRUCTURES::ControlMessage>,
                                      public DataFlow::DataSource<ErrorHandling::SensorAccessLinkError> {

    protected:

        using ServerCommunicator = SensorAccessLinkElement::ServerCommunicator<SERVER_STRUCTURES>;
        using DataTranslator = SensorAccessLinkElement::DataTranslator<SENSOR_STRUCTURES, SERVER_STRUCTURES>;

        using ThisClass = SensorParameterController<SENSOR_STRUCTURES, SERVER_STRUCTURES>;
        using ControlMessage = typename SENSOR_STRUCTURES::ControlMessage;
        using ControlMessageScheduler = DataFlow::DataProcessingScheduler<ControlMessage, ThisClass, 1>;

        using ErrorSource = DataFlow::DataSource<ErrorHandling::SensorAccessLinkError>;

        using AvailableParameters = typename SERVER_STRUCTURES::Parameters;

    public:
        explicit SensorParameterController(ServerCommunicator* serverCommunicator,
                                           DataTranslator* dataTranslator) :
                serverCommunicator(serverCommunicator),
                dataTranslator(dataTranslator) {}

        ~SensorParameterController() noexcept {};

        SensorParameterController(SensorParameterController const& other) = delete;

        SensorParameterController(SensorParameterController&& other) noexcept = delete;

        SensorParameterController& operator=(SensorParameterController const& other)& = delete;

        SensorParameterController& operator=(SensorParameterController&& other)& noexcept = delete;

        void consume(ControlMessage&& controlMessage) override {
            /**
             *  TODO:
             *  - Create schedulers and link'em in SensorAccessLink
             *  - receive and process ControlMessage // from sensor
             *  - transmit to server w/ ServerCommunicator*
             */

        }

        template<class Request>
        void handleIncomingServerRequest(Request&& request) {
            /**
             *  TODO:
             *  - Give pointer to *this* to ServerCommunicator
             *  - have SensorCommunicator call this function in `handleServerRequest`
             *  - Story 3 -> route answer to "consume"
             *  - Story 4 -> parse `Request` and create ControlMessage -> translate in DataTranslator
             */
        }

        using ErrorSource::linkConsumer;

    private:

        ServerCommunicator* serverCommunicator;
        DataTranslator* dataTranslator;
        AvailableParameters parameters;
    };
}

#endif //SENSORGATEWAY_SENSORPARAMETERCONTROLLER_HPP
