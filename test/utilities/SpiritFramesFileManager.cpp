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

#include "spirit-sensor-gateway/common/TypeDefinition.h"
#include "SpiritFramesFileManager.h"

using TestUtilities::SpiritFramesFileManager;

void SpiritFramesFileManager::buildFileFromSpiritFrames(std::vector<SpiritProtocol::Frame> frames,
                                                        char const* filename) {
    auto file = std::fopen(filename, "w+");
    for (SpiritProtocol::Frame const& frame : frames){
        std::fprintf(file, "======================================================================================\n");
        std::fprintf(file, "Frame : %d \n", frame.getFrameID() );
        std::fprintf(file, "System : %d \n", frame.getSystemID());

        std::unordered_map<SpiritProtocol::PixelID, SpiritProtocol::Pixel> pixels = frame.getPixels();
        for (auto pixel : pixels) {
            std::fprintf(file, "Pixel %d : \n", pixel.second.getID());

            std::unordered_map<SpiritProtocol::TrackID, SpiritProtocol::Track> tracks = pixel.second.getTracks();
            for (auto track : tracks) {
                std::fprintf(file, ".. Track : %d \n",  track.second.getID() );
                std::fprintf(file, "..... Acceleration : %d \n", track.second.getAcceleration());
                //TODO: remove comment line once getDistance return the right value
                //std::fprintf(file, "..... Distance : %d \n", track.second.getDistance());
                std::fprintf(file, "..... Intensity : %d \n", track.second.getIntensity());
                std::fprintf(file, "..... Confidence Level : %d \n", track.second.getConfidenceLevel());
                //TODO: remove comment line once getSpeed return the right value
                //std::fprintf(file, "..... Speed : %d \n", track.second.getSpeed() );
            }
        }
    }
    fflush(file);
    fclose(file);
}
