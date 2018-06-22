#ifndef SPIRITSENSORGATEWAY_CONSTANTFUNCTIONTEST_CPP
#define SPIRITSENSORGATEWAY_CONSTANTFUNCTIONTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "spirit-sensor-gateway/common/ConstantFunction.h"

class ConstantFunctionTest : public ::testing::Test {

};

TEST_F(ConstantFunctionTest, given_anArray_when_convertFourBytesToBigEndian_then_returnsTheCorrectlyFormated32Integer) {
    uint8_t testBytes[4];
    // 25 32 FF 0A is 624099082 in little endian
    testBytes[0] = 0x25;
    testBytes[1] = 0x32;
    testBytes[2] = 0xFF;
    testBytes[3] = 0x0A;

    uint32_t computedValue;
    convertFourBytesToBigEndian(testBytes, &computedValue);


    // 0A FF 32 25 is 184496677 in big endian
    uint32_t expectedValue = 184496677U;
    ASSERT_EQ(expectedValue,computedValue);
}

#endif //SPIRITSENSORGATEWAY_CONSTANTFUNCTIONTEST_CPP
