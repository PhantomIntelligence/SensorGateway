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

#ifndef SPIRITSENSORGATEWAY_AWLMESSAGESFILEMANAGER_H
#define SPIRITSENSORGATEWAY_AWLMESSAGESFILEMANAGER_H

#include "FileManager.hpp"
#include "spirit-sensor-gateway/domain/AWLMessage.h"

namespace TestUtilities {

    class AWLMessagesFileManager : public FileManager<AWLMessage> {

    public:
        AWLMessagesFileManager() = default;

        ~AWLMessagesFileManager() override = default;

    private:
        AWLMessage readMessageFromLine(std::string const& line) override;

        void parseAWLMessageContentFromLine(std::string line, std::string* messageContentArray);

        void writeBlockWithMessage(AWLMessage const& message, std::FILE* file) override;

        const std::string LINE_SEPARATOR = "-";

        const int NUMBER_OF_DATA_IN_LINE = 11;

        const int POSITION_OF_AWL_MESSAGE_ID_IN_LINE = 0;

        const int POSITION_OF_AWL_MESSAGE_LENGTH_IN_LINE = 1;

        const int POSITION_OF_AWL_MESSAGE_TIMESTAMP_IN_LINE = 2;

        const int POSITION_OF_AWL_MESSAGE_DATA_IN_LINE = 3;

    };
}

#endif //SPIRITSENSORGATEWAY_AWLMESSAGESFILEMANAGER_H
