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

#ifndef SENSORGATEWAY_AWLMESSAGESFILEMANAGER_H
#define SENSORGATEWAY_AWLMESSAGESFILEMANAGER_H

#include "FileManager.hpp"
#include "sensor-gateway/common/sensor-structures/AWLMessage.h"

namespace TestUtilities {

    class AWLMessagesFileManager : public FileManager<DataFlow::AWLMessage> {

    public:
        AWLMessagesFileManager() = default;

        ~AWLMessagesFileManager() override = default;

    private:
        DataFlow::AWLMessage readMessageFromFileBlock(std::string const& fileBlock) override;

        void writeFileBlockWithMessage(DataFlow::AWLMessage message, std::FILE* file) override;

        std::string const ID_LABEL = "ID";

        std::string const LENGTH_LABEL = "Length";

        std::string const TIMESTAMP_LABEL = "Timestamp";

        std::string const DATA_LABEL = "Data";

        std::string const DATA_POSITION_LABEL = "Position";
    };
}

#endif //SENSORGATEWAY_AWLMESSAGESFILEMANAGER_H
