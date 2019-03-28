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
*/#ifndef SENSORGATEWAY_KVASERCOMMUNICATIONERROR_H
#define SENSORGATEWAY_KVASERCOMMUNICATIONERROR_H

#include "sensor-gateway/common/error/SensorAccessLinkError.h"
#include <canstat.h>

namespace ErrorHandling {
    std::string const canERR_PARAM_MESSAGE = "Error in one or more parameters";
    std::string const canERR_NOMSG_MESSAGE = "There were no messages to read";
    std::string const canERR_NOTFOUND_MESSAGE = "Specified device or channel not found";
    std::string const canERR_NOMEM_MESSAGE = "Out of memory";
    std::string const canERR_NOCHANNELS_MESSAGE = "No channels available";
    std::string const canERR_INTERRUPTED_MESSAGE = "Interrupted by signals";
    std::string const canERR_TIMEOUT_MESSAGE = "Timeout occurred";
    std::string const canERR_NOTINITIALIZED_MESSAGE = "The library is not initialized";
    std::string const canERR_NOHANDLES_MESSAGE = "Out of handles";
    std::string const canERR_INVHANDLE_MESSAGE = "Handle is invalid";
    std::string const canERR_INIFILE_MESSAGE = "Error in the ini-file (16-bit only)";
    std::string const canERR_DRIVER_MESSAGE = "Driver type not supported";
    std::string const canERR_TXBUFOFL_MESSAGE = "Transmit buffer overflow";
    std::string const canERR_RESERVED_1_MESSAGE = "Reserved";
    std::string const canERR_HARDWARE_MESSAGE = "A hardware error has occurred";
    std::string const canERR_DYNALOAD_MESSAGE = "A driver DLL can't be found or loaded";
    std::string const canERR_DYNALIB_MESSAGE = "A DLL seems to have wrong version";
    std::string const canERR_DYNAINIT_MESSAGE = "Error when initializing a DLL";
    std::string const canERR_NOT_SUPPORTED_MESSAGE = "Operation not supported by hardware or firmware";
    std::string const canERR_RESERVED_5_MESSAGE = "Reserved";
    std::string const canERR_RESERVED_6_MESSAGE = "Reserved";
    std::string const canERR_RESERVED_2_MESSAGE = "Reserved";
    std::string const canERR_DRIVERLOAD_MESSAGE = "Can't find or load kernel driver";
    std::string const canERR_DRIVERFAILED_MESSAGE = "DeviceIOControl failed";
    std::string const canERR_NOCONFIGMGR_MESSAGE = "Can't find required config s/w (e.g. CS/SS)";
    std::string const canERR_NOCARD_MESSAGE = "The card was removed or not inserted";
    std::string const canERR_RESERVED_7_MESSAGE = "Reserved";
    std::string const canERR_REGISTRY_MESSAGE = "Error (missing data) in the Registry";
    std::string const canERR_LICENSE_MESSAGE = "The license is not valid.";
    std::string const canERR_INTERNAL_MESSAGE = "Internal error in the driver";
    std::string const canERR_NO_ACCESS_MESSAGE = "Access denied";
    std::string const canERR_NOT_IMPLEMENTED_MESSAGE = "Not implemented";
    std::string const canERR_DEVICE_FILE_MESSAGE = "Device File error";
    std::string const canERR_HOST_FILE_MESSAGE = "Host File error";
    std::string const canERR_DISK_MESSAGE = "Disk error";
    std::string const canERR_CRC_MESSAGE = "CRC error";
    std::string const canERR_CONFIG_MESSAGE = "Configuration Error";
    std::string const canERR_MEMO_FAIL_MESSAGE = "Memo Error";
    std::string const canERR_SCRIPT_FAIL_MESSAGE = "Script Fail";
    std::string const canERR_SCRIPT_WRONG_VERSION_MESSAGE = "The t script version doesn't match the version(s) that the device firmware supports";
    std::string const canERR_SCRIPT_TXE_CONTAINER_VERSION_MESSAGE = "The compiled t script container file format is of a version which is not supported by this version of canlib";
    std::string const canERR_SCRIPT_TXE_CONTAINER_FORMAT_MESSAGE = "An error occurred while trying to parse the compiled t script file";
    std::string const canERR_BUFFER_TOO_SMALL_MESSAGE = "The buffer provided was not large enough to contain the requested data";
    std::string const canERR__RESERVED_MESSAGE = "The last entry - a dummy so we know where NOT to place a comma";

    [[noreturn]] static void
    throwKvaserCommunicationError(ErrorHandling::ErrorCode const& errorCode, std::string const& callOrigin) {
        std::string origin = "Kvaser " + callOrigin;
        ErrorHandling::Category category;
        ErrorHandling::Severity severity;
        std::string errorMessage = Message::EMPTY_MESSAGE;
        if (errorCode == canERR_PARAM) {
            category = COMMUNICATION_ERROR;
            severity = ERROR;
            errorMessage = canERR_PARAM_MESSAGE;
        } else if (errorCode == canERR_NOMSG) {
            category = COMMUNICATION_ERROR;
            severity = INFO;
            errorMessage = canERR_NOMSG_MESSAGE;
        } else if (errorCode == canERR_NOTFOUND) {
            category = CONNECTION_ERROR;
            severity = CRITICAL;
            errorMessage = canERR_NOTFOUND_MESSAGE;
        } else if (errorCode == canERR_NOMEM) {
            category = COMMUNICATION_ERROR;
            severity = ERROR;
            errorMessage = canERR_NOMEM_MESSAGE;
        } else if (errorCode == canERR_NOCHANNELS) {
            category = CONNECTION_ERROR;
            severity = ERROR;
            errorMessage = canERR_NOCHANNELS_MESSAGE;
        } else if (errorCode == canERR_INTERRUPTED) {
            category = COMMUNICATION_ERROR;
            severity = WARNING;
            errorMessage = canERR_INTERRUPTED_MESSAGE;
        } else if (errorCode == canERR_TIMEOUT) {
            category = COMMUNICATION_ERROR;
            severity = INFO;
            errorMessage = canERR_TIMEOUT_MESSAGE;
        } else if (errorCode == canERR_NOTINITIALIZED) {
            category = CONNECTION_ERROR;
            severity = WARNING;
            errorMessage = canERR_NOTINITIALIZED_MESSAGE;
        } else if (errorCode == canERR_NOHANDLES) {
            category = UNHANDLED_ERROR;
            severity = CRITICAL;
            errorMessage = canERR_NOHANDLES_MESSAGE;
        } else if (errorCode == canERR_INVHANDLE) {
            category = UNHANDLED_ERROR;
            severity = CRITICAL;
            errorMessage = canERR_INVHANDLE_MESSAGE;
        } else if (errorCode == canERR_INIFILE) {
            category = COMMUNICATION_ERROR;
            severity = CRITICAL;
            errorMessage = canERR_INIFILE_MESSAGE;
        } else if (errorCode == canERR_DRIVER) {
            category = EXTERNAL_ERROR;
            severity = EMERGENCY;
            errorMessage = canERR_DRIVER_MESSAGE;
        } else if (errorCode == canERR_TXBUFOFL) {
            category = COMMUNICATION_ERROR;
            severity = ERROR;
            errorMessage = canERR_TXBUFOFL_MESSAGE;
        } else if (errorCode == canERR_RESERVED_1) {
            category = UNRECOGNIZED_ERROR;
            severity = WARNING;
            errorMessage = canERR_RESERVED_1_MESSAGE;
        } else if (errorCode == canERR_HARDWARE) {
            category = EXTERNAL_ERROR;
            severity = CRITICAL;
            errorMessage = canERR_HARDWARE_MESSAGE;
        } else if (errorCode == canERR_DYNALOAD) {
            category = EXTERNAL_ERROR;
            severity = EMERGENCY;
            errorMessage = canERR_DYNALOAD_MESSAGE;
        } else if (errorCode == canERR_DYNALIB) {
            category = EXTERNAL_ERROR;
            severity = EMERGENCY;
            errorMessage = canERR_DYNALIB_MESSAGE;
        } else if (errorCode == canERR_DYNAINIT) {
            category = EXTERNAL_ERROR;
            severity = CRITICAL;
            errorMessage = canERR_DYNAINIT_MESSAGE;
        } else if (errorCode == canERR_NOT_SUPPORTED) {
            category = EXTERNAL_ERROR;
            severity = CRITICAL;
            errorMessage = canERR_NOT_SUPPORTED_MESSAGE;
        } else if (errorCode == canERR_RESERVED_5) {
            category = UNRECOGNIZED_ERROR;
            severity = WARNING;
            errorMessage = canERR_RESERVED_5_MESSAGE;
        } else if (errorCode == canERR_RESERVED_6) {
            category = UNRECOGNIZED_ERROR;
            severity = WARNING;
            errorMessage = canERR_RESERVED_6_MESSAGE;
        } else if (errorCode == canERR_RESERVED_2) {
            category = UNRECOGNIZED_ERROR;
            severity = WARNING;
            errorMessage = canERR_RESERVED_2_MESSAGE;
        } else if (errorCode == canERR_DRIVERLOAD) {
            category = EXTERNAL_ERROR;
            severity = EMERGENCY;
            errorMessage = canERR_DRIVERLOAD_MESSAGE;
        } else if (errorCode == canERR_DRIVERFAILED) {
            category = EXTERNAL_ERROR;
            severity = CRITICAL;
            errorMessage = canERR_DRIVERFAILED_MESSAGE;
        } else if (errorCode == canERR_NOCONFIGMGR) {
            category = EXTERNAL_ERROR;
            severity = CRITICAL;
            errorMessage = canERR_NOCONFIGMGR_MESSAGE;
        } else if (errorCode == canERR_NOCARD) {
            category = EXTERNAL_ERROR;
            severity = WARNING;
            errorMessage = canERR_NOCARD_MESSAGE;
        } else if (errorCode == canERR_RESERVED_7) {
            category = UNRECOGNIZED_ERROR;
            severity = WARNING;
            errorMessage = canERR_RESERVED_7_MESSAGE;
        } else if (errorCode == canERR_REGISTRY) {
            category = EXTERNAL_ERROR;
            severity = ERROR;
            errorMessage = canERR_REGISTRY_MESSAGE;
        } else if (errorCode == canERR_LICENSE) {
            category = EXTERNAL_ERROR;
            severity = WARNING;
            errorMessage = canERR_LICENSE_MESSAGE;
        } else if (errorCode == canERR_INTERNAL) {
            category = EXTERNAL_ERROR;
            severity = EMERGENCY;
            errorMessage = canERR_INTERNAL_MESSAGE;
        } else if (errorCode == canERR_NO_ACCESS) {
            category = CONNECTION_ERROR;
            severity = ERROR;
            errorMessage = canERR_NO_ACCESS_MESSAGE;
        } else if (errorCode == canERR_NOT_IMPLEMENTED) {
            category = EXTERNAL_ERROR;
            severity = ERROR;
            errorMessage = canERR_NOT_IMPLEMENTED_MESSAGE;
        } else if (errorCode == canERR_DEVICE_FILE) {
            category = EXTERNAL_ERROR;
            severity = ERROR;
            errorMessage = canERR_DEVICE_FILE_MESSAGE;
        } else if (errorCode == canERR_HOST_FILE) {
            category = EXTERNAL_ERROR;
            severity = ERROR;
            errorMessage = canERR_HOST_FILE_MESSAGE;
        } else if (errorCode == canERR_DISK) {
            category = UNHANDLED_ERROR;
            severity = ERROR;
            errorMessage = canERR_DISK_MESSAGE;
        } else if (errorCode == canERR_CRC) {
            category = COMMUNICATION_ERROR;
            severity = CRITICAL;
            errorMessage = canERR_CRC_MESSAGE;
        } else if (errorCode == canERR_CONFIG) {
            category = UNHANDLED_ERROR;
            severity = CRITICAL;
            errorMessage = canERR_CONFIG_MESSAGE;
        } else if (errorCode == canERR_MEMO_FAIL) {
            category = UNHANDLED_ERROR;
            severity = ERROR;
            errorMessage = canERR_MEMO_FAIL_MESSAGE;
        } else if (errorCode == canERR_SCRIPT_FAIL) {
            category = EXTERNAL_ERROR;
            severity = EMERGENCY;
            errorMessage = canERR_SCRIPT_FAIL_MESSAGE;
        } else if (errorCode == canERR_SCRIPT_WRONG_VERSION) {
            category = EXTERNAL_ERROR;
            severity = WARNING;
            errorMessage = canERR_SCRIPT_WRONG_VERSION_MESSAGE;
        } else if (errorCode == canERR_SCRIPT_TXE_CONTAINER_VERSION) {
            category = EXTERNAL_ERROR;
            severity = EMERGENCY;
            errorMessage =
                    canERR_SCRIPT_TXE_CONTAINER_VERSION_MESSAGE;
        } else if (errorCode == canERR_SCRIPT_TXE_CONTAINER_FORMAT) {
            category = EXTERNAL_ERROR;
            severity = ERROR;
            errorMessage =
                    canERR_SCRIPT_TXE_CONTAINER_FORMAT_MESSAGE;
        } else if (errorCode == canERR_BUFFER_TOO_SMALL) {
            category = UNHANDLED_ERROR;
            severity = ERROR;
            errorMessage = canERR_BUFFER_TOO_SMALL_MESSAGE;
        } else if (errorCode == canERR__RESERVED) {
            category = UNRECOGNIZED_ERROR;
            severity = WARNING;
            errorMessage = canERR__RESERVED_MESSAGE;
        } else {
            category = UNRECOGNIZED_ERROR;
            severity = CRITICAL;
            errorMessage = "Unknown error for canlib";
        }
        throw SensorAccessLinkError(origin, category, severity, errorCode, errorMessage);
    }

}
#endif //SENSORGATEWAY_KVASERCOMMUNICATIONERROR_H
