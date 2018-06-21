#include "SpiritFrame.h"

SpiritFrame::SpiritFrame(){
};

SpiritFrame:: ~SpiritFrame(){
};

void SpiritFrame::addPixel(SpiritPixel spiritPixel){
    spiritPixels.push_back(spiritPixel);
}

std::vector<SpiritPixel> SpiritFrame::getPixels(){
    return spiritPixels;
};

void SpiritFrame::setFrameID(uint16_t frameID){
    this->frameID = frameID;
}

void SpiritFrame::setSystemID(uint16_t systemID){
    this->systemID = systemID;
}

uint16_t SpiritFrame::getFrameID(){
    return frameID;
};


