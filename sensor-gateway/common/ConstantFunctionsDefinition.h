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

#ifndef SENSORGATEWAY_CONSTANTFUNCTIONSDEFINITION_H
#define SENSORGATEWAY_CONSTANTFUNCTIONSDEFINITION_H

#include "ConstantValuesDefinition.h"

namespace {
    uint16_t convertTwoBytesToUnsignedBigEndian(uint8_t firstByte, uint8_t secondByte) noexcept {
        auto shift = static_cast<uint16_t>(256U);
        uint16_t unsignedBigEndianNumber = shift * secondByte + firstByte;
        return unsignedBigEndianNumber;
    }

    void convertFourBytesToUnsignedBigEndian(uint8_t (& littleEndianBytes)[4], uint32_t* bigEndianResult) noexcept {
        memcpy(bigEndianResult, littleEndianBytes, sizeof *bigEndianResult);
    }

    int16_t convertTwoBytesToSignedBigEndian(uint8_t firstByte, uint8_t secondByte) noexcept {
        auto shift = static_cast<int16_t>(256);
        int16_t signedBigEndianNumber = shift * secondByte + firstByte;
        return signedBigEndianNumber;
    }

    int16_t reverseEndiannessOfInt16(int16_t value) {
        int16_t reverseValue = (static_cast<int16_t>(value & 0xFF00) >> 8) |
                               (static_cast<int16_t>(value & 0x00FF) << 8);
        return reverseValue;
    }

    template<typename T>
    inline DurationInNanoseconds castToDuration(T duration) {
        return std::chrono::duration_cast<DurationInNanoseconds>(duration);
    }

    /**
      * @warning Allows to start the various JoinableThreads in the constructors without blocking anything
      */
    void doNothing() {}

    // TODO : ERROR
    // TODO: uncomment this to have a warning pop everywhere a std::runtime_error is thrown and should be replaced by some other error
//    [[deprecated("std::runtime_error should be replaced with SensorAccessLinkError")]] [[noreturn]] void throwRuntimeError(char const* message) {
    [[noreturn]] void throwRuntimeError(char const* message) {
        throw std::runtime_error(message);
    }

}

namespace {
    template<typename T>
    using Bare = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

    template<typename Tuple, size_t... Is>
    std::array<typename std::tuple_element<0, Bare<Tuple>>::type,
            std::tuple_size<Bare<Tuple>>::value>
    convertTupleToArray(Tuple&& tuple, std::index_sequence<Is...>) {
        using std::get;
        return {{get<Is>(std::forward<Tuple>(tuple))...}};
    }

    template<typename Tuple>
    auto convertTupleToArray(Tuple&& tuple)
    -> decltype(convertTupleToArray(std::declval<Tuple>(), std::make_index_sequence<std::tuple_size<Bare<Tuple>>::value>{})) {
        return convertTupleToArray(std::forward<Tuple>(tuple), std::make_index_sequence<std::tuple_size<Bare<Tuple>>::value>{});
    }
}

#endif //SENSORGATEWAY_CONSTANTFUNCTIONSDEFINITION_H
