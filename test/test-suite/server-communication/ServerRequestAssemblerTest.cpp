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

#ifndef SPIRITSENSORGATEWAY_SERVERREQUESTASSEMBLERTEST_CPP
#define SPIRITSENSORGATEWAY_SERVERREQUESTASSEMBLERTEST_CPP

#include <gtest/gtest.h>

#include "test/utilities/data-model/TestDataStructures.h"
#include "test/utilities/mock/DevNullDataTranslationStrategyMock.hpp"

#include "sensor-gateway/server-communication/ServerRequestAssembler.hpp"

class ServerRequestAssemblerTest : public ::testing::Test {

protected:

    ServerRequestAssemblerTest() = default;

    virtual ~ServerRequestAssemblerTest() = default;
};

namespace ServerRequestAssemblerTestMock {

    using RealisticDataStructures = Sensor::Test::RealisticImplementation::Structures;
    using DevNullTranslationStrategy = Mock::DevNullDataTranslationStrategyMock<RealisticDataStructures>;
}

TEST_F(ServerRequestAssemblerTest,
       given_aNameNotInAvailableParameters_when_ensureParameterIsAvailable_then_throwsSensorAccessLinkError) {

    using Sensor::Gateway::Parameters;
    using ImpossibleParameter = Sensor::FakeParameter::Impossible;
    using ImpossibleParameters = Parameters<ImpossibleParameter>;
    auto hasThrownError = false;
    try {
        Assemble::ServerRequest::ensureParameterIsAvailable<ImpossibleParameters>("test");
    } catch (ErrorHandling::SensorAccessLinkError& strategyError) {
        hasThrownError = true;
    }
    ASSERT_TRUE(hasThrownError);
}

TEST_F(ServerRequestAssemblerTest,
       given_aNameInAvailableParameters_when_ensureParameterIsAvailable_then_doesNotThrowsSensorAccessLinkError) {

    using Sensor::Gateway::Parameters;
    using ImpossibleParameter = Sensor::FakeParameter::Impossible;
    using ImpossibleParameters = Parameters<ImpossibleParameter>;
    auto hasThrownError = false;
    try {
        Assemble::ServerRequest::ensureParameterIsAvailable<ImpossibleParameters>(ImpossibleParameter::getStringifiedName());
    } catch (ErrorHandling::SensorAccessLinkError& strategyError) {
        hasThrownError = true;
    }
    ASSERT_FALSE(hasThrownError);
}

#endif //SPIRITSENSORGATEWAY_SERVERREQUESTASSEMBLERTEST_CPP


