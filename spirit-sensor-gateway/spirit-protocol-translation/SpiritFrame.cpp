#include "SpiritFrame.h"

SpiritFrame::SpiritFrame(){
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


