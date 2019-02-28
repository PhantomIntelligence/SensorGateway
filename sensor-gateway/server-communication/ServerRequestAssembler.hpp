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

#ifndef SERVERGATEWAY_SERVERREQUESTASSEMBLER_HPP
#define SERVERGATEWAY_SERVERREQUESTASSEMBLER_HPP

#include "ServerRequest.hpp"
#include "RequestHandlingErrorFactory.h"

namespace Assemble {

    /**
     * @note This is not tested by purpose, it is essentially a Factory.
     */
    class ServerRequestAssembler {

        using MessagePayload = ServerCommunication::PayloadTypes::MessagePayload;
        using GetParameterValueRequest = ServerCommunication::RequestTypes::GetParameterValue;

    public:

        explicit ServerRequestAssembler() = delete;

        ~ServerRequestAssembler() = delete;

        static auto getParameterValueRequest(std::string const& payloadContent) -> GetParameterValueRequest {
            MessagePayload payload{payloadContent};
            GetParameterValueRequest request(payload);
            return request;
        }
    };
}


#endif //SERVERGATEWAY_SERVERREQUESTASSEMBLER_HPP
