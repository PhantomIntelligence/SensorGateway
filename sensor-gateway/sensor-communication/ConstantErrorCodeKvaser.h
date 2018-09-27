/**
	Copyright 2014-2018 Phantom Intelligence Inc.

	Licensed under the Apache License; Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing; software
	distributed under the License is distributed on an "AS IS" BASIS;
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND; either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/#ifndef SENSORGATEWAY_CONSTANTERRORCODEKVASER_H
#define SENSORGATEWAY_CONSTANTERRORCODEKVASER_H

#include <cstdint>
#include <string>

namespace ErrorHandling {

    const std::string canERR_PARAM_MESSAGE = "Error in one or more parameters";
    const std::string canERR_NOMSG_MESSAGE = "There were no messages to read";
    const std::string canERR_NOTFOUND_MESSAGE = "Specified device or channel not found";
    const std::string canERR_NOMEM_MESSAGE = "Out of memory";
    const std::string canERR_NOCHANNELS_MESSAGE = "No channels available";
    const std::string canERR_INTERRUPTED_MESSAGE = "Interrupted by signals";
    const std::string canERR_TIMEOUT_MESSAGE = "Timeout occurred";
    const std::string canERR_NOTINITIALIZED_MESSAGE = "The library is not initialized";
    const std::string canERR_NOHANDLES_MESSAGE = "Out of handles";
    const std::string canERR_INVHANDLE_MESSAGE = "Handle is invalid";
    const std::string canERR_INIFILE_MESSAGE = "Error in the ini-file (16-bit only)";
    const std::string canERR_DRIVER_MESSAGE = "Driver type not supported";
    const std::string canERR_TXBUFOFL_MESSAGE = "Transmit buffer overflow";
    const std::string canERR_RESERVED_1_MESSAGE = "Reserved";
    const std::string canERR_HARDWARE_MESSAGE = "A hardware error has occurred";
    const std::string canERR_DYNALOAD_MESSAGE = "A driver DLL can't be found or loaded";
    const std::string canERR_DYNALIB_MESSAGE = "A DLL seems to have wrong version";
    const std::string canERR_DYNAINIT_MESSAGE = "Error when initializing a DLL";
    const std::string canERR_NOT_SUPPORTED_MESSAGE = "Operation not supported by hardware or firmware";
    const std::string canERR_RESERVED_5_MESSAGE = "Reserved";
    const std::string canERR_RESERVED_6_MESSAGE = "Reserved";
    const std::string canERR_RESERVED_2_MESSAGE = "Reserved";
    const std::string canERR_DRIVERLOAD_MESSAGE = "Can't find or load kernel driver";
    const std::string canERR_DRIVERFAILED_MESSAGE = "DeviceIOControl failed";
    const std::string canERR_NOCONFIGMGR_MESSAGE = "Can't find req'd config s/w (e.g. CS/SS)";
    const std::string canERR_NOCARD_MESSAGE = "The card was removed or not inserted";
    const std::string canERR_RESERVED_7_MESSAGE = "Reserved";
    const std::string canERR_REGISTRY_MESSAGE = "Error (missing data) in the Registry";
    const std::string canERR_LICENSE_MESSAGE = "The license is not valid.";
    const std::string canERR_INTERNAL_MESSAGE = "Internal error in the driver";
    const std::string canERR_NO_ACCESS_MESSAGE = "Access denied";
    const std::string canERR_NOT_IMPLEMENTED_MESSAGE = "Not implemented";
    const std::string canERR_DEVICE_FILE_MESSAGE = "Device File error";
    const std::string canERR_HOST_FILE_MESSAGE = "Host File error";
    const std::string canERR_DISK_MESSAGE = "Disk error";
    const std::string canERR_CRC_MESSAGE = "CRC error";
    const std::string canERR_CONFIG_MESSAGE = "Configuration Error";
    const std::string canERR_MEMO_FAIL_MESSAGE = "Memo Error";
    const std::string canERR_SCRIPT_FAIL_MESSAGE = "Script Fail";
    const std::string canERR_SCRIPT_WRONG_VERSION_MESSAGE = "The t script version dosen't match the version(s) that the device firmware supports";
    const std::string canERR_SCRIPT_TXE_CONTAINER_VERSION_MESSAGE = "The compiled t script container file format is of a version which is not supported by this version of canlib";
    const std::string canERR_SCRIPT_TXE_CONTAINER_FORMAT_MESSAGE = "An error occured while trying to parse the compiled t script file";
    const std::string canERR_BUFFER_TOO_SMALL_MESSAGE = "The buffer provided was not large enough to contain the requested data";
    const std::string canERR__RESERVED_MESSAGE = "The last entry - a dummy so we know where NOT to place a comma";
}
#endif //SENSORGATEWAY_CONSTANTERRORCODEKVASER_H
