#include <iostream>
#include "SpiritPixel.h"

SpiritPixel::SpiritPixel(uint16_t id): id(id){};

SpiritPixel:: ~SpiritPixel(){
};

uint16_t SpiritPixel::getId(){
    return id;
};

void SpiritPixel::addTrack(SpiritTrack spiritTrack){
    spiritTracks.insert(std::make_pair(spiritTrack.getTrackID(), spiritTrack));
}

std::unordered_map<uint16_t, SpiritTrack> SpiritPixel::getTracks(){
    return spiritTracks;
};

bool SpiritPixel::doesTrackExist(uint16_t trackID){
    bool trackExists =  false;
    if (spiritTracks.find(trackID) != spiritTracks.end()) {
        trackExists = true;
    }
    return trackExists;
};

SpiritTrack SpiritPixel::getTrackById(uint16_t trackID){
    std::cout << spiritTracks[trackID].getTrackID() << std::endl;
    SpiritTrack test = SpiritTrack(0, 0, 0);
    return test;
}
