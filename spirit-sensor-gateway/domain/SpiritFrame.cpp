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

uint16_t SpiritFrame::getSystemID(){
    return systemID;
};

std::vector<std::string> SpiritFrame::getSystemErrorFlagMessages() {
    return  systemErrorFlagMessages;
};


SpiritPixel * SpiritFrame::getPixelByID(uint16_t pixelID){
    SpiritPixel * spiritPixel = NULL;
    for (int pixelNumber = 0; pixelNumber < spiritPixels.size(); pixelNumber++) {
        if (spiritPixels[pixelNumber].getId() == pixelID){
            spiritPixel = &spiritPixels[pixelNumber];
            break;
        }
    }
    return  spiritPixel;

}


