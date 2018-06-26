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
#ifndef SPIRITSENSORGATEWAY_SYSTEMERRORMESSAGES_H
#define SPIRITSENSORGATEWAY_SYSTEMERRORMESSAGES_H

#include <string>

namespace AWLSystemErrorMessages {

    namespace AWLFPGAError {

        std::string const RECEIVE_FIFO_OVERFLOW = "Receive FIFO Overflow";
        std::string const RECEIVE_FIFO_UNDERFLOW = "Receive FIFO Underflow";
        std::string const SYSTEM_RESET = "System Reset";

    }

    namespace  AWLDeviceDriverError {

        std::string const MISPLACED_TIMESTAMP = "Misplaced Timestamp";
        std::string const MISSING_TIMESTAMP = "Missing Timestamp";
        std::string const TIMESTAMP_MISMATCH = "Timestamp Mismatch";

    }

    namespace AWLDaemonError {

        std::string const DEVICE_OPEN_ERROR = "Device Open Error";
        std::string const DEVICE_READ_ERROR = "Device Read Error";
        std::string const DEVICE_WRITE_ERROR = "Device Write Error";
        std::string const COMMUNICATION_ERROR = "Communication Error";

    }
}

#endif //SPIRITSENSORGATEWAY_SYSTEMERRORMESSAGES_H
