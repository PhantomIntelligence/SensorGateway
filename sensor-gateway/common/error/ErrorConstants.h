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

    namespace Origin {
        std::string const SENSOR_COMMUNICATOR_OPEN_CONNECTION = "open sensor connection";
        std::string const SENSOR_COMMUNICATOR_CLOSE_CONNECTION = "close sensor connection";
        std::string const SENSOR_COMMUNICATOR_HANDLE_MESSAGE = "handle incoming message";
        std::string const SENSOR_COMMUNICATOR_HANDLE_RAWDATA = "handle incoming rawdata";

        std::string const TRANSLATE_MESSAGE = "translate message";
        std::string const TRANSLATE_RAWDATA = "translate rawdata";

        std::string const SERVER_COMMUNICATOR_OPEN_CONNECTION = "open server connection";
        std::string const SERVER_COMMUNICATOR_CLOSE_CONNECTION = "close server connection";
        std::string const SERVER_COMMUNICATOR_SEND_MESSAGE = "send message";
        std::string const SERVER_COMMUNICATOR_SEND_RAWDATA = "send rawdata";
    }

    namespace Message {
        std::string const SEPARATOR = " - ";
        std::string const EMPTY_MESSAGE = "";
    }

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
                EMPTY_SEVERITY = 42,
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
                EMPTY_CATEGORY = 142,

        TRANSLATION_ERROR = 200,
    };

    typedef int64_t ErrorCode;

    enum GatewayErrorCode : ErrorCode {
        DATA_NOT_RECOGNIZED = 10000,


        /**
         * @brief This SHOULD NEVER be used for any application, for empty initialization purposes only
         */
                EMPTY_CODE = 10042,

        PIXEL_ALREADY_HAS_MAXIMUM_TRACK = 10110, // 10100-10199 Spirit Protocol error code

        UNRECOGNIZED_AWL_MESSAGE_ID = 10200, // 10200-10299 AWL INTERNAL error code

        UNRECOGNIZED_GUARDIAN_MESSAGE_ID = 10300, // 10300-10399 Guardian INTERNAL error code
    };
}

#endif //SENSORGATEWAY_ERRORCONSTANTS_H
