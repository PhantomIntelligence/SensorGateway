#ifndef SENSORGATEWAY_PARAMETERSTEST_CPP
#define SENSORGATEWAY_PARAMETERSTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock-generated-matchers.h>
#include "sensor-gateway/common/data-structure/gateway/Parameters.hpp"

class ParametersTest : public ::testing::Test {

protected:

    using WHATEVER_TYPE = bool;
    static Byte const WHATEVER_INTERNAL_ADDRESS = 0x16;
    static uint8_t const WHATEVER_SMALL_INTEGER_VALUE = 4;

};

TEST_F(ParametersTest,
       given_aParameterDefinition_when_getName_then_returnsTheNameInAString) {
    using TestLiteralName = StringLiteral<decltype("Test"_ToString)>;
    typedef typename Sensor::Gateway::SensorParameterDefinition<
            TestLiteralName,
            WHATEVER_TYPE,
            WHATEVER_INTERNAL_ADDRESS,
            WHATEVER_SMALL_INTEGER_VALUE,
            WHATEVER_SMALL_INTEGER_VALUE,
            WHATEVER_SMALL_INTEGER_VALUE
    > sensorParameterDefinition;
    Sensor::Gateway::Parameter<sensorParameterDefinition> parameterTest;

    std::string const actualName(parameterTest.getName());

    std::string const expectedName("Test");
    ASSERT_THAT(actualName, testing::StrEq(expectedName));
}

TEST_F(ParametersTest,
       given_twoParameterTypes_when_declaresParameters_then_compiles) {
    using FirstLiteralName = StringLiteral<decltype("Yin"_ToString)>;
    std::string const firstExpectedName("Yin");
    typedef typename Sensor::Gateway::SensorParameterDefinition<
            FirstLiteralName,
            WHATEVER_TYPE,
            WHATEVER_INTERNAL_ADDRESS,
            WHATEVER_SMALL_INTEGER_VALUE,
            WHATEVER_SMALL_INTEGER_VALUE,
            WHATEVER_SMALL_INTEGER_VALUE
    > FirstParameterDefinition;

    using SecondLiteralName = StringLiteral<decltype("Yang"_ToString)>;
    std::string const secondExpectedName("Yang");
    typedef typename Sensor::Gateway::SensorParameterDefinition<
            SecondLiteralName,
            WHATEVER_TYPE,
            WHATEVER_INTERNAL_ADDRESS,
            WHATEVER_SMALL_INTEGER_VALUE,
            WHATEVER_SMALL_INTEGER_VALUE,
            WHATEVER_SMALL_INTEGER_VALUE
    > SecondParameterDefinition;

    using FirstParameter = Sensor::Gateway::Parameter<FirstParameterDefinition>;
    using SecondParameter = Sensor::Gateway::Parameter<SecondParameterDefinition>;

    Sensor::Gateway::Parameters<FirstParameter, SecondParameter> parameters;
    auto names = parameters.getNames();

    auto firstActualName = std::get<0>(names);
    auto secondActualName = std::get<1>(names);

    ASSERT_THAT(firstActualName, testing::StrEq(firstExpectedName));
    ASSERT_THAT(secondActualName, testing::StrEq(secondExpectedName));
}

#endif //SENSORGATEWAY_PARAMETERSTEST_H
