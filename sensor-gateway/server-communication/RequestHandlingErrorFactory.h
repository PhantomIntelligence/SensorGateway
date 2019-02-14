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

#ifndef SENSORGATEWAY_REQUESTHANDLINGERRORFACTORY_H
#define SENSORGATEWAY_REQUESTHANDLINGERRORFACTORY_H

#include "sensor-gateway/common/error/SensorAccessLinkError.h"

namespace ErrorHandling {

    [[noreturn]] static void throwRequestHandlingError(GatewayErrorCode const& errorCode,
                                                       std::string const& callOrigin,
                                                       std::string const& message = Message::EMPTY_MESSAGE) {

        static std::string const REQUEST_HANDLING_NAME = "Request Handling";

        std::string origin = REQUEST_HANDLING_NAME + Message::SEPARATOR + callOrigin;
        Category category = COMMUNICATION_ERROR;
        Severity severity;
        std::string errorMessage;
        if (errorCode == BAD_REQUEST) {
            severity = ERROR;
        } else if (errorCode == REQUEST_NOT_SUPPORTED) {
            severity = ERROR;
        } else if (errorCode == INVALID_PARAMETER_NAME) {
            severity = WARNING;
        } else if (errorCode == INVALID_PARAMETER_VALUE) {
            severity = INFO;
        } else {
            category = UNRECOGNIZED_ERROR;
            severity = EMERGENCY;
            errorMessage = "Error unknown for " + REQUEST_HANDLING_NAME;
        }
        throw SensorAccessLinkError(origin, category, severity, errorCode, errorMessage);
    }

}
#endif //SENSORGATEWAY_REQUESTHANDLINGERRORFACTORY_H
