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

#include "SpiritFrame.h"

SpiritFrame::SpiritFrame(){
};

SpiritFrame:: ~SpiritFrame(){
};

SpiritPixel* SpiritFrame::getPixelByID(uint16_t pixelID){
    return &spiritPixels.at(pixelID);
}

std::unordered_map<uint16_t, SpiritPixel> SpiritFrame::getPixels(){
    return spiritPixels;
};

uint16_t SpiritFrame::getFrameID(){
    return frameID;
};

uint16_t SpiritFrame::getSystemID(){
    return systemID;
};

void SpiritFrame::addPixel(SpiritPixel spiritPixel){
    spiritPixels.insert(std::make_pair(spiritPixel.getId(), spiritPixel));
}

void SpiritFrame::setFrameID(uint16_t frameID){
    this->frameID = frameID;
}

void SpiritFrame::setSystemID(uint16_t systemID){
    this->systemID = systemID;
}
