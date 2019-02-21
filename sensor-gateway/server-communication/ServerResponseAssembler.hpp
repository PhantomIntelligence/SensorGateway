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
    namespace ServerResponse {

        using ServerCommunication::ResponseType::ParameterErrorResponse;
        using ServerCommunication::ResponseType::ErrorMessageResponse;
        using ServerCommunication::PayloadTypes::ParameterErrorPayload;
        using ServerCommunication::PayloadTypes::ErrorPayload;
        using ServerCommunication::PayloadTypes::MessagePayload;


        template<typename Request>
        static auto createErrorResponseFromBadRequest(Request&& request) -> ErrorMessageResponse {
            using ServerCommunication::ResponseMessage::BadRequest;
            ErrorPayload responseBadRequestPayload(BadRequest::toString());
            ServerCommunication::PayloadTypes::ErrorPayload badRequestPayload(BadRequest::toString());
            ErrorMessageResponse response(responseBadRequestPayload, badRequestPayload);
            return response;
        }

        namespace Details {

            template<typename Data>
            static auto createParameterErrorPayload(Data const& data) -> ParameterErrorPayload {
                // TODO : change this for eventual actual response from sensor
                ErrorPayload errorMessage("...");

                ParameterErrorPayload parameterErrorPayload(
                        std::make_tuple(data.name, errorMessage, data.unit)
                );
                return parameterErrorPayload;
            };
        }

        template<typename Data, typename Request>
        static auto createParameterErrorResponse(Data const& data, Request&& request) -> ParameterErrorResponse {
            using ServerCommunication::ResponseMessage::ParameterError;
            ParameterErrorPayload const parameterErrorPayload = Details::createParameterErrorPayload(data);
            ErrorPayload const errorMessage(ParameterError::toString());
            ParameterErrorResponse response(parameterErrorPayload, errorMessage);
            return response;
        }

    }
}


#endif //SENSORGATEWAY_SENSORRESPONSEASSEMBLER_HPP
