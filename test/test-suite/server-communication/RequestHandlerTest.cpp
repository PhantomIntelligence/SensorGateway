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

#ifndef SENSORGATEWAY_REQUESTHANDLERTEST_CPP
#define SENSORGATEWAY_REQUESTHANDLERTEST_CPP

#include <gtest/gtest.h>

#include "test/utilities/data-model/DataModelFixture.h"
#include "test/utilities/mock/DevNullDataTranslationStrategyMock.hpp"
#include "test/utilities/assertion/ErrorAssertion.hpp"

#include "sensor-gateway/parameter-control/SensorParameterController.hpp"
#include "sensor-gateway/server-communication/ServerRequestAssembler.hpp"

class RequestHandlerTest : public ::testing::Test {

public:

    using DataStructures = Sensor::Test::RealisticImplementation::Structures;

protected:

    using AvailableParameters = DataStructures::Parameters;
    using RequestHandler = SensorAccessLinkElement::RequestHandler<DataStructures, DataStructures>;

    using RequestAssembler = Assemble::ServerRequestAssembler;
    using GetParameterValueRequest = ServerCommunication::RequestTypes::GetParameterValue;

    RequestHandlerTest() = default;

    virtual ~RequestHandlerTest() = default;

    GetParameterValueRequest givenValidGetParameterValueRequest() {
        auto validParameterName = TestFunctions::Parameters::availableNames<AvailableParameters>().front();
        auto validGetParameterValueRequest = RequestAssembler::getParameterValueRequest(validParameterName);
        return validGetParameterValueRequest;
    }

    GetParameterValueRequest givenInvalidGetParameterValueRequest() {
        auto invalidParameterName = TestFunctions::Parameters::nonAvailableNames<AvailableParameters>().front();
        std::cout << "invalidParameterName : " << invalidParameterName << std::endl;
        auto invalidGetParameterValueRequest = RequestAssembler::getParameterValueRequest(invalidParameterName);
        return invalidGetParameterValueRequest;
    }

private:

};

namespace RequestHandlerTestMock {

    using RealisticDataStructures = typename RequestHandlerTest::DataStructures;

    class SensorParameterControllerMock final
            : public SensorAccessLinkElement::SensorParameterController<RealisticDataStructures, RealisticDataStructures> {

    };

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

TEST_F(RequestHandlerTest, given_anInvalidGetParameterValueRequest_when_handle_then_producesSensorAccessLinkError) {

    auto numberOfErrorToReceive = 1;
    ErrorSinkMock sink(numberOfErrorToReceive);
    ErrorProcessingScheduler scheduler(&sink);

    RequestHandler requestHandler(nullptr, nullptr);
    requestHandler.linkConsumer(&scheduler);

    auto invalidRequest = givenInvalidGetParameterValueRequest();
    requestHandler.handleGetParameterValueRequest(std::move(invalidRequest));

    sink.waitConsumptionToBeReached();
    scheduler.terminateAndJoin();

    Error expectedError = ErrorHandling::createRequestHandlingError(
            ErrorHandling::INVALID_PARAMETER_NAME,
            ErrorHandling::Origin::SERVER_REQUEST_HANDLING_PARAMETER,
            ErrorHandling::Message::PARAMETER_NOT_AVAILABLE);

    ASSERT_TRUE(Assert::errorHasBeenThrown(&sink, expectedError));
}


TEST_F(RequestHandlerTest, given_anInvalidGetParameterValueRequest_when_handle_then_asksTheServerCommunicatorToRespondBadRequest) {
    auto invalidRequest = givenInvalidGetParameterValueRequest();
    auto expectedBadRequest = ServerCommunication::RequestTypes::GetParameterValue(invalidRequest);
    expectedBadRequest.makeBad();

//    ASSERT_TRUE(hasThrownError);


// TODO : complete this test

//
//    using Sensor::Gateway::Parameters;
//    using ImpossibleParameter = Sensor::FakeParameter::Impossible;
//    using ImpossibleParameters = Parameters<ImpossibleParameter>;
//    auto hasThrownError = false;
//    try {
//        Assemble::ServerRequestAssembler<ImpossibleParameters>::ensureParameterIsAvailable(
//                ImpossibleParameter::getStringifiedName());
//    } catch (ErrorHandling::SensorAccessLinkError& strategyError) {
//        hasThrownError = true;
//    }
//    ASSERT_EQ(expectedBadRequest, actualRequest);
    SUCCEED();
}


#endif //SENSORGATEWAY_REQUESTHANDLERTEST_CPP


