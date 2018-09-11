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

#ifndef SENSORGATEWAY_FRAMEFILEMANAGER_H
#define SENSORGATEWAY_FRAMEFILEMANAGER_H

#include "FileManager.hpp"
#include "sensor-gateway/common/data-structure/spirit/Frame.h"

namespace TestUtilities {

    namespace Structures {
        static size_t const MAX_NUMBER_OF_FRAMES_CURRENTLY_NEEDED_FOR_TEST = 2;
        using Frames = std::array<DataFlow::Frame, MAX_NUMBER_OF_FRAMES_CURRENTLY_NEEDED_FOR_TEST>;
    }

    class FrameFileManager : public FileManager<DataFlow::Frame> {

    public:
        FrameFileManager() = default;

        ~FrameFileManager() override = default;

        void writeFileWithFrames(Structures::Frames frames, std::string const& filename);

    private:
        DataFlow::Frame readMessageFromFileBlock(std::string const& fileBlock) override;

        void writeFileBlockWithMessage(DataFlow::Frame message, std::FILE* file) override;

        void writeFileLineWithContentLabel(std::FILE* file, unsigned int numberOfTabulator, char const* contentLabel);

        void writeFileLineWithContentLabelAndValue(std::FILE* file, unsigned int numberOfTabulator,
                                                   char const* contentLabel, unsigned int contentValue);

        std::string const FRAME_ID_LABEL = "Frame ID";

        std::string const SYSTEM_ID_LABEL = "System ID";

        std::string const PIXELS_LABEL = "Pixels";

        std::string const PIXEL_ID_LABEL = "Pixel ID";

        std::string const TRACKS_LABEL = "Tracks";

        std::string const TRACK_ID_LABEL = "Track ID";

        std::string const INTENSITY_LABEL = "Ìntensity";

        std::string const CONFIDENCE_LEVEL_LABEL = "Confidence Level";

        std::string const DISTANCE_LABEL = "Distance";

        std::string const ACCELERATION_LABEL = "Acceleration";

        std::string const SPEED_LABEL = "Speed";
    };
}

#endif //SENSORGATEWAY_FRAMEFILEMANAGER_H
