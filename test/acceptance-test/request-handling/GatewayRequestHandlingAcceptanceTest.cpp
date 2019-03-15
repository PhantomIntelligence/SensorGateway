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

#ifndef SENSORGATEWAY_GATEWAYREQUESTHANDLINGACCEPTANCETEST_CPP
#define SENSORGATEWAY_GATEWAYREQUESTHANDLINGACCEPTANCETEST_CPP

#include <gtest/gtest.h>

#include "test/utilities/data-model/DataModelFixture.h"
#include "test/utilities/mock/DataTranslationStrategyMock.hpp"
#include "test/utilities/mock/RequestResponseSpecializedServerCommunicationStrategyMock.hpp"
#include "test/utilities/mock/LoopBackSensorCommunicationStrategyMock.hpp"

#include "sensor-gateway/common/data-structure/gateway/GatewayStructures.h"
#include "sensor-gateway/application/SensorAccessLinkManager.hpp"


/**
 * @brief This acceptance test file includes *Gateway* only files (i.e. does not include sensor-specific translation and communication)
 */
class GatewayRequestHandlingAcceptanceTest : public ::testing::Test {

protected:

    std::string const SERVER_ADDRESS = "Die Potato!";


    using TestAccessLinkName = StringLiteral<decltype("TestAccessLink"_ToString)>;
    using DataStructures = Sensor::Test::RealisticImplementation::Structures;
    using FakeSensorMessage = DataStructures::Message;
    using DefaultParameter = SensorGateway::DefaultGuardianConnectionParameter;
    using TestAccessLink = SensorGateway::GenericAccessLink<
            TestAccessLinkName,
            DataStructures,
            Mock::DataTranslationStrategyMock<DataStructures, GatewayStructuresFor<DataStructures>>,
            Mock::LoopBackSensorCommunicationStrategyMock<DataStructures>,
            DefaultParameter
    >;

    using GatewayStructures = TestAccessLink::GatewayStructures;
    using AvailableParameters = DataStructures::Parameters;

    using ServerCommunicationStrategyMock = Mock::RequestResponseSpecializedServerCommunicationStrategyMock<GatewayStructures>;
    using DataTranslationStrategyMock = Mock::DataTranslationStrategyMock<DataStructures, GatewayStructures>;
    using SensorCommunicationStrategyMock = Mock::LoopBackSensorCommunicationStrategyMock<DataStructures>;

    using TranslationStubbedAccessLink = TestAccessLink::AccessLink;

};


TEST_F(GatewayRequestHandlingAcceptanceTest,
       given_anInvalidParameterName_when_getParameterValue_then_sendResponseErrorMessageIsCalledInServerCommunicationStrategy) {
    ServerCommunicationStrategyMock serverCommunicationStrategyMock;
    serverCommunicationStrategyMock.increaseNumberOfUniqueInvalidGetParameterValueContentsToReturnBy(1);
    DataTranslationStrategyMock dataTranslationStrategyMock;
    SensorCommunicationStrategyMock sensorCommunicationStrategyMock;
    TranslationStubbedAccessLink accessLink(&serverCommunicationStrategyMock,
                                            &dataTranslationStrategyMock,
                                            &sensorCommunicationStrategyMock);

    accessLink.start(SERVER_ADDRESS);
    serverCommunicationStrategyMock.waitUntilSendResponseErrorMessageIsCalled();
    accessLink.terminateAndJoin();

    auto strategyCalled = serverCommunicationStrategyMock.hasSendResponseErrorMessageBeenCalled();

    ASSERT_TRUE(strategyCalled);
}

TEST_F(GatewayRequestHandlingAcceptanceTest,
       given_aValidParameterName_when_getParameterValue_then_dataTranslatorIsAskedToTranslateASensorControlMessage) {
    ServerCommunicationStrategyMock serverCommunicationStrategyMock;
    DataTranslationStrategyMock dataTranslationStrategyMock;
    SensorCommunicationStrategyMock sensorCommunicationStrategyMock;
    AvailableParameters parameters;

    serverCommunicationStrategyMock.increaseNumberOfUniqueValidGetParameterValueContentsToReturnBy(1);
    auto validParameterRequestName = serverCommunicationStrategyMock.getReturnedGetParameterValueRequest()[0];
    auto expectedControlMessageRequest = parameters.createGetParameterValueControlMessageFor(validParameterRequestName);
    auto nameHash = std::hash<std::string>{}(validParameterRequestName);
    FakeSensorMessage fakeGetParameterRequest;
    fakeGetParameterRequest.sensorId = static_cast<decltype(fakeGetParameterRequest.sensorId)>(nameHash);
    fakeGetParameterRequest.messageId = expectedControlMessageRequest.getControlMessageCode();

    dataTranslationStrategyMock.onTranslateControlMessageToSensorMessageRequest(
            std::move(expectedControlMessageRequest))->returnThisSensorMessage(fakeGetParameterRequest);


    TranslationStubbedAccessLink accessLink(&serverCommunicationStrategyMock,
                                            &dataTranslationStrategyMock,
                                            &sensorCommunicationStrategyMock);

    accessLink.start(SERVER_ADDRESS);
    dataTranslationStrategyMock.waitUntilTranslateControlMessageToSensorMessageRequestInvocation();
    accessLink.terminateAndJoin();

    auto strategyCalled = dataTranslationStrategyMock.hasTranslateControlMessageToSensorMessageRequestBeenCalled();

    ASSERT_TRUE(strategyCalled);
}


TEST_F(GatewayRequestHandlingAcceptanceTest,
       given_aValidParameterName_when_getParameterValueAndAnErrorHappensInTheSensor_then_sensorCommunicationStrategyReceivesCorresponingSensorControlMessage) {
    ServerCommunicationStrategyMock serverCommunicationStrategyMock;
    DataTranslationStrategyMock dataTranslationStrategyMock;
    SensorCommunicationStrategyMock sensorCommunicationStrategyMock;
    AvailableParameters parameters;

    serverCommunicationStrategyMock.increaseNumberOfUniqueValidGetParameterValueContentsToReturnBy(1);
    auto validParameterRequestName = serverCommunicationStrategyMock.getReturnedGetParameterValueRequest()[0];
    auto expectedControlMessageRequest = parameters.createGetParameterValueControlMessageFor(validParameterRequestName);
    auto nameHash = std::hash<std::string>{}(validParameterRequestName);
    FakeSensorMessage fakeGetParameterRequest;
    fakeGetParameterRequest.sensorId = static_cast<decltype(fakeGetParameterRequest.sensorId)>(nameHash);
    fakeGetParameterRequest.messageId = expectedControlMessageRequest.getControlMessageCode();

    dataTranslationStrategyMock.onTranslateControlMessageToSensorMessageRequest(
            std::move(expectedControlMessageRequest))->returnThisSensorMessage(fakeGetParameterRequest);

    TranslationStubbedAccessLink accessLink(&serverCommunicationStrategyMock,
                                            &dataTranslationStrategyMock,
                                            &sensorCommunicationStrategyMock);

    accessLink.start(SERVER_ADDRESS);
    sensorCommunicationStrategyMock.waitUntilSendRequestInvocation();
    accessLink.terminateAndJoin();

    auto strategyCalled = sensorCommunicationStrategyMock.hasSendRequestBeenInvoked();

    ASSERT_TRUE(strategyCalled);
}

// TODO: Write test : given a loop back data translation strategy and given a valid get parameter request -> receives response with loopback response and given request
//TEST_F(GatewayRequestHandlingAcceptanceTest,
//       given_aParameterErrorSensorMessage_when_receivesResultFromSensor_then_serverCommunicationStrategyHasToSendResponseParameterError) {
//    ServerCommunicationStrategyMock serverCommunicationStrategyMock;
//    DataTranslationStrategyMock dataTranslationStrategyMock;
//    SensorCommunicationStrategyMock sensorCommunicationStrategyMock;
//    AvailableParameters parameters;
//
//    // serverCommunicationStrategyMock.increaseNumberOfUniqueValidGetParameterValueContentsToReturnBy(1);
//    // auto validParameterRequestName = serverCommunicationStrategyMock.getReturnedGetParameterValueRequest()[0];
//    // auto expectedControlMessageRequest = parameters.createGetParameterValueControlMessageFor(validParameterRequestName);
//    // auto nameHash = std::hash<std::string>{}(validParameterRequestName);
//    // FakeSensorMessage fakeGetParameterRequest;
//    // fakeGetParameterRequest.sensorId = static_cast<decltype(fakeGetParameterRequest.sensorId)>(nameHash);
//    // fakeGetParameterRequest.messageId = expectedControlMessageRequest.getControlMessageCode();
//
//    // dataTranslationStrategyMock.onTranslateControlMessageToSensorMessageRequest(std::move(expectedControlMessageRequest))->returnThisSensorMessage(fakeGetParameterRequest);
//
////  //   sensorCommunicationStrategyMock.onSendRequestReturn
//    // TranslationStubbedAccessLink accessLink(&serverCommunicationStrategyMock, &dataTranslationStrategyMock, &sensorCommunicationStrategyMock);
//
//    accessLink.start(SERVER_ADDRESS);
//    serverCommunicationStrategyMock.waitUntilSendResponseParameterErrorIsCalled();
//    accessLink.terminateAndJoin();
//
//    auto strategyCalled = serverCommunicationStrategyMock.hasSendResponseParameterErrorBeenCalled();
//
//    ASSERT_TRUE(strategyCalled);
//}


// TODO :
//TEST_F(GatewayRequestHandlingAcceptanceTest,
//       given_aValidGetParameterRequestForABooleanParameter_when_getParameterValue_then_serverCommunicationStrategyReceivesBooleanParameterResponse) {

#endif //SENSORGATEWAY_GATEWAYREQUESTHANDLINGACCEPTANCETEST_CPP
