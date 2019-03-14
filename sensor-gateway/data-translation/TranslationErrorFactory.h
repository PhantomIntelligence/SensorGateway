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

#ifndef SENSORGATEWAY_TRANSLATIONERRORFACTORY_H
#define SENSORGATEWAY_TRANSLATIONERRORFACTORY_H

#include "sensor-gateway/common/error/SensorAccessLinkError.h"

namespace ErrorHandling {

    namespace _internal {

        [[noreturn]] static void
        throwTranslationError(std::string const& origin, ErrorCode const& code, std::string const& message) {
            throw SensorAccessLinkError(origin,
                                        Category::TRANSLATION_ERROR,
                                        Severity::ERROR,
                                        code,
                                        message);
        }
    }

    std::string const CALL_SEPARATOR = "::";
    std::string const AWL_STRATEGY_NAME = "AWLTranslationStrategy";
    std::string const GUARDIAN_STRATEGY_NAME = "GuardianTranslationStrategy";

    std::string const UNRECOGNIZED_ID = "Unrecognized id";

    [[noreturn]] static void
    throwAWLTranslationError(std::string const& callOrigin, ErrorCode const& errorCode) {
        std::string source = AWL_STRATEGY_NAME + CALL_SEPARATOR + callOrigin;
        std::string message;
        if(errorCode == GatewayErrorCode::UNRECOGNIZED_AWL_MESSAGE_ID) {
            message = UNRECOGNIZED_ID;
        } else {
            message = "Unknown error code for " + AWL_STRATEGY_NAME ;
        }
        _internal::throwTranslationError(source, errorCode, message);
    }

    [[noreturn]] static void
    throwGuardianTranslationError(std::string const& callOrigin, ErrorCode const& errorCode) {
        std::string source = GUARDIAN_STRATEGY_NAME + CALL_SEPARATOR + callOrigin;
        std::string message;
        if(errorCode == GatewayErrorCode::UNRECOGNIZED_GUARDIAN_MESSAGE_ID) {
            message = UNRECOGNIZED_ID;
        } else {
            message = "Unknown error code for " + GUARDIAN_STRATEGY_NAME ;
        }
        _internal::throwTranslationError(source, errorCode, message);
    }

}
#endif //SENSORGATEWAY_TRANSLATIONERRORFACTORY_H
