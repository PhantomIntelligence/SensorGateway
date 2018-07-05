/**
	Copyright 2014-2018 Phantom Intelligence Inc.

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

#ifndef SPIRITSENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP
#define SPIRITSENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP

#include <gtest/gtest.h>

#include "spirit-sensor-gateway/sensor-communication/SensorCommunicator.hpp"
#include "data-model/DataModelFixture.h"

using DataFlow::AWLMessage;
using AWLCommunicator = SensorAccessLinkElement::SensorCommunicator<AWLMessage>;
using TestFunctions::DataTestUtil;

/**
 * @brief Test Fixture meant to ensure correct behavior of SensorCommunicator.
 * @note A RingBuffer is used to implement the different tested functions
 */
class SensorCommunicatorTest : public ::testing::Test {

protected:
    SensorCommunicatorTest() = default;

    virtual ~SensorCommunicatorTest() = default;
};

class MockSensorCommunicationStrategy final : public SensorCommunication::CommunicationProtocolStrategy<AWLMessage> {

public:

    MockSensorCommunicationStrategy() : openConnectionCalled(false) {
    }

    AWLMessage readMessage() override {
        return AWLMessage::returnDefaultData();
    }

    void openConnection() override {
        openConnectionCalled.store(true);
    }

    void closeConnection() override {
    }

    bool hasOpenConnectionBeenCalled() const {
        return openConnectionCalled.load();
    }

private:

    AtomicFlag openConnectionCalled;

};

TEST_F(SensorCommunicatorTest, given_aSensorCommunicationStrategy_when_initializingConnection_then_callsOpenConnectionInStrategy) {
    MockSensorCommunicationStrategy mockStrategy;
    AWLCommunicator sensorCommunicator(&mockStrategy);

    sensorCommunicator.initializeConnection();

    auto strategyHasBeenCalled = mockStrategy.hasOpenConnectionBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}



#endif //SPIRITSENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP

