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
#include "test/utilities/mock/LoopBackDataTranslationStrategyMock.hpp"
#include "test/utilities/mock/RequestResponseSpecializedServerCommunicationStrategyMock.hpp"
#include "test/utilities/mock/DevNullSensorCommunicationStrategyMock.hpp"

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

        template<typename CommunicationStrategy, typename TranslationStrategy>
        class SensorStubAccessLink final : public SensorGateway::SensorAccessLink<S, SensorStubGatewayStructures> {

        protected:

            using super = SensorGateway::SensorAccessLink<S, SensorStubGatewayStructures>;

            using typename super::ServerCommunicationStrategy;

        public:

            explicit SensorStubAccessLink(ServerCommunicationStrategy* serverCommunicationStrategy)
                    : super(serverCommunicationStrategy,
                            &translationStrategy,
                            &communicationStrategy) {}

            ~SensorStubAccessLink() noexcept = default;

            SensorStubAccessLink(SensorStubAccessLink const& other) = delete;

            SensorStubAccessLink(SensorStubAccessLink&& other) noexcept = delete;

            SensorStubAccessLink& operator=(SensorStubAccessLink const& other)& = delete;

            SensorStubAccessLink& operator=(SensorStubAccessLink&& other)& noexcept = delete;

        private:

            TranslationStrategy translationStrategy;
            CommunicationStrategy communicationStrategy;
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

    using ServerCommunicationStrategyMock = Mock::RequestResponseSpecializedServerCommunicationStrategyMock<GatewayStructures>;
    using DataTranslationStrategyMock = Mock::LoopBackDataTranslationStrategyMock<DataStructures, GatewayStructures>;
    using DevNullSensorCommunicationStrategyMock = Mock::DevNullSensorCommunicationStrategyMock<DataStructures>;

    using TranslationStubbedAccessLink = StructureAccessLink::SensorStubAccessLink<DevNullSensorCommunicationStrategyMock, DataTranslationStrategyMock>;

// TODO: Write test : given a loopback data translation strategy and given a valid get parameter request -> receives response with loopback response and given request

};


TEST_F(GatewayRequestHandlingAcceptanceTest,
       given_anInvalidParameterName_when_getParameterValue_then_sendResponseErrorMessageIsCalledInServerCommunicationStrategy) {
    ServerCommunicationStrategyMock serverCommunicationStrategyMock;
    serverCommunicationStrategyMock.increaseNumberOfUniqueInvalidGetParameterValueContentsToReturnBy(1);
    TranslationStubbedAccessLink accessLink(&serverCommunicationStrategyMock);

    accessLink.start(SERVER_ADDRESS);
    serverCommunicationStrategyMock.waitUntilSendResponseErrorMessageIsCalled();
    accessLink.terminateAndJoin();

    auto strategyCalled = serverCommunicationStrategyMock.hasSendResponseErrorMessageBeenCalled();

    ASSERT_TRUE(strategyCalled);
}

// TODO : Complete this test for complete story 3 implementation in SensorGateway
//TEST_F(GatewayRequestHandlingAcceptanceTest,
//       given_aValidParameterName_when_getParameterValue_then_serverCommunicationStrategyReceivesCorrectlyFormattedResponse) {
//    DataTranslator dataTranslator(&dataTranslationStrategyMock);
//    ServerCommunicationStrategyMock serverCommunicationStrategyMock;
//    RequestHandler* requestHandlerPointer;
//    ServerCommunicator serverCommunicator(&serverCommunicationStrategyMock,
//                                          std::bind(&RequestHandler::handleGetParameterValueRequest,
//                                                    requestHandlerPointer,
//                                                    std::placeholders::_1));
//    SensorParameterController sensorParameterController(requestHandlerPointer, &dataTranslator);
//    RequestHandler requestHandler(&serverCommunicator, &sensorParameterController);
//    requestHandlerPointer = &requestHandler;
//
//    serverCommunicationStrategyMock.increaseNumberOfUniqueInvalidGetParameterValueContentsToReturnBy(1);
//// TODO: Write test : given a loopback data translation strategy and given a invalid get parameter request -> receives bad request response with given request
//
////    ASSERT_TRUE(hasThrownError);
//
//
//    serverCommunicator.terminateAndJoin();
//    SUCCEED();
//}

#endif //SENSORGATEWAY_GATEWAYREQUESTHANDLINGACCEPTANCETEST_CPP
