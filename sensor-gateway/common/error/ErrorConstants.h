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

#include "sensor-gateway/common/custom-type-and-helper/IncludeRootForDefinitions.h"

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
        std::string const SERVER_COMMUNICATOR_FETCH_GET_PARAMETER_VALUE = "fetch get parameter value content for requests";
        std::string const SERVER_COMMUNICATOR_FETCH_SET_UNSIGNED_INTEGER_PARAMETER_VALUE = "fetch set unsigned integer parameter value content for requests";
        std::string const SERVER_COMMUNICATOR_FETCH_SET_SIGNED_INTEGER_PARAMETER_VALUE = "fetch set signed integer parameter value content for requests";
        std::string const SERVER_COMMUNICATOR_FETCH_SET_REAL_NUMBER_PARAMETER_VALUE = "fetch set real number parameter value content for requests";
        std::string const SERVER_COMMUNICATOR_FETCH_SET_BOOLEAN_PARAMETER_VALUE = "fetch set boolean parameter value content for requests";
        std::string const SERVER_COMMUNICATOR_SEND_MESSAGE = "send message";
        std::string const SERVER_COMMUNICATOR_SEND_RAWDATA = "send rawdata";
        std::string const SERVER_COMMUNICATOR_SEND_RESPONSE = "send response";

        std::string const SERVER_REQUEST_HANDLING_PARAMETER = "sensor parameter value";

        std::string const SENSOR_PARAMETER_CONTROLLER_PARSE_VALUE_BEFORE_SEND_TO_REQUEST = "parse value before sending to request handler";
    }

    namespace Message {
        std::string const SEPARATOR = " - ";
        std::string const EMPTY_MESSAGE = "";

        std::string const PARAMETER_NOT_AVAILABLE = "Parameter is not available";

        std::string const PARAMETER_VALUE_TYPE_NOT_RECOGNIZED = "Parameter value type is not recognized";
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

        TIME_POINTS_ERROR = 300,

        PARAMETER_ERROR = 300,
    };

    typedef int64_t ErrorCode;

    enum GatewayErrorCode : ErrorCode {
        DATA_NOT_RECOGNIZED = 10000,


        /**
         * @brief This SHOULD NEVER be used for any application, for empty initialization purposes only
         */
                EMPTY_CODE = 10042,

        // 10100-10199 Gateway Protocol error code

        PIXEL_ALREADY_HAS_MAXIMUM_TRACK = 10110,

        TIME_POINTS_ALREADY_AT_MAXIMUM_CAPACITY = 10111,

        // 10200-10299 AWL INTERNAL error code

        UNRECOGNIZED_AWL_MESSAGE_ID = 10200,

        // 10300-10399 Guardian INTERNAL error code

        UNRECOGNIZED_GUARDIAN_MESSAGE_ID = 10300,
        CANNOT_FIND_GUARDIAN_LIBUSB_DEVICE = 10305,


        // 20000-20099 Server Communication error code


        // 20100-20199 Request Handling error code

        BAD_REQUEST = 20100,
        REQUEST_NOT_SUPPORTED = 20101,

        INVALID_PARAMETER_NAME = 20110,

        INVALID_PARAMETER_VALUE = 20120,

        // 20200-20299 Request Handling error code

        INVALID_SENSOR_PARAMETER_VALUE_TYPE = 20200,

    };
};
#endif //SENSORGATEWAY_ERRORCONSTANTS_H
