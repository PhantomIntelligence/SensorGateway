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

#ifndef SPIRITSENSORGATEWAY_SPIRITPROTOCOL_H
#define SPIRITSENSORGATEWAY_SPIRITPROTOCOL_H

#include <vector>
#include <cstdint>


struct SensorTrack {
    uint64_t id;
    uint16_t distance;
    int16_t speed;
    int16_t acceleration;
    uint8_t confidenceLevel;
    uint16_t intensity;
};



struct SensorPixel {
    uint16_t id;
    std::vector<SensorTrack> trackList;
};

struct SensorFrame {
    uint16_t frameID;
    uint16_t systemID;
    std::vector<SensorPixel> pixelList;
    uint32_t errorFlag;
};

struct SpiritDebugMessage{
    uint8_t id;
    uint16_t adress;
    int32_t value;
};
#endif //SPIRITSENSORGATEWAY_SPIRITPROTOCOL_H
