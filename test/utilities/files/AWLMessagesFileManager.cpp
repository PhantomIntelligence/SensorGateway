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

#include <cstring>
#include "AWLMessagesFileManager.h"

using TestUtilities::AWLMessagesFileManager;
using DataFlow::AWLMessage;

AWLMessage AWLMessagesFileManager::readMessageFromFileBlock(std::string const& fileBlock) {
    auto id = std::stoi(fetchSubstringBetweenDelimiters(fileBlock, ID_LABEL+MESSAGE_LABEL_VALUE_ASSOCIATOR, "\n"));
    auto length = static_cast<unsigned int>(std::stoi(fetchSubstringBetweenDelimiters(fileBlock, LENGTH_LABEL+MESSAGE_LABEL_VALUE_ASSOCIATOR, "\n")));
    auto timestamp = static_cast<unsigned int> (std::stoi(fetchSubstringBetweenDelimiters(fileBlock, TIMESTAMP_LABEL+MESSAGE_LABEL_VALUE_ASSOCIATOR, "\n")));
    AWL::DataArray data;
    for (auto dataPosition = 0; dataPosition < MAXIMUM_NUMBER_OF_DATA_IN_MESSAGE; dataPosition++) {
        auto dataValue = std::stoi(fetchSubstringBetweenDelimiters(fileBlock, DATA_POSITION_LABEL+ " " +std::to_string(dataPosition + 1) +MESSAGE_LABEL_VALUE_ASSOCIATOR, "\n"));
        data[dataPosition] = static_cast<unsigned char>(dataValue);
    }
    AWLMessage message = AWLMessage(id,timestamp,length,data);
    return message;
}

void AWLMessagesFileManager::writeFileBlockWithMessage(AWLMessage message, std::FILE* file) {
    std::fprintf(file, "%s%s%d\n", ID_LABEL.c_str(), MESSAGE_LABEL_VALUE_ASSOCIATOR.c_str(), static_cast<int>(message.id));
    std::fprintf(file, "%s%s%d\n", LENGTH_LABEL.c_str(), MESSAGE_LABEL_VALUE_ASSOCIATOR.c_str(), static_cast<int>(message.length));
    std::fprintf(file, "%s%s%ld\n", TIMESTAMP_LABEL.c_str(), MESSAGE_LABEL_VALUE_ASSOCIATOR.c_str(), static_cast<long>(message.timestamp));
    std::fprintf(file, "%s%s\n", DATA_LABEL.c_str(), MESSAGE_LABEL_VALUE_ASSOCIATOR.c_str());
    for (auto dataPosition = 0; dataPosition < MAXIMUM_NUMBER_OF_DATA_IN_MESSAGE; dataPosition++) {
        std::fprintf(file, "%s", MESSAGE_CONTENT_TABULATOR.c_str());
        std::fprintf(file, "%s %d%s%d\n", DATA_POSITION_LABEL.c_str(), dataPosition + 1, MESSAGE_LABEL_VALUE_ASSOCIATOR.c_str(),
                     static_cast<int> (message.data[dataPosition]));
    }
    std::fprintf(file, "%s\n", MESSAGES_SEPARATOR.c_str());
}


