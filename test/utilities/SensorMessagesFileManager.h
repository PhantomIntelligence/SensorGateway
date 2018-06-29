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

#ifndef SPIRITSENSORGATEWAY_FILEPARSER_H
#define SPIRITSENSORGATEWAY_FILEPARSER_H

#include "spirit-sensor-gateway/domain/AWLMessage.h"

namespace TestUtilities {

    class SensorMessagesFileManager {

    public:
        SensorMessagesFileManager() = default;

        virtual ~SensorMessagesFileManager() = default;

        //TODO: change AWLMessage to SensorMessage once this class exists
        void buildFileFromSensorMessages(std::vector<AWLMessage> messages, std::string const& filename);

        std::vector<AWLMessage> parseSensorMessagesFromFile(std::string const& filename);

    protected:
        virtual std::string buildLineFromSensorMessage(AWLMessage const& message) = 0;

        virtual AWLMessage parseSensorMessageFromLine(std::string const& line) = 0;
    };
}

#endif //SPIRITSENSORGATEWAY_FILEPARSER_H
