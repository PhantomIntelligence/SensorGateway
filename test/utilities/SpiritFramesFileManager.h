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

#ifndef SPIRITSENSORGATEWAY_SPIRITFRAMESFILEMANAGER_H
#define SPIRITSENSORGATEWAY_SPIRITFRAMESFILEMANAGER_H

#include "FileManager.hpp"
#include "spirit-sensor-gateway/domain/Frame.h"

namespace TestUtilities {

    class SpiritFramesFileManager : public FileManager<SpiritProtocol::Frame> {

    public:
        SpiritFramesFileManager() = default;

        ~SpiritFramesFileManager() override = default;

    private:
        SpiritProtocol::Frame readMessageFromFileBlock(std::string const& fileBlock) override;

        void writeFileBlockWithMessage(SpiritProtocol::Frame message, std::FILE* file) override;

        void writeFileLineWithContentLabel(std::FILE* file, unsigned int numberOfTabulator, char const* contentLabel);

        void writeFileLineWithContentLabelAndValue(std::FILE* file, unsigned int numberOfTabulator,
                                                   char const* contentLabel, unsigned int contentValue);

        const std::string FRAME_ID_LABEL = "Frame ID";

        const std::string SYSTEM_ID_LABEL = "System ID";

        const std::string PIXELS_LABEL = "Pixels";

        const std::string PIXEL_ID_LABEL = "Pixel ID";

        const std::string TRACKS_LABEL = "Tracks";

        const std::string TRACK_ID_LABEL = "Track ID";

        const std::string INTENSITY_LABEL = "Ìntensity";

        const std::string CONFIDENCE_LEVEL_LABEL = "Confidence Level";

        const std::string DISTANCE_LABEL = "Distance";

        const std::string ACCELERATION_LABEL = "Acceleration";

        const std::string SPEED_LABEL = "Speed";
    };
}

#endif //SPIRITSENSORGATEWAY_SPIRITFRAMESFILEMANAGER_H