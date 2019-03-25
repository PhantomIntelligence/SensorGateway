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

#ifndef SENSORGATEWAY_SENSORRESPONSEASSEMBLER_HPP
#define SENSORGATEWAY_SENSORRESPONSEASSEMBLER_HPP

#include "ServerResponse.hpp"

namespace Assemble {

    /**
     * @note This is not tested by purpose, it is essentially a Factory.
     */
    class ServerResponseAssembler {

    protected:

        using ParameterErrorResponse =  ServerCommunication::ResponseType::ParameterErrorResponse;
        using SuccessMessageResponse =  ServerCommunication::ResponseType::SuccessMessageResponse;
        using ErrorMessageResponse =  ServerCommunication::ResponseType::ErrorMessageResponse;
        using ParameterErrorPayload =  ServerCommunication::PayloadTypes::ParameterErrorPayload;
        using SuccessPayload =  ServerCommunication::PayloadTypes::SuccessPayload;
        using ErrorPayload =  ServerCommunication::PayloadTypes::ErrorPayload;
        using MessagePayload =  ServerCommunication::PayloadTypes::MessagePayload;

        template<typename T>
        using ParameterType = ServerCommunication::PayloadTypes::Details::ParameterType<T>;
        template<typename T>
        using ParameterValuePayload = ServerCommunication::PayloadTypes::Details::SimplePayload<ParameterType<T>>;
        template<typename T>
        using ParameterValueResponse = ServerCommunication::ServerResponse<ParameterValuePayload<T>, MessagePayload>;

        using ParameterMetadataType = ServerCommunication::PayloadTypes::Details::ParameterMetadataType;
        template<size_t numberOfParameters>
        using AllParameterMetadataPayload = ServerCommunication::ResponseType::Details::AllParameterMetadataPayload<numberOfParameters>;
        template<size_t numberOfParameters>
        using AllParameterMetadataResponse = ServerCommunication::ResponseType::AllParameterMetadataResponse<numberOfParameters>;

    public:

        explicit ServerResponseAssembler() = delete;

        ~ServerResponseAssembler() = delete;

        template<typename Request>
        static auto createErrorMessageResponseFromBadRequest(Request const& request,
                                                             std::string const& attemptedActionString) -> ErrorMessageResponse {
            using ServerCommunication::ResponseMessage::BadRequest;
            ErrorPayload responseBadRequestPayload(BadRequest::toString());
            ServerCommunication::PayloadTypes::ErrorPayload badRequestPayload(
                    attemptedActionString + " : " + request.payloadToString());
            ErrorMessageResponse response(responseBadRequestPayload, badRequestPayload);
            return response;
        }


        // TODO : test this function
        template<typename AttemptedAction, typename Request>
        static auto createSuccessMessageResponseFromRequest(Request const& request) -> SuccessMessageResponse {
            using ServerCommunication::ResponseMessage::RequestSuccess;
            SuccessPayload responseRequestSuccessPayload(RequestSuccess::toString());
            ServerCommunication::PayloadTypes::SuccessPayload requestSuccessPayload(AttemptedAction::toString() + " : " + request.payloadToString());
            SuccessMessageResponse response(responseRequestSuccessPayload, requestSuccessPayload);
            return response;
        }

        template<typename AttemptedAction, typename Request>
        static auto createErrorMessageResponseFromRequest(Request const& request) -> ErrorMessageResponse {
            ErrorMessageResponse errorMessageResponse;
            if (request.isBadRequest()) {
                errorMessageResponse = createErrorMessageResponseFromBadRequest(request, AttemptedAction::toString());
            }

            return errorMessageResponse;
        }

        template<typename Data, typename Request>
        static auto createParameterErrorResponse(Data const& data, Request&& request) -> ParameterErrorResponse {
            using ServerCommunication::ResponseMessage::ParameterError;
            ParameterErrorPayload const parameterErrorPayload = createParameterErrorPayload(data);
            ErrorPayload const errorMessage(request.payloadToString());
            ParameterErrorResponse response(parameterErrorPayload, errorMessage);
            return response;
        }

        template<typename Data, typename ValueType, typename Request>
        static auto createParameterValueResponse(Data const& data, ValueType const& valueType, Request&& request)
        -> ParameterValueResponse<ValueType> {
            ParameterValuePayload<ValueType> responsePayload = createParameterValuePayload(data, valueType);
            ParameterValueResponse<ValueType> response(responsePayload, request.toResponseMessagePayload());
            return response;
        }

        // TODO : test this function
        template<typename AllParameterMetadata, typename Request, size_t numberOfParameters>
        static auto createAllParameterMetadataResponse(AllParameterMetadata&& allParameterMetadata, Request&& request)
        -> AllParameterMetadataResponse<numberOfParameters> {
            AllParameterMetadataPayload<numberOfParameters> responsePayload = createAllParameterMetadataPayload<AllParameterMetadata, numberOfParameters>(
                    std::forward<AllParameterMetadata>(allParameterMetadata)
            );
            AllParameterMetadataResponse<numberOfParameters> response(responsePayload,
                                                                      request.toResponseMessagePayload());
            return response;
        }

    private:
        template<typename Data>
        static auto createParameterErrorPayload(Data const& data) -> ParameterErrorPayload {
            // TODO : change this for eventual actual parameter error message from sensor
            ErrorPayload errorMessage("...");

            ParameterErrorPayload parameterErrorPayload(
                    std::make_tuple(data.name, errorMessage, data.unit)
            );
            return parameterErrorPayload;
        }

        template<typename Data, typename T>
        static ParameterValuePayload<T> createParameterValuePayload(Data const& data, T t) {
            ParameterType<T> parameterValues = std::make_tuple(data.name, t, data.unit);
            ParameterValuePayload<T> parameterValuePayload(parameterValues);
            return parameterValuePayload;
        }

        // TODO : test this function
        template<typename AllParameterMetadata, size_t numberOfParameters>
        static AllParameterMetadataPayload<numberOfParameters> createAllParameterMetadataPayload(AllParameterMetadata&& allParameterMetadata) {
            std::array<ParameterMetadataType, numberOfParameters> allParameterMetadataPayloadContent;
            for (auto parameterMetadataIndex = 0u;
                 parameterMetadataIndex < numberOfParameters; parameterMetadataIndex++) {
                auto parameterMetadata = allParameterMetadata[parameterMetadataIndex];
                allParameterMetadataPayloadContent[parameterMetadataIndex] = std::make_tuple(parameterMetadata.name,
                                                                                             parameterMetadata.unit);
            }
            AllParameterMetadataPayload<numberOfParameters> allParameterMetadataPayload(
                    allParameterMetadataPayloadContent);
            return allParameterMetadataPayload;
        }
    };

}


#endif //SENSORGATEWAY_SENSORRESPONSEASSEMBLER_HPP
