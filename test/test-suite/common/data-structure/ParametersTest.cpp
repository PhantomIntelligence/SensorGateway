#ifndef SENSORGATEWAY_PARAMETERSTEST_CPP
#define SENSORGATEWAY_PARAMETERSTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock-generated-matchers.h>
#include "test/utilities/data-model/FakeParameterConstants.hpp"

class ParametersTest : public ::testing::Test {

protected:

    using WHATEVER_NAME = Sensor::Parameter::Names::InvalidName;
    using WHATEVER_TYPE = Sensor::Parameter::Types::Boolean;
    using WHATEVER_UNIT = Sensor::Parameter::Units::not_applicable;
    static Byte const WHATEVER_INTERNAL_COMMAND = 0x4;
    static Byte const WHATEVER_INTERNAL_ADDRESS = 0x16;
    static uint8_t const WHATEVER_SMALL_INTEGER_VALUE = 32;

    template<typename N>
    struct SensorParameterWithName : public Sensor::Gateway::SensorParameterDefinition<
            Sensor::Gateway::GatewayParameterDefinition<N, WHATEVER_TYPE, WHATEVER_UNIT>,
            WHATEVER_INTERNAL_COMMAND,
            WHATEVER_INTERNAL_ADDRESS,
            WHATEVER_SMALL_INTEGER_VALUE,
            WHATEVER_SMALL_INTEGER_VALUE,
            WHATEVER_SMALL_INTEGER_VALUE,
            WHATEVER_SMALL_INTEGER_VALUE
    > {
    };

    template<typename U>
    struct SensorParameterWithUnit : public Sensor::Gateway::SensorParameterDefinition<
            Sensor::Gateway::GatewayParameterDefinition<WHATEVER_NAME, WHATEVER_TYPE, U>,
            WHATEVER_INTERNAL_COMMAND,
            WHATEVER_INTERNAL_ADDRESS,
            WHATEVER_SMALL_INTEGER_VALUE,
            WHATEVER_SMALL_INTEGER_VALUE,
            WHATEVER_SMALL_INTEGER_VALUE,
            WHATEVER_SMALL_INTEGER_VALUE
    > {
    };
};

TEST_F(ParametersTest,
       given_aParameterDefinition_when_getName_then_returnsTheNameInAString) {
    using TestLiteralName = StringLiteral<decltype("Test"_ToString)>;
    typedef SensorParameterWithName <TestLiteralName> sensorParameterDefinition;

    Sensor::Gateway::Parameter<sensorParameterDefinition> parameterTest;

    std::string const actualName(parameterTest.getStringifiedName());

    std::string const expectedName("Test");
    ASSERT_THAT(actualName, testing::StrEq(expectedName));
}

TEST_F(ParametersTest,
       given_aParameterDefinition_when_getUnit_then_returnsTheUnitInAString) {
    using Unit = StringLiteral<decltype("picofarad"_ToString)>;
    typedef SensorParameterWithUnit <Unit> sensorParameterDefinition;

    Sensor::Gateway::Parameter<sensorParameterDefinition> parameterTest;

    std::string const actualUnit(parameterTest.getStringifiedUnit());

    std::string const expectedUnit("picofarad");
    ASSERT_THAT(actualUnit, testing::StrEq(expectedUnit));
}

TEST_F(ParametersTest,
       given_twoParameterWithDifferentNames_when_getNames_then_receivesNamesInStrings) {

    using FirstLiteralName = StringLiteral<decltype("Yin"_ToString)>;
    std::string const firstExpectedName("Yin");

    typedef SensorParameterWithName <FirstLiteralName> FirstParameterDefinition;

    using SecondLiteralName = StringLiteral<decltype("Yang"_ToString)>;
    std::string const secondExpectedName("Yang");
    typedef SensorParameterWithName <SecondLiteralName> SecondParameterDefinition;

    using FirstParameter = Sensor::Gateway::Parameter<FirstParameterDefinition>;
    using SecondParameter = Sensor::Gateway::Parameter<SecondParameterDefinition>;

    Sensor::Gateway::Parameters<FirstParameter, SecondParameter> parameters;
    auto names = parameters.getNames();

    auto firstActualName = names[0];
    auto secondActualName = names[1];

    ASSERT_THAT(firstActualName, testing::StrEq(firstExpectedName));
    ASSERT_THAT(secondActualName, testing::StrEq(secondExpectedName));
}

TEST_F(ParametersTest,
       given_twoParameterWithDifferentUnits_when_getUnits_then_receivesUnitsInStrings) {

    using FirstLiteralUnit = StringLiteral<decltype("Light years"_ToString)>;
    std::string const firstExpectedUnit("Light years");

    typedef SensorParameterWithUnit <FirstLiteralUnit> FirstParameterDefinition;

    using SecondLiteralUnit = StringLiteral<decltype("mol/Kg"_ToString)>;
    std::string const secondExpectedUnit("mol/Kg");
    typedef SensorParameterWithUnit <SecondLiteralUnit> SecondParameterDefinition;

    using FirstParameter = Sensor::Gateway::Parameter<FirstParameterDefinition>;
    using SecondParameter = Sensor::Gateway::Parameter<SecondParameterDefinition>;

    Sensor::Gateway::Parameters<FirstParameter, SecondParameter> parameters;
    auto names = parameters.getUnits();

    auto firstActualUnit = names[0];
    auto secondActualUnit = names[1];

    ASSERT_THAT(firstActualUnit, testing::StrEq(firstExpectedUnit));
    ASSERT_THAT(secondActualUnit, testing::StrEq(secondExpectedUnit));
}

TEST_F(ParametersTest, given_someParametersAndTheNameOfOneOfThem_when_isAvailable_then_returnsTrue) {

    using Apple = Sensor::FakeParameter::Apple;
    Sensor::FakeParameter::FruitBasketParameters fruitBasketParameters;
    Apple apple;

    std::string const appleName = apple.getStringifiedName();
    auto isParameterAvailable = fruitBasketParameters.isAvailable(appleName);

    ASSERT_TRUE(isParameterAvailable);
}

TEST_F(ParametersTest, given_someParametersAndTheOneNameNotInThem_when_isAvailable_then_returnsFalse) {

    using Carrot = Sensor::FakeParameter::Carrot;
    Sensor::FakeParameter::FruitBasketParameters fruitBasketParameters;
    Carrot carrot;

    std::string const carrotName = carrot.getStringifiedName();
    auto isParameterAvailable = fruitBasketParameters.isAvailable(carrotName);

    ASSERT_FALSE(isParameterAvailable);
}
#endif //SENSORGATEWAY_PARAMETERSTEST_H
