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

#include "KvaserCommunicationError.h"

using ErrorHandling::KvaserCommunicationError;


KvaserCommunicationError::KvaserCommunicationError(std::string const& origin,
                                                   Category const& category,
                                                   Severity const& severity,
                                                   ErrorCode const& code,
                                                   std::string const& message) :
        super(origin, category, severity, code, message) {
}

KvaserCommunicationError::~KvaserCommunicationError() noexcept = default;

void KvaserCommunicationError::throwKvaserCommunicationError(ErrorCode errorCode, std::string callOrigin) {
    std::string origin;
    if (errorCode == canERR_PARAM) {

    } else if (errorCode == canERR_NOMSG) {
    } else if (errorCode == canERR_NOTFOUND) {
    } else if (errorCode == canERR_NOMEM) {}
    else if (errorCode == canERR_NOCHANNELS) {
    } else if (errorCode == canERR_INTERRUPTED) {
    } else if (errorCode == canERR_TIMEOUT) {
    } else if (errorCode == canERR_NOTINITIALIZED) {
    } else if (errorCode == canERR_NOHANDLES) {
    } else if (errorCode == canERR_INVHANDLE) {
    } else if (errorCode == canERR_INIFILE) {
    } else if (errorCode == canERR_DRIVER) {
    } else if (errorCode == canERR_TXBUFOFL) {
    } else if (errorCode == canERR_RESERVED_1) {
    } else if (errorCode == canERR_HARDWARE) {
    } else if (errorCode == canERR_DYNALOAD) {
    } else if (errorCode == canERR_DYNALIB) {
    } else if (errorCode == canERR_DYNAINIT) {
    } else if (errorCode == canERR_NOT_SUPPORTED) {
    } else if (errorCode == canERR_RESERVED_5) {
    } else if (errorCode == canERR_RESERVED_6) {
    } else if (errorCode == canERR_RESERVED_2) {
    } else if (errorCode == canERR_DRIVERLOAD) {
    } else if (errorCode == canERR_DRIVERFAILED) {
    } else if (errorCode == canERR_NOCONFIGMGR) {
    } else if (errorCode == canERR_NOCARD) {
    } else if (errorCode == canERR_RESERVED_7) {
    } else if (errorCode == canERR_REGISTRY) {
    } else if (errorCode == canERR_LICENSE) {
    } else if (errorCode == canERR_INTERNAL) {
    } else if (errorCode == canERR_NO_ACCESS) {
    } else if (errorCode == canERR_NOT_IMPLEMENTED) {
    } else if (errorCode == canERR_DEVICE_FILE) {
    } else if (errorCode == canERR_HOST_FILE) {
    } else if (errorCode == canERR_DISK) {}
    else if (errorCode == canERR_CRC) {
    } else if (errorCode == canERR_CONFIG) {
    } else if (errorCode == canERR_MEMO_FAIL) {
    } else if (errorCode == canERR_SCRIPT_FAIL) {
    } else if (errorCode == canERR_SCRIPT_WRONG_VERSION) {
    } else if (errorCode == canERR_SCRIPT_TXE_CONTAINER_VERSION) {
    } else if (errorCode == canERR_SCRIPT_TXE_CONTAINER_FORMAT) {
    } else if (errorCode == canERR_BUFFER_TOO_SMALL) {
    } else if (errorCode == canERR__RESERVED) {
    } else {}
}







