#ifndef SPIRITSENSORGATEWAY_CONSTANTFUNCTIONTEST_CPP
#define SPIRITSENSORGATEWAY_CONSTANTFUNCTIONTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "spirit-sensor-gateway/common/ConstantFunction.h"

class ConstantFunctionTest : public ::testing::Test {

};

TEST_F(ConstantFunctionTest, given_anArray_when_convertFourBytesToUnsignedBigEndian_then_returnsTheCorrectlyFormated32Integer) {
    uint8_t testBytes[4];
    // 25 32 FF 0A is 624099082 in little endian
    testBytes[0] = 37;
    testBytes[1] = 50;
    testBytes[2] = 255;
    testBytes[3] = 10;

    uint32_t computedValue;
    convertFourBytesToBigEndian(testBytes, &computedValue);


    // 0A FF 32 25 is 184496677 in big endian
    uint32_t expectedValue = 184496677;
    ASSERT_EQ(expectedValue,computedValue);
}

TEST_F(ConstantFunctionTest,given_twoBytes_when_convertTwoBytesToUnsignedBigEndian_then_returnTheCorrectlyFormatted16BitsInteger){
    // 10 08 is 4104 in little endian
    uint8_t firstTestByte = 16;
    uint8_t secondTestByte = 8;

    uint16_t computedValue = convertTwoBytesToUnsignedBigEndian(firstTestByte, secondTestByte);

    //08 10 is 2064 in big endian
    uint16_t expectedValue = 2064;

    ASSERT_EQ(expectedValue,computedValue);

}

TEST_F(ConstantFunctionTest,given_twoBytes_when_convertTwoBytesToSignedBigEndian_then_returnTheCorrectlyFormatted16BitsInteger){
    uint8_t firstTestByte = 8;
    uint8_t secondTestByte = 255;

    int16_t computedValue = convertTwoBytesToSignedBigEndian(firstTestByte, secondTestByte);

    int16_t expectedValue = -248;

    ASSERT_EQ(expectedValue,computedValue);
}


#endif //SPIRITSENSORGATEWAY_CONSTANTFUNCTIONTEST_CPP
