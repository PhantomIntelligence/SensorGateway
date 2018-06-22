#include "SpiritPixel.h"

SpiritPixel::SpiritPixel(uint16_t id): id(id){};

SpiritPixel:: ~SpiritPixel(){
};

uint16_t SpiritPixel::getId(){
    return id;
};

void SpiritPixel::addTrack(SpiritTrack spiritTrack){
    spiritTracks.push_back(spiritTrack);
}

std::vector<SpiritTrack> SpiritPixel::getTracks(){
    return spiritTracks;
};

SpiritTrack * SpiritPixel::getTrackById(uint16_t trackID){
    SpiritTrack * spiritTrack = NULL;
    for (int trackNumber = 0; trackNumber < spiritTracks.size(); ++trackNumber) {
        if (spiritTracks[trackNumber].getTrackID() == trackID){
           spiritTrack = &spiritTracks[trackNumber];
           break;
        }
    }
    return spiritTrack;
}
