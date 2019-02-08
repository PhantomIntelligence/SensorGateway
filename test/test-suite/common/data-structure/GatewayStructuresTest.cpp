#ifndef SENSORGATEWAY_GATEWAYSTRUCTURESTEST_CPP
#define SENSORGATEWAY_GATEWAYSTRUCTURESTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock-generated-matchers.h>

#include <test/utilities/data-model/DataModelFixture.h>
#include "sensor-gateway/common/data-structure/parameter/ParameterConstants.hpp"


using TestFunctions::DataTestUtil;
using Sensor::Test::Simple::Structures;

class GatewayStructuresTest : public ::testing::Test {

protected:

    static Byte const WHATEVER_INTERNAL_COMMAND = 0x4;
    static Byte const WHATEVER_INTERNAL_ADDRESS = 0x16;
    static uint8_t const WHATEVER_SMALL_INTEGER_VALUE = 4;

    template<typename GPD>
    struct TestParameter : public Sensor::Gateway::SensorParameterDefinition<
            GPD,
            WHATEVER_INTERNAL_COMMAND,
            WHATEVER_INTERNAL_ADDRESS,
            WHATEVER_SMALL_INTEGER_VALUE,
            WHATEVER_SMALL_INTEGER_VALUE,
            WHATEVER_SMALL_INTEGER_VALUE
    > {
    };

};

TEST_F(GatewayStructuresTest,
       given_aParameterList_when_getName_then_returnsTheNamesOfTheParameters) {

    // TODO: Create a Parameter List,
    //  TODO: declare a GatewayStructure with it
    //  TODO: Ensure the instantiated `Parameters`  from GatewayStructure returns same `getNames` the "type" list returns

//    ASSERT_THAT(actualName, testing::StrEq(expectedName));
}

#endif //SENSORGATEWAY_GATEWAYSTRUCTURESTEST_H
