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

#ifndef SENSORGATEWAY_SERVERRESULTTEST_CPP
#define SENSORGATEWAY_SERVERRESULTTEST_CPP

#include <gtest/gtest.h>
#include <test/utilities/data-model/DataModelFixture.h>

#include "sensor-gateway/parameter-control/SensorParameterController.hpp"
#include "sensor-gateway/server-communication/ServerResponseAssembler.hpp"

class ServerResponseTest : public ::testing::Test {

protected:

    using AvailableParameters = Sensor::FakeParameter::FruitBasketParameters;
    using RequestAssembler = Assemble::ServerRequestAssembler;
    using GetParameterValueRequest = ServerCommunication::RequestTypes::GetParameterValue;

    ServerResponseTest() = default;

    virtual ~ServerResponseTest() = default;

private:

};

TEST_F(ServerResponseTest, given_aBadRequestResponse_when_isSuccess_then_returnsFalse) {
    auto invalidRequest = Given::anInvalidGetParameterValueRequest<AvailableParameters>();
    invalidRequest.markAsBadRequest();
    auto response = Assemble::ServerResponse::createBadRequestResponseFrom(std::move(invalidRequest));

    auto success = response.isSuccess();
    ASSERT_FALSE(success);
}

TEST_F(ServerResponseTest, given_aGoodRequestResponseThatCausedAnError_when_isSuccess_then_returnsFalse) {
    auto validRequest = Given::aValidGetParameterValueRequest<AvailableParameters>();
    validRequest.markAsErrorCause();
    auto response = Assemble::ServerResponse::createNoPayloadRequestResponseFrom(std::move(validRequest));

    auto success = response.isSuccess();
    ASSERT_FALSE(success);
}

#endif //SENSORGATEWAY_SERVERRESULTTEST_CPP


