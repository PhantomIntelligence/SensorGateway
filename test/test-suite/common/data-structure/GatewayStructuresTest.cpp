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

#ifndef SENSORGATEWAY_GATEWAYSTRUCTURESTEST_CPP
#define SENSORGATEWAY_GATEWAYSTRUCTURESTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock-generated-matchers.h>

#include <test/utilities/data-model/DataModelFixture.h>
#include <sensor-gateway/common/data-structure/gateway/GatewayStructures.h>


using TestFunctions::DataTestUtil;
using Sensor::Test::Simple::Structures;

class GatewayStructuresTest : public ::testing::Test {

protected:

    static Byte const WHATEVER_INTERNAL_COMMAND = 0x4;
    static Byte const WHATEVER_INTERNAL_ADDRESS = 0x16;
    static uint8_t const WHATEVER_SMALL_INTEGER_VALUE = 4;

    using RealisticTestStructures = Sensor::Test::RealisticImplementation::Structures;
    using PulseWidthDefinition = Sensor::Gateway::PulseWidthDefinition;
    using LaserControlDefinition = Sensor::Gateway::LaserControlDefinition;
    using AccumulationDefinition = Sensor::Gateway::AccumulationDefinition;
    using TIAGainDefinition = Sensor::Gateway::TIAGainDefinition;
    using NoParameter = Sensor::Gateway::Details::NoParameter;

    template<typename GPD>
    struct FakeSensorTestParameter : public Sensor::Gateway::SensorParameterDefinition<
            GPD,
            WHATEVER_INTERNAL_COMMAND,
            WHATEVER_INTERNAL_ADDRESS,
            WHATEVER_SMALL_INTEGER_VALUE,
            WHATEVER_SMALL_INTEGER_VALUE,
            WHATEVER_SMALL_INTEGER_VALUE
    > {
    };

};

namespace GatewayStructureTest {
    template<typename T>
    using Param = Sensor::Gateway::Parameter<GatewayStructuresTest::FakeSensorTestParameter<T>>;

}

TEST_F(GatewayStructuresTest,
       given_aParameterListAsTemplateParameter_when_compiledAndUsed_then_isValidTypeAndActsLikeThePassedType) {
    using FakeGatewayParametersList = Sensor::Gateway::Parameters<
            GatewayStructureTest::Param<PulseWidthDefinition>,
            GatewayStructureTest::Param<LaserControlDefinition>,
            GatewayStructureTest::Param<TIAGainDefinition>,
            GatewayStructureTest::Param<AccumulationDefinition>,
            GatewayStructureTest::Param<LaserControlDefinition>
    >;

    using GatewayTestSturctures = Sensor::Gateway::Structures<
            RealisticTestStructures::StubMessageDefinition,
            RealisticTestStructures::StubRawDataDefinition,
            RealisticTestStructures::StubControlMessageDefinition,
            FakeGatewayParametersList
    >;

    FakeGatewayParametersList fakeGatewayParametersList;
    GatewayTestSturctures::Parameters parameters;

    auto expectedParametersNames = fakeGatewayParametersList.getNames();
    auto actualParametersNames = parameters.getNames();

    ASSERT_EQ(expectedParametersNames, actualParametersNames);
}

#endif //SENSORGATEWAY_GATEWAYSTRUCTURESTEST_H
