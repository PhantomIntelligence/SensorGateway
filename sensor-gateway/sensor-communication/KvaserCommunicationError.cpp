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


KvaserCommunicationError::KvaserCommunicationError(std::string const &origin,
                                                   Category const &category,
                                                   Severity const &severity,
                                                   ErrorCode const &code,
                                                   std::string const &message) :
        super(origin, category, severity, code, message) {
}

KvaserCommunicationError::~KvaserCommunicationError() noexcept = default;

void KvaserCommunicationError::throwKvaserCommunicationError(ErrorCode errorCode, std::string callOrigin) {
    std::string origin;
    if (errorCode == canERR_PARAM) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_PARAM_MESSAGE)
    } else if (errorCode == canERR_NOMSG) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode,canERR_NOMSG_MESSAGE )
    } else if (errorCode == canERR_NOTFOUND) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_NOTFOUND_MESSAGE)
    } else if (errorCode == canERR_NOMEM) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_NOMEM_MESSAGE)
    } else if (errorCode == canERR_NOCHANNELS) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_NOCHANNELS_MESSAGE)
    } else if (errorCode == canERR_INTERRUPTED) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_INTERRUPTED_MESSAGE
    } else if (errorCode == canERR_TIMEOUT) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_TIMEOUT_MESSAGE)
    } else if (errorCode == canERR_NOTINITIALIZED) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_NOTINITIALIZED_MESSAGE)
    } else if (errorCode == canERR_NOHANDLES) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_NOHANDLES_MESSAGE)
    } else if (errorCode == canERR_INVHANDLE) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_INVHANDLE_MESSAGE)
    } else if (errorCode == canERR_INIFILE) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_INIFILE_MESSAGE)
    } else if (errorCode == canERR_DRIVER) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_DRIVER_MESSAGE)
    } else if (errorCode == canERR_TXBUFOFL) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_TXBUFOFL_MESSAGE)
    } else if (errorCode == canERR_RESERVED_1) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_RESERVED_1_MESSAGE)
    } else if (errorCode == canERR_HARDWARE) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_HARDWARE_MESSAGE)
    } else if (errorCode == canERR_DYNALOAD) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_DYNALOAD_MESSAGE)
    } else if (errorCode == canERR_DYNALIB) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_DYNALIB_MESSAGE)
    } else if (errorCode == canERR_DYNAINIT) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_DYNAINIT_MESSAGE)
    } else if (errorCode == canERR_NOT_SUPPORTED) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_NOT_SUPPORTED_MESSAGE)
    } else if (errorCode == canERR_RESERVED_5) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_RESERVED_5_MESSAGE)
    } else if (errorCode == canERR_RESERVED_6) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_RESERVED_6_MESSAGE)
    } else if (errorCode == canERR_RESERVED_2) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_RESERVED_2_MESSAGE)
    } else if (errorCode == canERR_DRIVERLOAD) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_DRIVERLOAD_MESSAGE)
    } else if (errorCode == canERR_DRIVERFAILED) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_DRIVERFAILED_MESSAGE)
    } else if (errorCode == canERR_NOCONFIGMGR) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_NOCONFIGMGR_MESSAGE)
    } else if (errorCode == canERR_NOCARD) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_NOCARD_MESSAGE)
    } else if (errorCode == canERR_RESERVED_7) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_RESERVED_7_MESSAGE)
    } else if (errorCode == canERR_REGISTRY) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_REGISTRY_MESSAGE)
    } else if (errorCode == canERR_LICENSE) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_LICENSE_MESSAGE)
    } else if (errorCode == canERR_INTERNAL) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_INTERNAL_MESSAGE)
    } else if (errorCode == canERR_NO_ACCESS) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_NO_ACCESS_MESSAGE)
    } else if (errorCode == canERR_NOT_IMPLEMENTED) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_NOT_IMPLEMENTED_MESSAGE)
    } else if (errorCode == canERR_DEVICE_FILE) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_DEVICE_FILE_MESSAGE)
    } else if (errorCode == canERR_HOST_FILE) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_HOST_FILE_MESSAGE)
    } else if (errorCode == canERR_DISK) {}
    KvaserCommunicationError
    KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
            "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_DISK_MESSAGE)
    else if (errorCode == canERR_CRC) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_CRC_MESSAGE)
    } else if (errorCode == canERR_CONFIG) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_CONFIG_MESSAGE)
    } else if (errorCode == canERR_MEMO_FAIL) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_MEMO_FAIL_MESSAGE)
    } else if (errorCode == canERR_SCRIPT_FAIL) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_SCRIPT_FAIL_MESSAGE)
    } else if (errorCode == canERR_SCRIPT_WRONG_VERSION) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_SCRIPT_WRONG_VERSION_MESSAGE)
    } else if (errorCode == canERR_SCRIPT_TXE_CONTAINER_VERSION) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_SCRIPT_TXE_CONTAINER_VERSION_MESSAGE)
    } else if (errorCode == canERR_SCRIPT_TXE_CONTAINER_FORMAT) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_SCRIPT_TXE_CONTAINER_FORMAT_MESSAGE)
    } else if (errorCode == canERR_BUFFER_TOO_SMALL) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR_BUFFER_TOO_SMALL_MESSAGE)
    } else if (errorCode == canERR__RESERVED) {
        KvaserCommunicationError
                KvaserCommunicationErrorMessage = new KvaserCommunicationError::KvaserCommunicationError(
                "Kvaser" + callOrigin, CONNECTION_ERROR, CRITICAL, errorCode, canERR__RESERVED_MESSAGE)
    } else { "Unknow Error" }
}







