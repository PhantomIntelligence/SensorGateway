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

#ifndef SENSORGATEWAY_SENSORREQUEST_HPP
#define SENSORGATEWAY_SENSORREQUEST_HPP

#include "sensor-gateway/common/data-structure/gateway/SensorMessage.hpp"

namespace ServerCommunication {

    namespace PayloadTypes {

        template<typename N>
        class NamePayload {
            static N const name;
        };

        template<typename N, typename ValueType, ValueType v>
        struct NameValuePayload {
            static N const name;
            static ValueType const value = v;
        };
    }

    template<typename PayloadType>
    class SensorRequest {

    };

    template<class... R>
    class BulkRequest {

    public:

        static auto const REQUEST_SIZE = sizeof...(R);

        using Requests = std::tuple<R...>;

        explicit BulkRequest() : requests(std::make_tuple(R()...)) {}

        template <class F>
        constexpr auto processRequests(F f) const {
            return apply(requests, f);
        }

    private :
        Requests requests;
    };

}


#endif //SENSORGATEWAY_SENSORREQUEST_HPP
