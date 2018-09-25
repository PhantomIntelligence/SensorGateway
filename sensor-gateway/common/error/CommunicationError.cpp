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

#include "CommunicationError.h"

using ErrorHandling::CommunicationError;

CommunicationError::CommunicationError(std::string const& origin,
                                       Category const& category,
                                       Severity const& severity,
                                       ErrorCode const& code,
                                       std::string const& message) :
        super(origin, category, severity, code, message) {}

CommunicationError::~CommunicationError() noexcept = default;

CommunicationError::CommunicationError(CommunicationError const& other) :
        super(other) {
}

CommunicationError::CommunicationError(CommunicationError&& other) noexcept :
        super(std::forward<super>(other)) {
}

CommunicationError& CommunicationError::operator=(CommunicationError const& other)& {
    CommunicationError temporary(other);
    swap(*this, temporary);
    return *this;
}

CommunicationError& CommunicationError::operator=(CommunicationError&& other)& noexcept {
    swap(*this, other);
    return *this;
}

void CommunicationError::swap(CommunicationError& current, CommunicationError& other) noexcept {
    super::swap(current, other);
}

bool CommunicationError::isFatal() const noexcept {
    bool isFatal = false;
    if (severity == EMERGENCY) {
        isFatal = true;
    }
    return isFatal;
}

bool CommunicationError::isCloseConnectionRequired() const noexcept {
    bool isCloseConnectionRequired = true;
    if (category == COMMUNICATION_ERROR) {
        isCloseConnectionRequired = false;
    }
    return isCloseConnectionRequired;
}
