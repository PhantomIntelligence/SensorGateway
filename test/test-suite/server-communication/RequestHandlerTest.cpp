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

#ifndef SPIRITSENSORGATEWAY_REQUESTHANDLERTEST_CPP
#define SPIRITSENSORGATEWAY_REQUESTHANDLERTEST_CPP

#include <gtest/gtest.h>

#include "test/utilities/data-model/TestDataStructures.h"
#include "test/utilities/mock/DevNullDataTranslationStrategyMock.hpp"

#include "sensor-gateway/server-communication/RequestHandler.hpp"

class RequestHandlerTest : public ::testing::Test {

protected:

    RequestHandlerTest() = default;

    virtual ~RequestHandlerTest() = default;
};

namespace RequestHandlerTestMock {

    using RealisticDataStructures = Sensor::Test::RealisticImplementation::Structures;
    using DevNullTranslationStrategy = Mock::DevNullDataTranslationStrategyMock<RealisticDataStructures>;

    using DataTranslator = SensorAccessLinkElement::DataTranslator<RealisticDataStructures, RealisticDataStructures>;

    class CatchingDataTranslatorMock final : public DataTranslator {

    protected:


        using super = DataTranslator;

        using super::SensorControlMessage;

    public:

        explicit CatchingDataTranslatorMock() :
                DataTranslator(&devNullTranslationStrategy) {
        }

    private:

        DevNullTranslationStrategy devNullTranslationStrategy;
    };

    class RequestLoopBackDataTranslatorMock final : public DataTranslator {

    protected:


        using super = DataTranslator;

        using super::SensorControlMessage;

    public:

        explicit RequestLoopBackDataTranslatorMock() :
                DataTranslator(&devNullTranslationStrategy) {
        }


    private:

        DevNullTranslationStrategy devNullTranslationStrategy;
    };
}

TEST_F(RequestHandlerTest,
       given_aGetSensorParameterValueRequestWithNameNotInAvailableParameters_when_handleIncomingServerRequest_then_throwsSensorAccessLinkError) {

    using ImpossibleParameters = Sensor::Gateway::Parameters<
            Sensor::FakeParameter::Impossible
    >;
    auto request = Assemble::ServerRequest::getParameterValue<ImpossibleParameters>("test");
    // TODO : transfer isAvailable logic from assembler to RequestHandler
//    std::cout << " test : " << getImpossibleParameterValueRequest.payload.name->Name << std::endl;

//    ASSERT_TRUE(dataHasPassedThroughCorrectly);
}

//TEST_F(RequestHandlerTest,
//       given_catchingDataTranslatorAndAGetParameterValueRequest_when_handleIncomingServerRequest_then_theDataTranslatorIsAskedToCreateAGetParameterValueControlMessage) {
//    ASSERT_TRUE(dataHasPassedThroughCorrectly);
//}

TEST_F(RequestHandlerTest,
       given_aGetSensorParameterValueRequestALoopBackDataTranslatorAndAResponseWriter_when_handleIncomingServerRequest_then_asksResponseWriterToStoreAControlMessageFromTheTranslator) {
//    ASSERT_TRUE(dataHasPassedThroughCorrectly);
}


#endif //SPIRITSENSORGATEWAY_REQUESTHANDLERTEST_CPP


