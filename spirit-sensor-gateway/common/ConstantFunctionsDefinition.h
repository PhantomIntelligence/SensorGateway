/**
	Copyright 2014-2018 Phantom Intelligence Inc.
	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at
		http://www.apache.org/licenses/LICENSE-2.0
	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#ifndef SPIRITSENSORGATEWAY_CONSTANTFUNCTIONSDEFINITION_H
#define SPIRITSENSORGATEWAY_CONSTANTFUNCTIONSDEFINITION_H


#include "ConstantValuesDefinition.h"


namespace {

    uint16_t convertTwoBytesToUnsignedBigEndian(uint8_t firstByte, uint8_t secondByte) noexcept {
        auto shift = static_cast<uint16_t>(256U);
        uint16_t unsignedBigEndianNumber = shift * secondByte + firstByte;
        return unsignedBigEndianNumber;
    }

    void convertFourBytesToUnsignedBigEndian(uint8_t (&littleEndianBytes)[4], uint32_t *bigEndianResult) noexcept {
        memcpy(bigEndianResult, littleEndianBytes, sizeof *bigEndianResult);
    }

    int16_t convertTwoBytesToSignedBigEndian(uint8_t firstByte, uint8_t secondByte) noexcept {
        auto shift = static_cast<int16_t>(256);
        int16_t signedBigEndianNumber = shift * secondByte + firstByte;
        return signedBigEndianNumber;
    }

    /**
      * @warning Allows to start the various JoinableThreads in the constructors without blocking anything
      */
    void doNothing() {}

    [[noreturn]] void throwIllegalActionException(char const *message) {
        throw std::runtime_error(message);
    }

};

#endif //SPIRITSENSORGATEWAY_CONSTANTFUNCTIONSDEFINITION_H
