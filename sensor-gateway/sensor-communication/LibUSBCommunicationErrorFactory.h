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
*/#ifndef SENSORGATEWAY_GUARDIANUSBCOMMUNICATIONERROR_H
#define SENSORGATEWAY_GUARDIANUSBCOMMUNICATIONERROR_H

#include "sensor-gateway/common/error/SensorAccessLinkError.h"
#include <libusb-1.0/libusb.h>

namespace ErrorHandling {
    std::string const LIBUSB_NAME = "LibUSB";
    std::string const LIBUSB_EMPTY_PREPARED_MESSAGE = "";

    [[noreturn]] static void
    throwGuardianUSBCommunicationError(enum libusb_error const& errorCode,
                                       std::string const& callOrigin,
                                       std::string const& preparedMessage = LIBUSB_EMPTY_PREPARED_MESSAGE) {
        auto messageHasBeenPrepared = preparedMessage != LIBUSB_EMPTY_PREPARED_MESSAGE;
        std::string message;
        if (messageHasBeenPrepared) {
            message = libusb_strerror(errorCode);
        } else {
            message = preparedMessage;
        }

        std::string origin = LIBUSB_NAME + " " + callOrigin;
        ErrorHandling::Category category;
        ErrorHandling::Severity severity;
        std::string errorMessage;
        if (errorCode == LIBUSB_ERROR_IO) {
            category = COMMUNICATION_ERROR;
            severity = ERROR;
        } else if (errorCode == LIBUSB_ERROR_INVALID_PARAM) {
            category = COMMUNICATION_ERROR;
            severity = ERROR;
        } else if (errorCode == LIBUSB_ERROR_ACCESS) {
            category = CONNECTION_ERROR;
            severity = CRITICAL;
        } else if (errorCode == LIBUSB_ERROR_NO_DEVICE) {
            category = CONNECTION_ERROR;
            severity = EMERGENCY;
        } else if (errorCode == LIBUSB_ERROR_NOT_FOUND) {
            category = CONNECTION_ERROR;
            severity = ALERT;
        } else if (errorCode == LIBUSB_ERROR_BUSY) {
            category = CONNECTION_ERROR;
            severity = ALERT;
        } else if (errorCode == LIBUSB_ERROR_TIMEOUT) {
            category = COMMUNICATION_ERROR;
            severity = ERROR;
        } else if (errorCode == LIBUSB_ERROR_OVERFLOW) {
            category = COMMUNICATION_ERROR;
            severity = ERROR;
        } else if (errorCode == LIBUSB_ERROR_PIPE) {
            category = CONNECTION_ERROR;
            severity = ERROR;
        } else if (errorCode == LIBUSB_ERROR_INTERRUPTED) {
            category = CONNECTION_ERROR;
            severity = ERROR;
        } else if (errorCode == LIBUSB_ERROR_NO_MEM) {
            category = EXTERNAL_ERROR;
            severity = ERROR;
        } else if (errorCode == LIBUSB_ERROR_NOT_SUPPORTED) {
            category = UNHANDLED_ERROR;
            severity = EMERGENCY;
        } else if (errorCode == LIBUSB_ERROR_OTHER) {
            category = UNRECOGNIZED_ERROR;
            severity = ERROR;
        } else {
            category = UNRECOGNIZED_ERROR;
            severity = CRITICAL;
            errorMessage = "Error unknown for " + LIBUSB_NAME;
        }
        SensorAccessLinkError error(origin, category, severity, errorCode, errorMessage);
        throw error;
    }

    [[noreturn]] static void throwLibUSBDeviceNotFound() {
        throw ErrorHandling::SensorAccessLinkError(LIBUSB_NAME,
                                                   ErrorHandling::Category::CONNECTION_ERROR,
                                                   ErrorHandling::Severity::ERROR,
                                                   ErrorHandling::GatewayErrorCode::CANNOT_FIND_GUARDIAN_LIBUSB_DEVICE,
                                                   "Guardian device not found!");
    }
}
#endif //SENSORGATEWAY_KVASERCOMMUNICATIONERROR_H
