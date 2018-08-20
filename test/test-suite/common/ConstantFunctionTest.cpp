#ifndef SENSORGATEWAY_CONSTANTFUNCTIONTEST_CPP
#define SENSORGATEWAY_CONSTANTFUNCTIONTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "sensor-gateway/common/ConstantFunctionsDefinition.h"

class ConstantFunctionTest : public ::testing::Test {

};

TEST_F(ConstantFunctionTest, given_anArray_when_convertingFourBytesToUnsignedBigEndian_then_returnsTheCorrectlyFormated32BitsUnsignedInteger) {
    uint8_t testBytes[4];
    testBytes[0] = 37;
    testBytes[1] = 50;
    testBytes[2] = 255;
    testBytes[3] = 10;

    uint32_t computedValue;
    convertFourBytesToUnsignedBigEndian(testBytes, &computedValue);


    uint32_t expectedValue = 184496677;
    ASSERT_EQ(expectedValue,computedValue);
}

TEST_F(ConstantFunctionTest,given_twoBytes_when_convertingTwoBytesToUnsignedBigEndian_then_returnsTheCorrectlyFormatted16BitsUnsignedInteger){
    // 10 08 is 4104 in little endian
    uint8_t firstTestByte = 16;
    uint8_t secondTestByte = 8;

    uint16_t computedValue = convertTwoBytesToUnsignedBigEndian(firstTestByte, secondTestByte);

    //08 10 is 2064 in big endian
    uint16_t expectedValue = 2064;

    ASSERT_EQ(expectedValue,computedValue);

}

TEST_F(ConstantFunctionTest,given_twoBytes_when_convertingTwoBytesToSignedBigEndian_then_returnsTheCorrectlyFormatted16BitsSignedInteger){
    uint8_t firstTestByte = 8;
    uint8_t secondTestByte = 255;

    int16_t computedValue = convertTwoBytesToSignedBigEndian(firstTestByte, secondTestByte);

    int16_t expectedValue = -248;

    ASSERT_EQ(expectedValue,computedValue);
}


#endif //SENSORGATEWAY_CONSTANTFUNCTIONTEST_CPP
