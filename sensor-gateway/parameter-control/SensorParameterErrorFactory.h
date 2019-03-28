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

#ifndef SENSORGATEWAY_SENSORPARAMETERERRORFACTORY_H
#define SENSORGATEWAY_SENSORPARAMETERERRORFACTORY_H

#include "sensor-gateway/common/error/SensorAccessLinkError.h"

namespace ErrorHandling {

    static SensorAccessLinkError createSensorParameterError(GatewayErrorCode const& errorCode,
                                                             std::string const& callOrigin,
                                                             std::string const& message = Message::EMPTY_MESSAGE) {

        static std::string const SENSOR_PARAMETER_NAME = "Sensor Parameter";

        std::string origin = SENSOR_PARAMETER_NAME + Message::SEPARATOR + callOrigin;
        Category category = PARAMETER_ERROR;
        Severity severity;
        std::string errorMessage = message;
        if (errorCode == INVALID_SENSOR_PARAMETER_VALUE_TYPE) {
            severity = ERROR;
        } else {
            category = UNRECOGNIZED_ERROR;
            severity = EMERGENCY;
            errorMessage = "Error unknown for " + SENSOR_PARAMETER_NAME;
        }
        return SensorAccessLinkError(origin, category, severity, errorCode, errorMessage);
    }

    [[noreturn]] static void throwSensorParameterError(GatewayErrorCode const& errorCode,
                                                       std::string const& callOrigin,
                                                       std::string const& message) {
        throw createSensorParameterError(errorCode, callOrigin, message);
    }


}
#endif //SENSORGATEWAY_SENSORPARAMETERERRORFACTORY_H
