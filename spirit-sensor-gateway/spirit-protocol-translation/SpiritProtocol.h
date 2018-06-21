//
// Created by samuelbouchard on 19/06/18.
//

#ifndef SPIRITSENSORGATEWAY_SPIRITPROTOCOL_H
#define SPIRITSENSORGATEWAY_SPIRITPROTOCOL_H

#include <vector>
#include <cstdint>


struct SensorTrack {
    uint64_t id;
    uint16_t distance;
    int16_t speed;
    int16_t acceleration;
    uint8_t confidenceLevel;
    uint16_t intensity;
};



struct SensorPixel {
    std::vector<SensorTrack> trackList;
    uint16_t id;

};

struct SensorFrame {
    std::vector<SensorPixel> pixelList;
    uint16_t frameID;
    uint16_t systemID;
    uint32_t errorFlag;
};

struct SpiritDebug{
    uint8_t id;
    uint16_t adress;
    int32_t value;
};
#endif //SPIRITSENSORGATEWAY_SPIRITPROTOCOL_H
