

#ifndef SPIRITSENSORGATEWAY_CONSTANTFUNCTION_H
#define SPIRITSENSORGATEWAY_CONSTANTFUNCTION_H

#include "ConstantDefinition.h"

namespace {
     uint16_t convertTwoBytesToBigEndian(uint8_t firstByte,uint8_t secondByte) noexcept {
         return 256U*secondByte+firstByte;
     }
}

#endif //SPIRITSENSORGATEWAY_CONSTANTFUNCTION_H
