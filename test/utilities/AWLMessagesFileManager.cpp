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

#include <sstream>
#include "AWLMessagesFileManager.h"

using TestUtilities::AWLMessagesFileManager;

AWLMessage AWLMessagesFileManager::parseSensorMessageFromLine(std::string const& line) {
    AWLMessage message{};
    std::string messageContent[NUMBER_OF_DATA_IN_LINE];
    parseAWLMessageContentFromLine(line, messageContent);
    message.id = stoi(messageContent[POSITION_OF_AWL_MESSAGE_ID_IN_LINE]);
    message.length = (unsigned) stoi(messageContent[POSITION_OF_AWL_MESSAGE_LENGTH_IN_LINE]);
    message.timestamp = (unsigned) stoi(messageContent[POSITION_OF_AWL_MESSAGE_TIMESTAMP_IN_LINE]);
    for (auto dataPosition = 0; dataPosition < MAX_NUMBER_OF_DATA_IN_AWL_MESSAGE; dataPosition++) {
        auto dataValue = std::stoi(messageContent[POSITION_OF_AWL_MESSAGE_DATA_IN_LINE + dataPosition]);
        message.data[dataPosition] = static_cast<unsigned char>(dataValue);
    }
    return message;
}

void AWLMessagesFileManager::parseAWLMessageContentFromLine(std::string line, std::string* messageContentArray) {
    size_t separatorPosition = 0;
    auto contentPosition = 0;
    while ((separatorPosition = line.find(LINE_SEPARATOR)) != std::string::npos) {
        std::string content = line.substr(0, separatorPosition);
        messageContentArray[contentPosition] = content;
        contentPosition++;
        line.erase(0, separatorPosition + LINE_SEPARATOR.length());
    }
    messageContentArray[contentPosition] = line;
}

std::string AWLMessagesFileManager::buildLineFromSensorMessage(AWLMessage const& message) {
    std::stringstream lineStringStream;
    lineStringStream << (int)message.id  << LINE_SEPARATOR << (int)message.length << LINE_SEPARATOR << (long)message.timestamp << LINE_SEPARATOR;
    for (auto dataPosition = 0; dataPosition < MAX_NUMBER_OF_DATA_IN_AWL_MESSAGE - 1; dataPosition++) {
        lineStringStream << (int)message.data[dataPosition] << LINE_SEPARATOR;
    }
    lineStringStream << (int)message.data[MAX_NUMBER_OF_DATA_IN_AWL_MESSAGE - 1];
    std::string line = lineStringStream.str();
    return line;
}
