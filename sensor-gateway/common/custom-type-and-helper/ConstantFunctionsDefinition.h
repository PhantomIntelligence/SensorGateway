/**
	Copyright 2014-2019 Phantom Intelligence Inc.

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

namespace {
    uint16_t castLittleEndianBytesToUInt16InHostEndianness(uint8_t firstByte, uint8_t secondByte) noexcept {
        auto shift = static_cast<uint16_t>(256U);
        uint16_t flippedEndiannessNumber = shift * secondByte + firstByte;
        return flippedEndiannessNumber;
    }

    void castLittleEndianBytesToUInt32InHostEndianness(
            uint8_t (& littleEndianBytes)[4],
            uint32_t* bigEndianResult
    ) noexcept {
        memcpy(bigEndianResult, littleEndianBytes, sizeof *bigEndianResult);
    }

    inline void insertAsLittleEndian(int16_t value, Byte* output) noexcept {
        output[0] = static_cast<Byte>(value & 0x00FF);
        output[1] = static_cast<Byte>((value & 0xFF00) >> 8);
    }

    inline void insertAsLittleEndian(uint16_t value, Byte* output) noexcept {
        output[0] = static_cast<Byte>(value & 0x00FF);
        output[1] = static_cast<Byte>((value & 0xFF00) >> 8);
    }

    inline void insertAsLittleEndian(int32_t value, Byte* output) noexcept {
        output[0] = static_cast<Byte>(value & 0x000000FF);
        output[1] = static_cast<Byte>((value & 0x0000FF00) >> 8);
        output[2] = static_cast<Byte>((value & 0x00FF0000) >> 16);
        output[3] = static_cast<Byte>((value & 0xFF000000) >> 24);
    }

    inline void insertAsLittleEndian(uint32_t value, Byte* output) noexcept {
        output[0] = static_cast<Byte>(value & 0x000000FF);
        output[1] = static_cast<Byte>((value & 0x0000FF00) >> 8);
        output[2] = static_cast<Byte>((value & 0x00FF0000) >> 16);
        output[3] = static_cast<Byte>((value & 0xFF000000) >> 24);
    }

    inline void insertAsLittleEndian(int64_t value, Byte* output) noexcept {
        output[0] = static_cast<Byte>(value & 0x00000000000000FF);
        output[1] = static_cast<Byte>((value & 0x000000000000FF00) >> 8);
        output[2] = static_cast<Byte>((value & 0x0000000000FF0000) >> 16);
        output[3] = static_cast<Byte>((value & 0x00000000FF000000) >> 24);
        output[4] = static_cast<Byte>((value & 0x000000FF00000000) >> 32);
        output[5] = static_cast<Byte>((value & 0x0000FF0000000000) >> 40);
        output[6] = static_cast<Byte>((value & 0x00FF000000000000) >> 48);
        output[7] = static_cast<Byte>((value & 0xFF00000000000000) >> 56);
    }

    inline void insertAsLittleEndian(uint64_t value, Byte* output) noexcept {
        output[0] = static_cast<Byte>(value & 0x00000000000000FF);
        output[1] = static_cast<Byte>((value & 0x000000000000FF00) >> 8);
        output[2] = static_cast<Byte>((value & 0x0000000000FF0000) >> 16);
        output[3] = static_cast<Byte>((value & 0x00000000FF000000) >> 24);
        output[4] = static_cast<Byte>((value & 0x000000FF00000000) >> 32);
        output[5] = static_cast<Byte>((value & 0x0000FF0000000000) >> 40);
        output[6] = static_cast<Byte>((value & 0x00FF000000000000) >> 48);
        output[7] = static_cast<Byte>((value & 0xFF00000000000000) >> 56);
    }

    int16_t castLittleEndianBytesToInt16InHostEndianness(uint8_t firstByte, uint8_t secondByte) noexcept {
        auto shift = static_cast<int16_t>(256);
        int16_t signedBigEndianNumber = shift * secondByte + firstByte;
        return signedBigEndianNumber;
    }

    uint32_t flipEndiannessOfUInt32(uint32_t value) {
        uint32_t reverseValue = (static_cast<uint32_t>(value & 0xFF000000) >> 24) |
                                (static_cast<uint32_t>(value & 0x00FF0000) >> 8) |
                                (static_cast<uint32_t>(value & 0x0000FF00) << 8) |
                                (static_cast<uint32_t>(value & 0x000000FF) << 24);
        return reverseValue;
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

    inline void yield() {
        std::this_thread::yield();
    }

    inline void sleepForTenthOfASecond() {
        using namespace std::literals::chrono_literals;
        std::this_thread::sleep_for(100ms);
    }

    /**
      * @warning Allows to start the various JoinableThreads in the constructors without blocking anything
      */
    void doNothing() {
    }

    // TODO : ERROR
    // TODO: uncomment this to have a warning pop everywhere a std::runtime_error is thrown and should be replaced by some other error
//    [[deprecated("std::runtime_error should be replaced with SensorAccessLinkError")]] [[noreturn]] void throwRuntimeError(char const* message) {
    [[noreturn]] void throwRuntimeError(char const* message) {
        throw std::runtime_error(message);
    }

}

#endif //SENSORGATEWAY_CONSTANTFUNCTIONSDEFINITION_H
