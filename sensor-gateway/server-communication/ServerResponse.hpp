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

#ifndef SERVERGATEWAY_SERVERRESPONSE_HPP
#define SERVERGATEWAY_SERVERRESPONSE_HPP

#include "ServerRequestAssembler.hpp"

namespace ServerCommunication {

    template<typename PayloadType, typename Request, Request r>
    class ServerResponse {
        static Request const request = r;
    };

    template<class... R>
    class BulkResponse {

    public:

        static auto const RESPONSE_SIZE = sizeof...(R);

        using Responses = std::tuple<R...>;

        explicit BulkResponse() : responses(std::make_tuple(R()...)) {}

        template <class F>
        constexpr auto processResponses(F f) const {
            return apply(responses, f);
        }

    private :
        Responses responses;
    };

}


#endif //SERVERGATEWAY_SERVERRESPONSE_HPP
