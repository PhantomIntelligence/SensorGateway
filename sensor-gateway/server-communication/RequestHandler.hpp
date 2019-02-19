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

#include "ServerRequest.hpp"
#include "RequestHandlingErrorFactory.h"

namespace SensorAccessLinkElement {

    // WARNING : This declaration is made here to break a circular dependency. DO NOT REMOVE
    template<class SERVER_STRUCTURES>
    class ServerCommunicator;

    // WARNING : This declaration is made here to break a circular dependency. DO NOT REMOVE
    template<class SENSOR_STRUCTURES, class SERVER_STRUCTURES>
    class SensorParameterController;

    template<class SENSOR_STRUCTURES, class SERVER_STRUCTURES>
    class RequestHandler : public DataFlow::DataSource<ErrorHandling::SensorAccessLinkError> {

    protected:

        using SensorParameterController = SensorAccessLinkElement::SensorParameterController<SENSOR_STRUCTURES, SERVER_STRUCTURES>;
        using ServerCommunicator = SensorAccessLinkElement::ServerCommunicator<SERVER_STRUCTURES>;

        using ControlMessage = typename SENSOR_STRUCTURES::ControlMessage;
        using AvailableParameters = typename SERVER_STRUCTURES::Parameters;

        using ErrorSource = DataFlow::DataSource<ErrorHandling::SensorAccessLinkError>;

        using GetParameterValueRequest = ServerCommunication::RequestTypes::GetParameterValue;

    public:
        explicit RequestHandler(ServerCommunicator* serverCommunicator,
                                SensorParameterController* sensorParameterController) :
                serverCommunicator(serverCommunicator),
                sensorParameterController(sensorParameterController) {}

        ~RequestHandler() noexcept {};

        RequestHandler(RequestHandler const& other) = delete;

        RequestHandler(RequestHandler&& other) noexcept = delete;

        RequestHandler& operator=(RequestHandler const& other)& = delete;

        RequestHandler& operator=(RequestHandler&& other)& noexcept = delete;

        void ensureParameterIsAvailable(std::string const& parameterName) {
            auto parameterAvailable = parameters.isAvailable(parameterName);
            if (!parameterAvailable) {
                ErrorHandling::throwRequestHandlingError(
                        ErrorHandling::INVALID_PARAMETER_NAME,
                        ErrorHandling::Origin::SERVER_REQUEST_HANDLING_PARAMETER,
                        ErrorHandling::Message::PARAMETER_NOT_AVAILABLE);
            }
        }

        virtual void handleGetParameterValueRequest(GetParameterValueRequest&& getParameterValueRequest) {
            try {
                ensureParameterIsAvailable(getParameterValueRequest.getPayloadName());
            } catch(ErrorHandling::SensorAccessLinkError& error) {

                ErrorSource::produce(std::move(error));
            }

            // TODO : If request.isBadRequest() ->
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

        ServerCommunicator* serverCommunicator;
        SensorParameterController* sensorParameterController;

        AvailableParameters parameters;
    };
}

#endif //SENSORGATEWAY_REQUESTHANDLER_HPP
