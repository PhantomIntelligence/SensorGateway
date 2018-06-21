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

        bool isFrameValid();

        void addPixel(SpiritPixel spiritPixel);

        bool doesPixelExist(uint16_t pixelID);

        SpiritPixel getPixelById(uint16_t pixelID);

    private:

        uint16_t frameID;

        uint16_t systemID;

        uint32_t errorFlag;

        std::vector<SpiritPixel> pixelList;

};


#endif //SPIRITSENSORGATEWAY_SPIRITFRAME_H
