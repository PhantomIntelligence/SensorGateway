/**
	Copyright 2014-2018 Phantom Intelligence Inc.

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

#ifndef SENSORGATEWAY_ERRORCONSTANTS_H
#define SENSORGATEWAY_ERRORCONSTANTS_H

#include "sensor-gateway/common/ConstantFunctionsDefinition.h"

namespace ErrorHandling {

    enum Severity : int32_t {
        EMERGENCY = 0,
        ALERT = 1,
        CRITICAL = 2,
        ERROR = 3,
        WARNING = 4,
        NOTICE = 5,
        INFO = 6,
        DEBUG = 7,

        /**
         * @brief This SHOULD NEVER be used for any application, for empty initialization purposes only
         */
        EMPTY = 42,
    };

    enum Category : int32_t {
        CONNECTION_ERROR = 100,
        COMMUNICATION_ERROR = 101,
        UNHANDLED_ERROR = 102,
        EXTERNAL_ERROR = 103,
        UNRECOGNIZED_ERROR = 104,

        /**
         * @brief This SHOULD NEVER be used for any application, for empty initialization purposes only
         */
        EMPTY_ERROR = 142,
    };

}

#endif //SENSORGATEWAY_ERRORCONSTANTS_H
