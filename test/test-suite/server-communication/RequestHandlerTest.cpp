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

    using ServerCommunicator = SensorAccessLinkElement::ServerCommunicator<GatewayStructures>;
    using ServerCommunicatorRequestHandlingCallBacks = typename ServerCommunicator::RequestHandlingCallBacks;
    using DevNullCommunicationStrategy = Mock::DevNullServerCommunicationStrategyMock<GatewayStructures>;

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
        using MockFunctionProcessCalibrationRequest = Mock::Function<StringLiteral<decltype("mock->processCalibrationRequest"_ToString)>, Mock::VoidType, Mock::VoidType>;
        using MockFunctionProcessClearCalibrationRequest = Mock::Function<StringLiteral<decltype("mock->ProcessClearCalibrationRequest"_ToString)>, Mock::VoidType, Mock::VoidType>;

    public:

        explicit SensorParameterControllerMock() : super(nullptr, nullptr) {
        };

        ~SensorParameterControllerMock() noexcept {
        };

        void process(GetParameterValueRequest&& getParameterValueRequest) noexcept {
            mockFunctionProcessGetParameterValueRequest.invokeVoidReturn(
                    std::forward<GetParameterValueRequest>(getParameterValueRequest));
        }

        void calibrate() noexcept {
            mockFunctionProcessCalibrationRequest.invokeVoidReturn();
        }

        void clearCalibration() noexcept {
            mockFunctionProcessClearCalibrationRequest.invokeVoidReturn();
        }

        bool hasProcessGetParameterValueRequestBeenCalledWith(GetParameterValueRequest const& getParameterValueRequest) {
            return mockFunctionProcessGetParameterValueRequest.hasBeenInvokedWith(getParameterValueRequest);
        }

        bool hasProcessCalibrationRequestBeenCalled() {
            return mockFunctionProcessCalibrationRequest.hasBeenInvoked();
        }

        bool hasProcessClearCalibrationRequestBeenCalled() {
            return mockFunctionProcessClearCalibrationRequest.hasBeenInvoked();
        }
    private:

        MockFunctionProcessGetParameterValueRequest mockFunctionProcessGetParameterValueRequest;
        MockFunctionProcessCalibrationRequest mockFunctionProcessCalibrationRequest;
        MockFunctionProcessClearCalibrationRequest mockFunctionProcessClearCalibrationRequest;

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

        explicit DevNullServerCommunicatorMock(ServerCommunicator::RequestCallBackStore::InstancesTuple* callbacks)
                :
                ServerCommunicator(&devNullCommunicationStrategy,
                                   callbacks) {
        }

        void voidFunction() {

        }

        void getParameters(super::GetParameterValueRequest&& g) {

        }

    private:


        DevNullCommunicationStrategy devNullCommunicationStrategy;

    };
}

TEST_F(RequestHandlerTest, given_anInvalidGetParameterValueRequest_when_handle_then_producesSensorAccessLinkError) {

    auto numberOfErrorToReceive = 1;
    ErrorSinkMock sink(numberOfErrorToReceive);
    ErrorProcessingScheduler scheduler(&sink);

    RequestHandlerTestMock::DevNullServerCommunicatorMock* mockServerCommunicatorPointer;
    DevNullCommunicationStrategy devNullCommunicationStrategy;
    ServerCommunicatorRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    std::bind(&RequestHandlerTestMock::DevNullServerCommunicatorMock::voidFunction,
                              mockServerCommunicatorPointer),
                    std::bind(&RequestHandlerTestMock::DevNullServerCommunicatorMock::getParameters,
                              mockServerCommunicatorPointer, std::placeholders::_1),
                    std::bind(&RequestHandlerTestMock::DevNullServerCommunicatorMock::voidFunction,
                              mockServerCommunicatorPointer),
                    std::bind(&RequestHandlerTestMock::DevNullServerCommunicatorMock::voidFunction,
                              mockServerCommunicatorPointer)

            );
    RequestHandlerTestMock::DevNullServerCommunicatorMock mockServerCommunicator(&handlingCallBacks);
    mockServerCommunicatorPointer = &mockServerCommunicator;
    ServerCommunicator serverCommunicator(&devNullCommunicationStrategy, &handlingCallBacks);

    RequestHandler requestHandler(&serverCommunicator, nullptr);
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
    RequestHandlerTestMock::DevNullServerCommunicatorMock* mockServerCommunicatorPointer;
    DevNullCommunicationStrategy devNullCommunicationStrategy;
    ServerCommunicatorRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    std::bind(&RequestHandlerTestMock::DevNullServerCommunicatorMock::voidFunction,
                              mockServerCommunicatorPointer),
                    std::bind(&RequestHandlerTestMock::DevNullServerCommunicatorMock::getParameters,
                              mockServerCommunicatorPointer, std::placeholders::_1),
                    std::bind(&RequestHandlerTestMock::DevNullServerCommunicatorMock::voidFunction,
                              mockServerCommunicatorPointer),
                    std::bind(&RequestHandlerTestMock::DevNullServerCommunicatorMock::voidFunction,
                              mockServerCommunicatorPointer)

            );
    RequestHandlerTestMock::DevNullServerCommunicatorMock mockServerCommunicator(&handlingCallBacks);
    mockServerCommunicatorPointer = &mockServerCommunicator;

    using RequestHandlerTestMock::SensorParameterControllerMock;
    SensorParameterControllerMock sensorParameterControllerMock;
    RequestHandler::ParameterRequestCallBacks parameterRequestCallBacks =
            RequestHandler::ParameterRequestCallBackStore::createCallBacks(
                    std::bind(&SensorParameterControllerMock::process, &sensorParameterControllerMock,
                              std::placeholders::_1),
                    std::bind(&SensorParameterControllerMock::calibrate, &sensorParameterControllerMock),
                    std::bind(&SensorParameterControllerMock::clearCalibration, &sensorParameterControllerMock)
            );

    RequestHandler requestHandler(mockServerCommunicatorPointer, &parameterRequestCallBacks);

    auto validRequest = Given::aValidGetParameterValueRequest<AvailableParameters>();
    auto requestCopy = ServerCommunication::RequestTypes::GetParameterValue(validRequest);

    requestHandler.handleGetParameterValueRequest(std::move(validRequest));

    auto hasReceivedRequest = sensorParameterControllerMock
            .hasProcessGetParameterValueRequestBeenCalledWith(requestCopy);

    ASSERT_TRUE(hasReceivedRequest);
}

TEST_F(RequestHandlerTest,
       given__when_calibrate_then_forwardsTheRequestToTheSensorParameterController) {
    RequestHandlerTestMock::DevNullServerCommunicatorMock* mockServerCommunicatorPointer;
    DevNullCommunicationStrategy devNullCommunicationStrategy;
    ServerCommunicatorRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    std::bind(&RequestHandlerTestMock::DevNullServerCommunicatorMock::voidFunction,
                              mockServerCommunicatorPointer),
                    std::bind(&RequestHandlerTestMock::DevNullServerCommunicatorMock::getParameters,
                              mockServerCommunicatorPointer, std::placeholders::_1),
                    std::bind(&RequestHandlerTestMock::DevNullServerCommunicatorMock::voidFunction,
                              mockServerCommunicatorPointer),
                    std::bind(&RequestHandlerTestMock::DevNullServerCommunicatorMock::voidFunction,
                              mockServerCommunicatorPointer)

            );
    RequestHandlerTestMock::DevNullServerCommunicatorMock mockServerCommunicator(&handlingCallBacks);
    mockServerCommunicatorPointer = &mockServerCommunicator;

    using RequestHandlerTestMock::SensorParameterControllerMock;
    SensorParameterControllerMock sensorParameterControllerMock;
    RequestHandler::ParameterRequestCallBacks parameterRequestCallBacks =
            RequestHandler::ParameterRequestCallBackStore::createCallBacks(
                    std::bind(&SensorParameterControllerMock::process, &sensorParameterControllerMock,
                              std::placeholders::_1),
                    std::bind(&RequestHandlerTestMock::DevNullServerCommunicatorMock::voidFunction,
                              mockServerCommunicatorPointer),
                    std::bind(&RequestHandlerTestMock::DevNullServerCommunicatorMock::voidFunction,
                              mockServerCommunicatorPointer)
            );

    RequestHandler requestHandler(mockServerCommunicatorPointer, &parameterRequestCallBacks);

    auto validRequest = Given::aValidGetParameterValueRequest<AvailableParameters>();
    auto requestCopy = ServerCommunication::RequestTypes::GetParameterValue(validRequest);

    requestHandler.handleGetParameterValueRequest(std::move(validRequest));

    auto hasReceivedRequest = sensorParameterControllerMock
            .hasProcessGetParameterValueRequestBeenCalledWith(requestCopy);

    ASSERT_TRUE(hasReceivedRequest);
}

#endif //SENSORGATEWAY_REQUESTHANDLERTEST_CPP
