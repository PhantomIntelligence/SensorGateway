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
#ifndef SPIRITSENSORGATEWAY_SPIRITPIXEL_H
#define SPIRITSENSORGATEWAY_SPIRITPIXEL_H

#include <vector>
#include "SpiritTrack.h"

class SpiritPixel {
    public:
        explicit SpiritPixel(uint16_t id);
        ~SpiritPixel();
        void addTrack(SpiritTrack spiritTrack);
        SpiritTrack * getTrackById(uint16_t trackID);
        uint16_t getId();
        std::vector<SpiritTrack> getTracks();

    private:
        uint16_t id;
        std::vector<SpiritTrack> spiritTracks;
};


#endif //SPIRITSENSORGATEWAY_SPIRITPIXEL_H
