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

namespace Assemble {
    namespace ServerRequest {

        template<class AvailableParameters>
        auto getParameterValue(std::string const& parameterName) {
            AvailableParameters availableParameters;
            auto parameterAvailable = availableParameters.isAvailable(parameterName);
            if(!parameterAvailable) {
                return false;
            }
            return true;
        }

    }
}


#endif //SENSORGATEWAY_SENSORREQUESTASSEMBLER_HPP
