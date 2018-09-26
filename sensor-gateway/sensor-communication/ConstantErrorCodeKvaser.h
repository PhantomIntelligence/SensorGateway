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
    int64_t canERR_PARAM = -1;
    int64_t canERR_NOMSG = -2;
    int64_t canERR_NOTFOUND = -3;
    int64_t canERR_NOMEM = -4;
    int64_t canERR_NOCHANNELS = -5;
    int64_t canERR_INTERRUPTED = -6;
    int64_t canERR_TIMEOUT = -7;
    int64_t canERR_NOTINITIALIZED = -8;
    int64_t canERR_NOHANDLES = -9;
    int64_t canERR_INVHANDLE = -10;
    int64_t canERR_INIFILE = -11;
    int64_t canERR_DRIVER = -12;
    int64_t canERR_TXBUFOFL = -13;
    int64_t canERR_RESERVED_1 = -14;
    int64_t canERR_HARDWARE = -15;
    int64_t canERR_DYNALOAD = -16;
    int64_t canERR_DYNALIB = -17;
    int64_t canERR_DYNAINIT = -18;
    int64_t canERR_NOT_SUPPORTED = -19;
    int64_t canERR_RESERVED_5 = -20;
    int64_t canERR_RESERVED_6 = -21;
    int64_t canERR_RESERVED_2 = -22;
    int64_t canERR_DRIVERLOAD = -23;
    int64_t canERR_DRIVERFAILED = -24;
    int64_t canERR_NOCONFIGMGR = -25;
    int64_t canERR_NOCARD = -26;
    int64_t canERR_RESERVED_7 = -27;
    int64_t canERR_REGISTRY = -28;
    int64_t canERR_LICENSE = -29;
    int64_t canERR_INTERNAL = -30;
    int64_t canERR_NO_ACCESS = -31;
    int64_t canERR_NOT_IMPLEMENTED = -32;
    int64_t canERR_DEVICE_FILE = -33;
    int64_t canERR_HOST_FILE = -34;
    int64_t canERR_DISK = -35;
    int64_t canERR_CRC = -36;
    int64_t canERR_CONFIG = -37;
    int64_t canERR_MEMO_FAIL = -38;
    int64_t canERR_SCRIPT_FAIL = -39;
    int64_t canERR_SCRIPT_WRONG_VERSION = -40;
    int64_t canERR_SCRIPT_TXE_CONTAINER_VERSION = -41;
    int64_t canERR_SCRIPT_TXE_CONTAINER_FORMAT = -42;
    int64_t canERR_BUFFER_TOO_SMALL = -43;
    int64_t canERR__RESERVED = -44;


    std::string canERR_PARAM_MESSAGE = "Error in one or more parameters";
    std::string canERR_NOMSG_MESSAGE = "There were no messages to read";
    std::string canERR_NOTFOUND_MESSAGE = "Specified device or channel not found";
    std::string canERR_NOMEM_MESSAGE = "Out of memory";
    std::string canERR_NOCHANNELS_MESSAGE = "No channels available";
    std::string canERR_INTERRUPTED_MESSAGE = "Interrupted by signals";
    std::string canERR_TIMEOUT_MESSAGE = "Timeout occurred";
    std::string canERR_NOTINITIALIZED_MESSAGE = "The library is not initialized";
    std::string canERR_NOHANDLES_MESSAGE = "Out of handles";
    std::string canERR_INVHANDLE_MESSAGE = "Handle is invalid";
    std::string canERR_INIFILE_MESSAGE = "Error in the ini-file (16-bit only)";
    std::string canERR_DRIVER_MESSAGE = "Driver type not supported";
    std::string canERR_TXBUFOFL_MESSAGE = "Transmit buffer overflow";
    std::string canERR_RESERVED_1_MESSAGE = "Reserved";
    std::string canERR_HARDWARE_MESSAGE = "A hardware error has occurred";
    std::string canERR_DYNALOAD_MESSAGE = "A driver DLL can't be found or loaded";
    std::string canERR_DYNALIB_MESSAGE = "A DLL seems to have wrong version";
    std::string canERR_DYNAINIT_MESSAGE = "Error when initializing a DLL";
    std::string canERR_NOT_SUPPORTED_MESSAGE = "Operation not supported by hardware or firmware";
    std::string canERR_RESERVED_5_MESSAGE = "Reserved";
    std::string canERR_RESERVED_6_MESSAGE = "Reserved";
    std::string canERR_RESERVED_2_MESSAGE = "Reserved";
    std::string canERR_DRIVERLOAD_MESSAGE = "Can't find or load kernel driver";
    std::string canERR_DRIVERFAILED_MESSAGE = "DeviceIOControl failed";
    std::string canERR_NOCONFIGMGR_MESSAGE = "Can't find req'd config s/w (e.g. CS/SS)";
    std::string canERR_NOCARD_MESSAGE = "The card was removed or not inserted";
    std::string canERR_RESERVED_7_MESSAGE = "Reserved";
    std::string canERR_REGISTRY_MESSAGE = "Error (missing data) in the Registry";
    std::string canERR_LICENSE_MESSAGE = "The license is not valid.";
    std::string canERR_INTERNAL_MESSAGE = "Internal error in the driver";
    std::string canERR_NO_ACCESS_MESSAGE = "Access denied";
    std::string canERR_NOT_IMPLEMENTED_MESSAGE = "Not implemented";
    std::string canERR_DEVICE_FILE_MESSAGE = "Device File error";
    std::string canERR_HOST_FILE_MESSAGE = "Host File error";
    std::string canERR_DISK_MESSAGE = "Disk error";
    std::string canERR_CRC_MESSAGE = "CRC error";
    std::string canERR_CONFIG_MESSAGE = "Configuration Error";
    std::string canERR_MEMO_FAIL_MESSAGE = "Memo Error";
    std::string canERR_SCRIPT_FAIL_MESSAGE = "Script Fail";
    std::string canERR_SCRIPT_WRONG_VERSION_MESSAGE = "The t script version dosen't match the version(s) that the device firmware supports";
    std::string canERR_SCRIPT_TXE_CONTAINER_VERSION_MESSAGE = "The compiled t script container file format is of a version which is not supported by this version of canlib";
    std::string canERR_SCRIPT_TXE_CONTAINER_FORMAT_MESSAGE = "An error occured while trying to parse the compiled t script file";
    std::string canERR_BUFFER_TOO_SMALL_MESSAGE = "The buffer provided was not large enough to contain the requested data";
    std::string canERR__RESERVED_MESSAGE = "The last entry - a dummy so we know where NOT to place a comma";
}
#endif //SENSORGATEWAY_CONSTANTERRORCODEKVASER_H
