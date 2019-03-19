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
#include "test/utilities/mock/Function.hpp"
#include "test/utilities/mock/DevNullDataTranslationStrategyMock.hpp"
#include "test/utilities/mock/DevNullServerCommunicationStrategyMock.hpp"
#include "test/utilities/assertion/ErrorAssertion.hpp"

#include "sensor-gateway/parameter-control/SensorParameterController.hpp"
#include "sensor-gateway/server-communication/ServerRequestAssembler.hpp"

class RequestHandlerTest : public ::testing::Test {

public:

    using DataStructures = Sensor::Test::RealisticImplementation::Structures;
    using GatewayStructures = GatewayStructuresFor<DataStructures>;

protected:

    using AvailableParameters = DataStructures::Parameters;
    using RequestHandler = SensorAccessLinkElement::RequestHandler<DataStructures, GatewayStructures>;

    using RequestAssembler = Assemble::ServerRequestAssembler;
    using GetParameterValueRequest = ServerCommunication::RequestTypes::GetParameterValue;

    RequestHandlerTest() = default;

    virtual ~RequestHandlerTest() = default;

private:

};

namespace RequestHandlerTestMock {

    using DataStructures = typename RequestHandlerTest::DataStructures;
    using GatewayStructures = typename RequestHandlerTest::GatewayStructures;

    class SensorParameterControllerMock final
            : public SensorAccessLinkElement::SensorParameterController<DataStructures, GatewayStructures> {

        using super = SensorAccessLinkElement::SensorParameterController<DataStructures, GatewayStructures>;

        using GetParameterValueRequest = typename super::GetParameterValueRequest;

        using MockFunctionProcessGetParameterValueRequest = Mock::Function<StringLiteral<decltype("mock->processGetParameterValueRequest"_ToString)>, Mock::VoidType, GetParameterValueRequest>;

    public:

        explicit SensorParameterControllerMock() : super(nullptr, nullptr) {};

        ~SensorParameterControllerMock() noexcept {};

        void process(GetParameterValueRequest&& getParameterValueRequest) noexcept {
            mockFunctionProcessGetParameterValueRequest.invokeVoidReturn(
                    std::forward<GetParameterValueRequest>(getParameterValueRequest));
        }

        bool
        hasProcessGetParameterValueRequestBeenCalledWith(GetParameterValueRequest const& getParameterValueRequest) {
            return mockFunctionProcessGetParameterValueRequest.hasBeenInvokedWith(getParameterValueRequest);
        }

    private:

        MockFunctionProcessGetParameterValueRequest mockFunctionProcessGetParameterValueRequest;

    };

    using DevNullTranslationStrategy = Mock::DevNullDataTranslationStrategyMock<DataStructures>;
    using DataTranslator = SensorAccessLinkElement::DataTranslator<DataStructures, GatewayStructures>;

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

    using ServerCommunicator = SensorAccessLinkElement::ServerCommunicator<GatewayStructures>;
    using DevNullCommunicationStrategy = Mock::DevNullServerCommunicationStrategyMock<GatewayStructures>;

    class DevNullServerCommunicatorMock final : public ServerCommunicator {

    protected:

        using super = ServerCommunicator;

    public:

        explicit DevNullServerCommunicatorMock() :
                ServerCommunicator(&devNullCommunicationStrategy,
                                   nullptr,
                                   nullptr,
                                   nullptr,
                                   nullptr) {
        }

    private:

        DevNullCommunicationStrategy devNullCommunicationStrategy;
    };
}

TEST_F(RequestHandlerTest, given_anInvalidGetParameterValueRequest_when_handle_then_producesSensorAccessLinkError) {

    auto numberOfErrorToReceive = 1;
    ErrorSinkMock sink(numberOfErrorToReceive);
    ErrorProcessingScheduler scheduler(&sink);

    RequestHandlerTestMock::DevNullServerCommunicatorMock serverCommunicatorMock;

    RequestHandler requestHandler(&serverCommunicatorMock, nullptr);
    requestHandler.linkConsumer(&scheduler);

    auto invalidRequest = Given::anInvalidGetParameterValueRequest<AvailableParameters>();
    requestHandler.handleGetParameterValueRequest(std::move(invalidRequest));

    sink.waitConsumptionToBeReached();
    scheduler.terminateAndJoin();

    Error expectedError = ErrorHandling::createRequestHandlingError(
            ErrorHandling::INVALID_PARAMETER_NAME,
            ErrorHandling::Origin::SERVER_REQUEST_HANDLING_PARAMETER,
            ErrorHandling::Message::PARAMETER_NOT_AVAILABLE);

    ASSERT_TRUE(Assert::errorHasBeenThrown(&sink, expectedError));
}

TEST_F(RequestHandlerTest,
       given_aValidGetParameterValueRequest_when_handle_then_forwardsTheRequestToTheSensorParameterController) {
    RequestHandlerTestMock::DevNullServerCommunicatorMock serverCommunicatorMock;
    using RequestHandlerTestMock::SensorParameterControllerMock;
    SensorParameterControllerMock sensorParameterControllerMock;
    RequestHandler requestHandler(&serverCommunicatorMock,
                                  std::bind(&SensorParameterControllerMock::process, &sensorParameterControllerMock,
                                            std::placeholders::_1));

    auto validRequest = Given::aValidGetParameterValueRequest<AvailableParameters>();
    auto requestCopy = ServerCommunication::RequestTypes::GetParameterValue(validRequest);

    requestHandler.handleGetParameterValueRequest(std::move(validRequest));

    auto hasReceivedRequest = sensorParameterControllerMock.hasProcessGetParameterValueRequestBeenCalledWith(
            requestCopy);

    ASSERT_TRUE(hasReceivedRequest);
}

#endif //SENSORGATEWAY_REQUESTHANDLERTEST_CPP
