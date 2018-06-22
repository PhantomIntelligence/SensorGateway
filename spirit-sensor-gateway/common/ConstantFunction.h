

#ifndef SPIRITSENSORGATEWAY_CONSTANTFUNCTION_H
#define SPIRITSENSORGATEWAY_CONSTANTFUNCTION_H

#include <cstring>
#include "ConstantDefinition.h"

namespace {
    uint16_t convertTwoBytesToBigEndian(uint8_t firstByte, uint8_t secondByte) noexcept {
        return 256U * secondByte + firstByte;
    }

    void convertFourBytesToBigEndian(uint8_t (&littleEndianBytes)[4], uint32_t* bigEndianResult) noexcept {
        memcpy(bigEndianResult, littleEndianBytes, sizeof *bigEndianResult);
    }
}

#endif //SPIRITSENSORGATEWAY_CONSTANTFUNCTION_H
