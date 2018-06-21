#include "SpiritPixel.h"

SpiritPixel::SpiritPixel(uint16_t id): id(id){};

SpiritPixel:: ~SpiritPixel(){
};

void SpiritPixel::addTrack(SpiritTrack spiritTrack){
    spiritTracks.push_back(spiritTrack);
}


SpiritTrack SpiritPixel::getTrackById(uint16_t trackID){
    SpiritTrack * spiritTrack = NULL;
    for (int trackNumber = 0; trackNumber < spiritTracks.size(); ++trackNumber) {
        if (spiritTracks[trackNumber].getTrackID() == trackID){
           spiritTrack = &spiritTracks[trackNumber];
           break;
        }
    }
    return * spiritTrack;
}
