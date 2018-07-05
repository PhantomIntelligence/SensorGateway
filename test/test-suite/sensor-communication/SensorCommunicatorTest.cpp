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

#include "spirit-sensor-gateway/sensor-communication/SensorCommunicator.h"
#include "data-model/DataModelFixture.h"

using SensorAccessLinkElement::SensorCommunicator;
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


TEST_F(SensorCommunicatorTest, given_aNewPad_when_fetchingNextPad_then_returnsSelf) {
}

#endif //SPIRITSENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP

