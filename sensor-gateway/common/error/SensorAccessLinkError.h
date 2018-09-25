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

#ifndef SENSORGATEWAY_SENSORACCESSLINKERROR_H
#define SENSORGATEWAY_SENSORACCESSLINKERROR_H

#include "ErrorConstants.h"

namespace ErrorHandling {

    class SensorAccessLinkError : public std::runtime_error {
    protected:

    public:

        typedef int64_t ErrorCode;

        explicit SensorAccessLinkError(std::string const& origin,
                                       Category const& category,
                                       Severity const& severity,
                                       ErrorCode const& code,
                                       std::string const& message,
                                       HighResolutionTimePoint timestamp = HighResolutionClock::now());

        ~SensorAccessLinkError() noexcept override;

        SensorAccessLinkError(SensorAccessLinkError const& other) noexcept;

        SensorAccessLinkError(SensorAccessLinkError&& other) noexcept;

        SensorAccessLinkError& operator=(SensorAccessLinkError const& other)&;

        SensorAccessLinkError& operator=(SensorAccessLinkError&& other)& noexcept;

        void swap(SensorAccessLinkError& current, SensorAccessLinkError& other) noexcept;

        bool operator==(SensorAccessLinkError const& other) const;

        bool operator!=(SensorAccessLinkError const& other) const;

        SensorAccessLinkError static const returnDefaultData() noexcept;

        bool isFatal() const noexcept;

        bool isOpenConnectionRequired() const noexcept;

        bool isCloseConnectionRequired() const noexcept;

        std::string fetchDetailedMessage() const noexcept;


    private:

        static std::string buildDetailedMessage(HighResolutionTimePoint const& timestamp,
                                                std::string const& origin,
                                                Category const& category,
                                                Severity const& severity,
                                                ErrorCode const& code,
                                                std::string const& message) noexcept;

        static std::string formatTimestamp(HighResolutionTimePoint const& timestamp) noexcept;

    protected:

        HighResolutionTimePoint timestamp;
        std::string origin;
        Category category;
        Severity severity;
        ErrorCode code;
        std::string message;

    };
}

namespace Defaults {
    using ErrorHandling::SensorAccessLinkError;
    using ErrorHandling::Severity;
    using ErrorHandling::Category;
    using ErrorCode = typename SensorAccessLinkError::ErrorCode;

    std::string const DEFAULT_ORIGIN = "Initialization origin, should not appear in production.";
    Category const DEFAULT_CATEGORY = Category::EMPTY_ERROR;
    Severity const DEFAULT_SEVERITY = Severity::EMPTY;
    ErrorCode const DEFAULT_ERROR_CODE = -1;
    std::string const DEFAULT_MESSAGE = "This message is a placeholder for initialization purpose.";
    SensorAccessLinkError const DEFAULT_SENSOR_ACCESS_LINK_ERROR = SensorAccessLinkError(
            DEFAULT_ORIGIN, DEFAULT_CATEGORY, DEFAULT_SEVERITY, DEFAULT_ERROR_CODE, DEFAULT_MESSAGE
    );
}

#endif //SENSORGATEWAY_SENSORACCESSLINKERROR_H
