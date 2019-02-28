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
#include "test/utilities/mock/DevNullServerCommunicationStrategyMock.hpp"
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

    using ServerCommunicator = SensorAccessLinkElement::ServerCommunicator<RealisticDataStructures>;
    using DevNullCommunicationStrategy = Mock::DevNullServerCommunicationStrategyMock<RealisticDataStructures>;

    class DevNullServerCommunicatorMock final : public ServerCommunicator {

    protected:

        using super = ServerCommunicator;

    public:

        explicit DevNullServerCommunicatorMock() :
                ServerCommunicator(&devNullCommunicationStrategy, nullptr) {
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

#endif //SENSORGATEWAY_REQUESTHANDLERTEST_CPP


