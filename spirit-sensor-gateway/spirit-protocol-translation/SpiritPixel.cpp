#include "SpiritPixel.h"

SpiritPixel::SpiritPixel(uint16_t id): id(id){};

void SpiritPixel::addTrack(SpiritTrack spiritTrack){
    spiritTracks.push_back(spiritTrack);
}
