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

#include <fstream>
#include "SensorMessagesFileManager.h"

using TestUtilities::SensorMessagesFileManager;

std::vector<AWLMessage> SensorMessagesFileManager::parseSensorMessagesFromFile(std::string const& filename) {
    std::vector<AWLMessage> messages;
    auto file = std::ifstream(filename);
    if (file) {
        std::string fileLine;
        while (std::getline(file, fileLine)) {
            AWLMessage message = parseSensorMessageFromLine(fileLine);
            messages.push_back(message);
        }
    } else {
        std::cerr << "An error occurred while opening the file.\n";
    }
    return messages;
}

void SensorMessagesFileManager::buildFileFromSensorMessages(std::vector<AWLMessage> messages, std::string const& filename) {
    std::ofstream file;
    file.open(filename);
    for (auto message : messages){
        file << buildLineFromSensorMessage(message) << std::endl; 
    }
    file.close();
}



