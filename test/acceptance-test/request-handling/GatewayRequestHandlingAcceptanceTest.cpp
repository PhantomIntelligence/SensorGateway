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
#include "sensor-gateway/application/SensorAccessLink.hpp"

// TODO : Extract this in another file as soon as it is needed in another test file
namespace GatewayRequestHandlingMock {


    template<typename S>
    struct AccessLink {

        using SensorStubGatewayStructures = Sensor::Gateway::Structures<
                typename S::MessageDefinition,
                typename S::RawDataDefinition,
                typename S::ControlMessageDefinition>;

        class SensorStubAccessLink final : public SensorGateway::SensorAccessLink<S, SensorStubGatewayStructures> {

        protected:

            using super = SensorGateway::SensorAccessLink<S, SensorStubGatewayStructures>;

            using typename super::ServerCommunicationStrategy;
            using typename super::DataTranslationStrategy;
            using typename super::SensorCommunicationStrategy;

        public:

            explicit SensorStubAccessLink(ServerCommunicationStrategy* serverCommunicationStrategy,
                                          DataTranslationStrategy* dataTranslationStrategy,
                                          SensorCommunicationStrategy* sensorCommunicationStrategy) :
                    super(serverCommunicationStrategy,
                          dataTranslationStrategy,
                          sensorCommunicationStrategy) {}

            ~SensorStubAccessLink() noexcept = default;

            SensorStubAccessLink(SensorStubAccessLink const& other) = delete;

            SensorStubAccessLink(SensorStubAccessLink&& other) noexcept = delete;

            SensorStubAccessLink& operator=(SensorStubAccessLink const& other)& = delete;

            SensorStubAccessLink& operator=(SensorStubAccessLink&& other)& noexcept = delete;

        };
    };

}

/**
 * @brief This acceptance test file includes *Gateway* only files (i.e. does not include sensor-specific translation and communication)
 */
class GatewayRequestHandlingAcceptanceTest : public ::testing::Test {

protected:

    std::string const SERVER_ADDRESS = "Die Potato!";

    using DataStructures = Sensor::Test::RealisticImplementation::Structures;
    using StructureAccessLink = GatewayRequestHandlingMock::AccessLink<DataStructures>;
    using GatewayStructures = StructureAccessLink::SensorStubGatewayStructures;

    using AvailableParameters = DataStructures::Parameters;
    using FakeSensorMessage = DataStructures::Message;

    using ServerCommunicationStrategyMock = Mock::RequestResponseSpecializedServerCommunicationStrategyMock<GatewayStructures>;
    using DataTranslationStrategyMock = Mock::DataTranslationStrategyMock<DataStructures, GatewayStructures>;
    using SensorCommunicationStrategyMock = Mock::LoopBackSensorCommunicationStrategyMock<DataStructures>;

    using TranslationStubbedAccessLink = StructureAccessLink::SensorStubAccessLink;

};


TEST_F(GatewayRequestHandlingAcceptanceTest,
       given_anInvalidParameterName_when_getParameterValue_then_sendResponseErrorMessageIsCalledInServerCommunicationStrategy) {
    ServerCommunicationStrategyMock serverCommunicationStrategyMock;
    serverCommunicationStrategyMock.increaseNumberOfUniqueInvalidGetParameterValueContentsToReturnBy(1);
    DataTranslationStrategyMock dataTranslationStrategyMock;
    SensorCommunicationStrategyMock sensorCommunicationStrategyMock;
    TranslationStubbedAccessLink accessLink(&serverCommunicationStrategyMock, &dataTranslationStrategyMock, &sensorCommunicationStrategyMock);

    accessLink.start(SERVER_ADDRESS);
    serverCommunicationStrategyMock.waitUntilSendResponseErrorMessageIsCalled();
    accessLink.terminateAndJoin();

    auto strategyCalled = serverCommunicationStrategyMock.hasSendResponseErrorMessageBeenCalled();

    ASSERT_TRUE(strategyCalled);
}

// TODO: Write test : given a loop back data translation strategy and given a valid get parameter request -> receives response with loopback response and given request
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

    dataTranslationStrategyMock.onTranslateControlMessageToSensorMessageRequest(std::move(expectedControlMessageRequest))->returnThisSensorMessage(fakeGetParameterRequest);

//    sensorCommunicationStrategyMock.onSendRequestReturn
    TranslationStubbedAccessLink accessLink(&serverCommunicationStrategyMock, &dataTranslationStrategyMock, &sensorCommunicationStrategyMock);

    accessLink.start(SERVER_ADDRESS);
    serverCommunicationStrategyMock.waitUntilSendResponseParameterErrorIsCalled();
    accessLink.terminateAndJoin();

    auto strategyCalled = serverCommunicationStrategyMock.hasSendResponseParameterErrorBeenCalled();

    ASSERT_TRUE(strategyCalled);
}

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
