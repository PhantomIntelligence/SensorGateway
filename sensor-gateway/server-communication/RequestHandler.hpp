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

#include "ServerResponseAssembler.hpp"
#include "ServerRequestAssembler.hpp"

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
        using AllParameterMetadata = typename AvailableParameters::AllParameterMetadata;

        using ErrorSource = DataFlow::DataSource<ErrorHandling::SensorAccessLinkError>;

        using GetAllParameterMetadataRequest = ServerCommunication::RequestTypes::GetAllParameterMetadata;

        using GetParameterValueRequest = ServerCommunication::RequestTypes::GetParameterValue;

        using GetParameter = StringLiteral<decltype("get parameter"_ToString)>;

    public:

        using ProcessGetParameterValueRequest  = StringLiteral<decltype("ProcessGetParameterValueRequest"_ToString)>;
        using ProcessCalibrationRequest        = StringLiteral<decltype("ProcessCalibrationRequest"_ToString)>;
        using ProcessClearCalibrationRequest   = StringLiteral<decltype("ProcessClearCalibrationRequest"_ToString)>;

        using ParameterRequestCallBackStore =
        typename CallBack
                <
                        std::function<void(GetParameterValueRequest&&)>,
                        std::function<void()>,
                        std::function<void()>
                >
        ::UsingArgument
                <
                        GetParameterValueRequest&&,
                        EmptyDescription,
                        EmptyDescription
                >
        ::Named<
                ProcessGetParameterValueRequest,
                ProcessCalibrationRequest,
                ProcessClearCalibrationRequest
        >;

        using ParameterRequestCallBacks = typename ParameterRequestCallBackStore::InstancesTuple;

        explicit RequestHandler(ServerCommunicator* serverCommunicator,
                                ParameterRequestCallBacks* parameterRequestCallBacks) :
                serverCommunicator(serverCommunicator),
                parameterRequestCallBacks(parameterRequestCallBacks) {
        }

        ~RequestHandler() noexcept {
        };

        RequestHandler(RequestHandler const& other) = delete;

        RequestHandler(RequestHandler&& other) noexcept = delete;

        RequestHandler& operator=(RequestHandler const& other)& = delete;

        RequestHandler& operator=(RequestHandler&& other)& noexcept = delete;

        // TODO : move this in SensorParameterController
        // TODO: test this function
        virtual void handleGetAllParameterNamesRequest() {
            AllParameterMetadata allParameterMetadata = parameters.getMetadataForAllParameters();
            GetAllParameterMetadataRequest getAllParameterMetadataRequest;
            auto parameterValueResponse = ResponseAssembler::createAllParameterMetadataResponse<
                    AllParameterMetadata,
                    GetAllParameterMetadataRequest,
                    AvailableParameters::NUMBER_OF_AVAILABLE_PARAMETERS
            >(
                    std::move(allParameterMetadata),
                    std::move(getAllParameterMetadataRequest)
            );

            serverCommunicator->sendResponse(std::move(parameterValueResponse));
        }

        // TODO : move this in SensorParameterController
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
            auto requestIsValid = true;
            std::string const& parameterName = getParameterValueRequest.payloadToString();
            try {
                ensureParameterIsAvailable(parameterName);
            } catch (ErrorHandling::SensorAccessLinkError& error) {
                requestIsValid = false;
                ErrorSource::produce(std::move(error));
                getParameterValueRequest.markAsBadRequest();
                writeAndSendErrorMessageResponse<GetParameter>(std::move(getParameterValueRequest));
            }

            if (requestIsValid) {
                getCallBack<ProcessGetParameterValueRequest>()(std::move(getParameterValueRequest));
            }
        }

// TODO : Test this and complete with actual value from Sensor --> integration test
        virtual void handleCalibrationRequest() {
            getCallBack<ProcessCalibrationRequest>()();
        }

// TODO : Test this and complete with actual value from Sensor --> integration test
        virtual void handleClearCalibrationRequest() {
            getCallBack<ProcessClearCalibrationRequest>()();
        }

// TODO : Test this and complete with actual value from Sensor --> integration test
        template<typename P, typename Value, typename Request>
        void writeAndSendParameterValueResponse(P* parameter, Value value, Request&& originalRequest) const {
            auto parameterData = parameter->extractMetadata();
            auto parameterValueResponse = ResponseAssembler::createParameterValueResponse(
                    parameterData,
                    value,
                    std::forward<Request>(originalRequest)
            );
            serverCommunicator->sendResponse(std::move(parameterValueResponse));
        }

// TODO : Test this and complete with error value from Sensor --> integration test
        template<typename P, typename Request>
        void writeAndSendParameterErrorResponse(P* parameter, Request&& originalRequest) const {
            auto parameterData = parameter->extractMetadata();
            auto parameterErrorResponse = ResponseAssembler::createParameterErrorResponse(
                    parameterData,
                    std::forward<Request>(originalRequest)
            );
            serverCommunicator->sendResponse(std::move(parameterErrorResponse));
        }

        using ErrorSource::linkConsumer;

    private:

        using ResponseAssembler = Assemble::ServerResponseAssembler;

        template<typename AttemptedAction, typename Request>
        void writeAndSendErrorMessageResponse(Request&& request) {
            auto errorMessageResponse =
                    ResponseAssembler::createErrorMessageResponseFromRequest<AttemptedAction>(request);
            serverCommunicator->sendResponse(std::move(errorMessageResponse));
        }

        // TODO : test this function
        template<typename AttemptedAction, typename Request>
        void writeAndSendSuccessMessageResponse(Request&& request) {
            auto successMessageResponse =
                    ResponseAssembler::createSuccessMessageResponseFromRequest<AttemptedAction>(request);
            serverCommunicator->sendResponse(std::move(successMessageResponse));
        }

        template<typename Name>
        constexpr auto getCallBack() const noexcept -> decltype(ParameterRequestCallBackStore::getInstanceNamed<Name>(
                std::declval<ParameterRequestCallBacks&>())) {
            ParameterRequestCallBacks& referenceToCallBacks = *parameterRequestCallBacks;
            return ParameterRequestCallBackStore::getInstanceNamed<Name>(referenceToCallBacks);
        }

        ServerCommunicator* serverCommunicator;
        ParameterRequestCallBacks* parameterRequestCallBacks;

        AvailableParameters parameters;
    };
}

#endif //SENSORGATEWAY_REQUESTHANDLER_HPP
