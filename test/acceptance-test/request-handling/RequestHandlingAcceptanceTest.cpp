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

#ifndef SENSORGATEWAY_REQUESTHANDLINGACCEPTANCETEST_CPP
#define SENSORGATEWAY_REQUESTHANDLINGACCEPTANCETEST_CPP

#include <gtest/gtest.h>

#include "sensor-gateway/parameter-control/SensorParameterController.hpp"
#include "test/utilities/mock/ArbitraryDataSinkMock.hpp"
#include "test/utilities/mock/RequestResponseSpecializedServerCommunicationStrategyMock.hpp"


class RequestHandlingAcceptanceTest : public ::testing::Test {

    using RequestAssembler = Assemble::ServerRequestAssembler;

protected:

// TODO: Write test : given a loopback data translation strategy and given a valid get parameter request -> receives response with loopback response and given request
// TODO: Write test : given a loopback data translation strategy and given a invalid get parameter request -> receives bad request response with given request

};

#endif //SENSORGATEWAY_REQUESTHANDLINGACCEPTANCETEST_CPP
