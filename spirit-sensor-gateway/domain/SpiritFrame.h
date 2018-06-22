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
#ifndef SPIRITSENSORGATEWAY_SPIRITFRAME_H
#define SPIRITSENSORGATEWAY_SPIRITFRAME_H


#include <stdint-gcc.h>
#include "SpiritPixel.h"

class SpiritFrame {
    public:
        SpiritFrame();
        ~SpiritFrame();
        void addPixel(SpiritPixel spiritPixel);
        std::vector<SpiritPixel> getPixels();
        void setFrameID(uint16_t frameID);
        uint16_t getFrameID();
        void setSystemID(uint16_t systemID);
        uint16_t getSystemID();

    private:
        uint16_t frameID;
        uint16_t systemID;
        std::vector<SpiritPixel> spiritPixels;
};

#endif //SPIRITSENSORGATEWAY_SPIRITFRAME_H
