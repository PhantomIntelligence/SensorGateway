#include "SpiritTrack.h"

SpiritTrack::SpiritTrack(uint16_t id, uint8_t confidenceLevel, uint16_t intensity) :
        id(id), confidenceLevel(confidenceLevel), intensity(intensity) {
};

SpiritTrack:: ~SpiritTrack(){
};

void SpiritTrack::setDistance(uint16_t distance){
    this->distance = distance;
};

void SpiritTrack::setSpeed(uint16_t speed){
    this->speed = speed;
};

void SpiritTrack::setAcceleration(uint16_t acceleration){
    this->acceleration = acceleration;
};

uint16_t SpiritTrack::getTrackID(){
    return id;
};

uint16_t SpiritTrack::getDistance(){
    return distance;
};

uint16_t SpiritTrack::getSpeed(){
    return speed;
};
uint16_t SpiritTrack::getAcceleration(){
    return acceleration;
};

uint16_t SpiritTrack::getIntensity(){
    return intensity;
};

uint16_t SpiritTrack::getConfidenceLevel(){
    return confidenceLevel;
};
