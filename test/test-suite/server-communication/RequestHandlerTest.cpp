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
        using SetUnsignedIntegerParameterValueRequest = typename super::SetUnsignedIntegerParameterValueRequest;
        using SetSignedIntegerParameterValueRequest = typename super::SetSignedIntegerParameterValueRequest;
        using SetRealNumberParameterValueRequest = typename super::SetRealNumberParameterValueRequest;
        using SetBooleanParameterValueRequest = typename super::SetBooleanParameterValueRequest;

        using MockFunctionProcessGetParameterValueRequest = Mock::Function<StringLiteral<decltype("mock->processGetParameterValueRequest"_ToString)>, Mock::VoidType, GetParameterValueRequest>;
        using MockFunctionProcessSetUnsignedIntegerParameterValueRequest = Mock::Function<StringLiteral<decltype("mock->processSetUnsignedIntegerParameterValueRequest"_ToString)>, Mock::VoidType, SetUnsignedIntegerParameterValueRequest>;
        using MockFunctionProcessSetSignedIntegerParameterValueRequest = Mock::Function<StringLiteral<decltype("mock->processSetSignedIntegerParameterValueRequest"_ToString)>, Mock::VoidType, SetSignedIntegerParameterValueRequest>;
        using MockFunctionProcessSetRealNumberParameterValueRequest = Mock::Function<StringLiteral<decltype("mock->processSetRealNumberParameterValueRequest"_ToString)>, Mock::VoidType, SetRealNumberParameterValueRequest>;
        using MockFunctionProcessSetBooleanParameterValueRequest = Mock::Function<StringLiteral<decltype("mock->processSetBooleanParameterValueRequest"_ToString)>, Mock::VoidType, SetBooleanParameterValueRequest>;
        using MockFunctionProcessCalibrationRequest = Mock::Function<StringLiteral<decltype("mock->processCalibrationRequest"_ToString)>, Mock::VoidType, Mock::VoidType>;
        using MockFunctionProcessClearCalibrationRequest = Mock::Function<StringLiteral<decltype("mock->ProcessClearCalibrationRequest"_ToString)>, Mock::VoidType, Mock::VoidType>;

    public:

        explicit SensorParameterControllerMock() : super(nullptr, nullptr) {
        };

        ~SensorParameterControllerMock() noexcept {
        };

        void processGet(GetParameterValueRequest&& getParameterValueRequest) noexcept {
            invokeVoidReturn(
                    &mockFunctionProcessGetParameterValueRequest,
                    std::forward<GetParameterValueRequest>(getParameterValueRequest)
            );
        }

        void processSetUnsignedInteger(
                SetUnsignedIntegerParameterValueRequest&& setUnsignedIntegerParameterValueRequest) noexcept {
            invokeVoidReturn(
                    &mockFunctionProcessSetUnsignedIntegerParameterValueRequest,
                    std::forward<SetUnsignedIntegerParameterValueRequest>(setUnsignedIntegerParameterValueRequest)
            );
        }

        void processSetSignedInteger(
                SetSignedIntegerParameterValueRequest&& setSignedIntegerParameterValueRequest) noexcept {
            invokeVoidReturn(
                    &mockFunctionProcessSetSignedIntegerParameterValueRequest,
                    std::forward<SetSignedIntegerParameterValueRequest>(setSignedIntegerParameterValueRequest)
            );
        }

        void processSetRealNumber(SetRealNumberParameterValueRequest&& setRealNumberParameterValueRequest) noexcept {
            invokeVoidReturn(
                    &mockFunctionProcessSetRealNumberParameterValueRequest,
                    std::forward<SetRealNumberParameterValueRequest>(setRealNumberParameterValueRequest)
            );
        }

        void processSetBoolean(SetBooleanParameterValueRequest&& setBooleanParameterValueRequest) noexcept {
            invokeVoidReturn(
                    &mockFunctionProcessSetBooleanParameterValueRequest,
                    std::forward<SetBooleanParameterValueRequest>(setBooleanParameterValueRequest)
            );
        }

        void calibrate() noexcept {
            mockFunctionProcessCalibrationRequest.invokeVoidReturn();
        }

        void clearCalibration() noexcept {
            mockFunctionProcessClearCalibrationRequest.invokeVoidReturn();
        }

        bool hasProcessGetParameterValueRequestBeenCalledWith(
                GetParameterValueRequest const& getParameterValueRequest) {
            return hasBeenCalledWith(&mockFunctionProcessGetParameterValueRequest, getParameterValueRequest);
        }

        bool hasProcessSetUnsignedIntegerParameterValueRequestBeenCalledWith(
                SetUnsignedIntegerParameterValueRequest const& value) {
            return hasBeenCalledWith(&mockFunctionProcessSetUnsignedIntegerParameterValueRequest, value);
        }

        bool hasProcessSetSignedIntegerParameterValueRequestBeenCalledWith(
                SetSignedIntegerParameterValueRequest const& value) {
            return hasBeenCalledWith(&mockFunctionProcessSetSignedIntegerParameterValueRequest, value);
        }

        bool hasProcessSetRealNumberParameterValueRequestBeenCalledWith(
                SetRealNumberParameterValueRequest const& value) {
            return hasBeenCalledWith(&mockFunctionProcessSetRealNumberParameterValueRequest, value);
        }

        bool hasProcessSetBooleanParameterValueRequestBeenCalledWith(
                SetBooleanParameterValueRequest const& value) {
            return hasBeenCalledWith(&mockFunctionProcessSetBooleanParameterValueRequest, value);
        }


        bool hasProcessCalibrationRequestBeenCalled() {
            return mockFunctionProcessCalibrationRequest.hasBeenInvoked();
        }

        bool hasProcessClearCalibrationRequestBeenCalled() {
            return mockFunctionProcessClearCalibrationRequest.hasBeenInvoked();
        }

    private:

        template<typename MockFunction, typename Value>
        static void invokeVoidReturn(MockFunction* mockFunction, Value&& value) noexcept {
            return mockFunction->invokeVoidReturn(std::forward<Value>(value));
        }

        template<typename MockFunction, typename ExpectedValue>
        static bool hasBeenCalledWith(MockFunction* mockFunction, ExpectedValue const& expectedValue) noexcept {
            return mockFunction->hasBeenInvokedWith(expectedValue);
        }

        MockFunctionProcessGetParameterValueRequest mockFunctionProcessGetParameterValueRequest;
        MockFunctionProcessSetUnsignedIntegerParameterValueRequest mockFunctionProcessSetUnsignedIntegerParameterValueRequest;
        MockFunctionProcessSetSignedIntegerParameterValueRequest mockFunctionProcessSetSignedIntegerParameterValueRequest;
        MockFunctionProcessSetRealNumberParameterValueRequest mockFunctionProcessSetRealNumberParameterValueRequest;
        MockFunctionProcessSetBooleanParameterValueRequest mockFunctionProcessSetBooleanParameterValueRequest;
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

        void setUnsignedIntegerParameters(super::SetUnsignedIntegerParameterValueRequest&& s) {
        }

        void setSignedIntegerParameters(super::SetSignedIntegerParameterValueRequest&& s) {
        }

        void setRealNumberParameters(super::SetRealNumberParameterValueRequest&& s) {
        }

        void setBooleanParameters(super::SetBooleanParameterValueRequest&& s) {
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

    using DevNull = RequestHandlerTestMock::DevNullServerCommunicatorMock;
    DevNull* mockPointer;
    ServerCommunicatorRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    std::bind(&DevNull::voidFunction, mockPointer),
                    std::bind(&DevNull::getParameters, mockPointer, _1),
                    std::bind(&DevNull::setUnsignedIntegerParameters, mockPointer, _1),
                    std::bind(&DevNull::setSignedIntegerParameters, mockPointer, _1),
                    std::bind(&DevNull::setRealNumberParameters, mockPointer, _1),
                    std::bind(&DevNull::setBooleanParameters, mockPointer, _1),
                    std::bind(&DevNull::voidFunction, mockPointer),
                    std::bind(&DevNull::voidFunction, mockPointer)
            );

    RequestHandlerTestMock::DevNullServerCommunicatorMock mockServerCommunicator(&handlingCallBacks);
    mockPointer = &mockServerCommunicator;
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
    DevNullCommunicationStrategy devNullCommunicationStrategy;
    using DevNull = RequestHandlerTestMock::DevNullServerCommunicatorMock;
    DevNull* mockPointer;
    ServerCommunicatorRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    std::bind(&DevNull::voidFunction, mockPointer),
                    std::bind(&DevNull::getParameters, mockPointer, _1),
                    std::bind(&DevNull::setUnsignedIntegerParameters, mockPointer, _1),
                    std::bind(&DevNull::setSignedIntegerParameters, mockPointer, _1),
                    std::bind(&DevNull::setRealNumberParameters, mockPointer, _1),
                    std::bind(&DevNull::setBooleanParameters, mockPointer, _1),
                    std::bind(&DevNull::voidFunction, mockPointer),
                    std::bind(&DevNull::voidFunction, mockPointer)
            );

    DevNull mockServerCommunicator(&handlingCallBacks);
    mockPointer = &mockServerCommunicator;

    using ParamControllerMock = RequestHandlerTestMock::SensorParameterControllerMock;
    ParamControllerMock paramControllerMock;
    RequestHandler::ParameterRequestCallBacks parameterRequestCallBacks =
            RequestHandler::ParameterRequestCallBackStore::createCallBacks(
                    std::bind(&ParamControllerMock::processGet, &paramControllerMock, _1),
                    std::bind(&ParamControllerMock::processSetUnsignedInteger, &paramControllerMock, _1),
                    std::bind(&ParamControllerMock::processSetSignedInteger, &paramControllerMock, _1),
                    std::bind(&ParamControllerMock::processSetRealNumber, &paramControllerMock, _1),
                    std::bind(&ParamControllerMock::processSetBoolean, &paramControllerMock, _1),
                    std::bind(&ParamControllerMock::calibrate, &paramControllerMock),
                    std::bind(&ParamControllerMock::clearCalibration, &paramControllerMock)
            );

    RequestHandler requestHandler(mockPointer, &parameterRequestCallBacks);

    auto validRequest = Given::aValidGetParameterValueRequest<AvailableParameters>();
    auto requestCopy = ServerCommunication::RequestTypes::GetParameterValue(validRequest);

    requestHandler.handleGetParameterValueRequest(std::move(validRequest));

    auto hasReceivedRequest = paramControllerMock.hasProcessGetParameterValueRequestBeenCalledWith(requestCopy);

    ASSERT_TRUE(hasReceivedRequest);
}

TEST_F(RequestHandlerTest,
       given__when_calibrate_then_forwardsTheRequestToTheSensorParameterController) {
    RequestHandlerTestMock::DevNullServerCommunicatorMock* mockServerCommunicatorPointer;

    using DevNull = RequestHandlerTestMock::DevNullServerCommunicatorMock;
    DevNull* mockPointer;
    ServerCommunicatorRequestHandlingCallBacks handlingCallBacks =
            ServerCommunicator::RequestCallBackStore::createCallBacks(
                    std::bind(&DevNull::voidFunction, mockPointer),
                    std::bind(&DevNull::getParameters, mockPointer, _1),
                    std::bind(&DevNull::setUnsignedIntegerParameters, mockPointer, _1),
                    std::bind(&DevNull::setSignedIntegerParameters, mockPointer, _1),
                    std::bind(&DevNull::setRealNumberParameters, mockPointer, _1),
                    std::bind(&DevNull::setBooleanParameters, mockPointer, _1),
                    std::bind(&DevNull::voidFunction, mockPointer),
                    std::bind(&DevNull::voidFunction, mockPointer)
            );

    RequestHandlerTestMock::DevNullServerCommunicatorMock mockServerCommunicator(&handlingCallBacks);
    mockServerCommunicatorPointer = &mockServerCommunicator;

    using ParamControllerMock = RequestHandlerTestMock::SensorParameterControllerMock;
    ParamControllerMock paramControllerMock;
    RequestHandler::ParameterRequestCallBacks parameterRequestCallBacks =
            RequestHandler::ParameterRequestCallBackStore::createCallBacks(
                    std::bind(&ParamControllerMock::processGet, &paramControllerMock, _1),
                    std::bind(&ParamControllerMock::processSetUnsignedInteger, &paramControllerMock, _1),
                    std::bind(&ParamControllerMock::processSetSignedInteger, &paramControllerMock, _1),
                    std::bind(&ParamControllerMock::processSetRealNumber, &paramControllerMock, _1),
                    std::bind(&ParamControllerMock::processSetBoolean, &paramControllerMock, _1),
                    std::bind(&ParamControllerMock::calibrate, &paramControllerMock),
                    std::bind(&ParamControllerMock::clearCalibration, &paramControllerMock)
            );

    RequestHandler requestHandler(mockServerCommunicatorPointer, &parameterRequestCallBacks);

    auto validRequest = Given::aValidGetParameterValueRequest<AvailableParameters>();
    auto requestCopy = ServerCommunication::RequestTypes::GetParameterValue(validRequest);

    requestHandler.handleGetParameterValueRequest(std::move(validRequest));

    auto hasReceivedRequest = paramControllerMock.hasProcessGetParameterValueRequestBeenCalledWith(requestCopy);

    ASSERT_TRUE(hasReceivedRequest);
}

#endif //SENSORGATEWAY_REQUESTHANDLERTEST_CPP
