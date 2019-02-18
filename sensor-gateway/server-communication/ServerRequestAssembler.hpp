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

#ifndef SENSORGATEWAY_SENSORREQUESTASSEMBLER_HPP
#define SENSORGATEWAY_SENSORREQUESTASSEMBLER_HPP

#include "ServerRequest.hpp"
#include "RequestHandlingErrorFactory.h"

namespace Assemble {
    namespace ServerRequest {

        template<class Parameters>
        static void ensureParameterIsAvailable(std::string const& parameterName) {
            Parameters parameters;
            auto parameterAvailable = parameters.isAvailable(parameterName);
            if (!parameterAvailable) {
                ErrorHandling::throwRequestHandlingError(
                        ErrorHandling::INVALID_PARAMETER_NAME,
                        ErrorHandling::Origin::SERVER_REQUEST_HANDLING_PARAMETER,
                        "Parameter is not available");
            }
        }

        /**
         * @note This is not tested by purpose, it is essentially a Factory.
         * @param parameterName
         * @return
         */
        static auto getParameterValueRequest(std::string const& payloadContent) {
            using GetParameterValueRequest = ServerCommunication::RequestTypes::GetParameterValue;
            ServerCommunication::PayloadTypes::GetParameterValuePayload payload{payloadContent};
            GetParameterValueRequest request(payload);
            return request;
        }
    }
}


#endif //SENSORGATEWAY_SENSORREQUESTASSEMBLER_HPP
