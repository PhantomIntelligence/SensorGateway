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

#include "SensorAccessLinkError.h"

using ErrorHandling::SensorAccessLinkError;
using ErrorHandling::Severity;
using ErrorHandling::Category;

SensorAccessLinkError::SensorAccessLinkError(std::string const& origin,
                                             Category const& category,
                                             Severity const& severity,
                                             ErrorCode const& code,
                                             std::string const& message,
                                             HighResolutionTimePoint timestamp) :
        runtime_error(buildDetailedMessage(timestamp, origin, category, severity, code, message)),
        timestamp(timestamp),
        origin(origin),
        category(category),
        severity(severity),
        code(code),
        message(message) {}

SensorAccessLinkError::~SensorAccessLinkError() noexcept {}

SensorAccessLinkError::SensorAccessLinkError(SensorAccessLinkError const& other) :
        SensorAccessLinkError(other.origin, other.category, other.severity, other.code, other.message,
                              other.timestamp) {
}

SensorAccessLinkError::SensorAccessLinkError(SensorAccessLinkError&& other) noexcept :
        SensorAccessLinkError(std::move(other.origin), other.category, other.severity, other.code,
                              std::move(other.message), std::move(other.timestamp)) {
}


void SensorAccessLinkError::swap(SensorAccessLinkError& current, SensorAccessLinkError& other) noexcept {
    std::swap(current.origin, other.origin);
    std::swap(current.category, other.category);
    std::swap(current.severity, other.severity);
    std::swap(current.code, other.code);
    std::swap(current.message, other.message);
    std::swap(current.timestamp, other.timestamp);
}

bool SensorAccessLinkError::operator==(SensorAccessLinkError const& other) const {
    auto sameOrigin = (origin == other.origin);
    auto sameCategory = (category == other.category);
    auto sameSeverity = (severity == other.severity);
    auto sameCode = (code == other.code);
    auto sameMessage = (message == other.message);
    auto messagesAreEqual = (sameOrigin &&
                             sameCategory &&
                             sameSeverity &&
                             sameCode &&
                             sameMessage
    );
    return messagesAreEqual;
}

bool SensorAccessLinkError::operator!=(SensorAccessLinkError const& other) const {
    return !operator==(other);
}

std::string SensorAccessLinkError::fetchDetailedMessage() const noexcept {
    std::string detailedMessage = buildDetailedMessage(timestamp, origin, category, severity, code, message);
    return detailedMessage;
}

std::string SensorAccessLinkError::buildDetailedMessage(HighResolutionTimePoint const& timestamp,
                                                        std::string const& origin,
                                                        Category const& category,
                                                        Severity const& severity,
                                                        SensorAccessLinkError::ErrorCode const& code,
                                                        std::string const& message) noexcept {
    auto formattedTimestamp = formatTimestamp(timestamp);
    std::ostringstream detailedMessageStream(formattedTimestamp, std::ios_base::ate);
    detailedMessageStream
            << " -- <" << severity
            << "> " << category
            << " - " << origin
            << ": " << code
            << " ->  " << message
            << "\n";
    std::string detailedMessage = detailedMessageStream.str();
    return detailedMessage;
}

std::string SensorAccessLinkError::formatTimestamp(HighResolutionTimePoint const& timestamp) noexcept {
    std::size_t const ARBITRARY_BIG_ENOUGH_SIZE = 128;
    auto time_tTimestamp = std::chrono::_V2::system_clock::to_time_t(timestamp);
    auto gmTime = gmtime(&time_tTimestamp);
    char charArrayTime[ARBITRARY_BIG_ENOUGH_SIZE];
    auto numberOfCharacterWritten = strftime(charArrayTime, sizeof(charArrayTime), "%D %T", gmTime);
    std::string formattedTime(std::begin(charArrayTime), std::begin(charArrayTime) + numberOfCharacterWritten);
    return formattedTime;
}

