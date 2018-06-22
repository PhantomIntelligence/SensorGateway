#include "SpiritTrack.h"

SpiritTrack::SpiritTrack(uint16_t id, uint8_t confidenceLevel, uint16_t intensity) :
        id(id), confidenceLevel(confidenceLevel), intensity(intensity) {
};

SpiritTrack:: ~SpiritTrack(){
};

void SpiritTrack::setDistance(uint16_t distance){
    this->distance = distance;
};

void SpiritTrack::setSpeed(int16_t speed){
    this->speed = speed;
};

void SpiritTrack::setAcceleration(int16_t acceleration){
    this->acceleration = acceleration;
};

uint16_t SpiritTrack::getTrackID(){
    return id;
};

uint16_t SpiritTrack::getDistance(){
    return distance;
};

int16_t SpiritTrack::getSpeed(){
    return speed;
};
int16_t SpiritTrack::getAcceleration(){
    return acceleration;
};

uint16_t SpiritTrack::getIntensity(){
    return intensity;
};

uint8_t SpiritTrack::getConfidenceLevel(){
    return confidenceLevel;
};
