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

#ifndef SENSORGATEWAY_REQUESTHANDLER_HPP
#define SENSORGATEWAY_REQUESTHANDLER_HPP

#include "sensor-gateway/data-translation/DataTranslator.hpp"
#include "ServerRequest.hpp"

namespace SensorAccessLinkElement {


    // WARNING : This declaration is made here to break a circular dependency. DO NOT REMOVE
    template<class SENSOR_STRUCTURES, class SERVER_STRUCTURES>
    class ResponseWriter;

    template<class SENSOR_STRUCTURES, class SERVER_STRUCTURES>
    class RequestHandler : public DataFlow::DataSource<ErrorHandling::SensorAccessLinkError> {

    protected:

        using DataTranslator = SensorAccessLinkElement::DataTranslator<SENSOR_STRUCTURES, SERVER_STRUCTURES>;
        using ResponseWriter = SensorAccessLinkElement::ResponseWriter<SENSOR_STRUCTURES, SERVER_STRUCTURES>;

        using ControlMessage = typename SENSOR_STRUCTURES::ControlMessage;
        using AvailableParameters = typename SERVER_STRUCTURES::Parameters;  // TODO: Extract this in a `Sensor` or `SensorParameters` class

        using ErrorSource = DataFlow::DataSource<ErrorHandling::SensorAccessLinkError>;

        using GetParameterValueRequest = ServerCommunication::RequestTypes::GetParameterValue;

    public:
        explicit RequestHandler(ResponseWriter* responseWriter,
                                DataTranslator* dataTranslator) :
                responseWriter(responseWriter),
                dataTranslator(dataTranslator) {}

        ~RequestHandler() noexcept {};

        RequestHandler(RequestHandler const& other) = delete;

        RequestHandler(RequestHandler&& other) noexcept = delete;

        RequestHandler& operator=(RequestHandler const& other)& = delete;

        RequestHandler& operator=(RequestHandler&& other)& noexcept = delete;

        virtual void handleGetParameterValueRequest(GetParameterValueRequest&& getParameterValueRequest) {
            // TODO : ensure request is valid, if not getParameterValueRequest.makeBad();
            /**
             *  TODO:
             *  - Give pointer to *this* to ServerCommunicator
             *  - have SensorCommunicator call this function in `handleServerRequest`
             *  - Story 3 -> Give `Request` to ResponseWriter and stop
             *  - Story 4 -> Add Request parsing and send result to `DataTranslator`
             */
        }

        using ErrorSource::linkConsumer;

    private:

        ResponseWriter* responseWriter;
        DataTranslator* dataTranslator;

        AvailableParameters parameters;
    };
}

#endif //SENSORGATEWAY_REQUESTHANDLER_HPP
