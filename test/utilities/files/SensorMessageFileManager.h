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

#ifndef SENSORGATEWAY_SENSORMESSAGEFILEMANAGER_H
#define SENSORGATEWAY_SENSORMESSAGEFILEMANAGER_H

#include "FileManager.hpp"
#include "sensor-gateway/common/data-structure/gateway/SensorMessage.hpp"
#include "sensor-gateway/common/data-structure/sensor/AWLStructures.h"
#include "sensor-gateway/common/data-structure/gateway/GatewayStructures.h"

namespace TestUtilities {

    namespace Structures {
        static size_t const MAX_NUMBER_OF_SENSOR_MESSAGES_CURRENTLY_NEEDED_FOR_TEST = 2;
        using AWL16Structures = Sensor::AWL::Structures;
        using AWL16GatewayStructures = Sensor::Gateway::Structures<
                AWL16Structures::AWLMessageDefinition,
                AWL16Structures::AWLRawDataDefinition,
                AWL16Structures::AWLCommandDefinition
        >;
        using Message = AWL16GatewayStructures::Message;
        using SensorMessages = std::array<Message, MAX_NUMBER_OF_SENSOR_MESSAGES_CURRENTLY_NEEDED_FOR_TEST>;
        using SensorMessageList = std::list<Message>;
    }

    class SensorMessageFileManager : public FileManager<Structures::Message> {

    public:
        SensorMessageFileManager() = default;

        ~SensorMessageFileManager() override = default;

        void writeFileWithSensorMessages(Structures::SensorMessages sensorMessages, std::string const& filename);

        void writeFileWithSensorMessages(Structures::SensorMessageList sensorMessages, std::string const& filename);

    private:
        Structures::Message readMessageFromFileBlock(std::string const& fileBlock) override;

        void writeFileBlockWithMessage(Structures::Message message, std::FILE* file) override;

        void writeFileLineWithContentLabel(std::FILE* file, unsigned int numberOfTabulator, char const* contentLabel);

        void writeFileLineWithContentLabelAndValue(std::FILE* file, unsigned int numberOfTabulator,
                                                   char const* contentLabel, unsigned int contentValue);

        std::string const MESSAGE_ID_LABEL = "Message id";

        std::string const SENSOR_ID_LABEL = "Sensor id";

        std::string const PIXELS_LABEL = "Pixels";

        std::string const PIXEL_ID_LABEL = "Pixel id";

        std::string const TRACKS_LABEL = "Tracks";

        std::string const TRACK_ID_LABEL = "Track id";

        std::string const INTENSITY_LABEL = "Intensity";

        std::string const CONFIDENCE_LEVEL_LABEL = "Confidence Level";

        std::string const DISTANCE_LABEL = "Distance";

        std::string const ACCELERATION_LABEL = "Acceleration";

        std::string const SPEED_LABEL = "Speed";
    };
}

#endif //SENSORGATEWAY_SENSORMESSAGEFILEMANAGER_H
