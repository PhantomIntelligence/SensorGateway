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
#include <list>

#include "spirit-sensor-gateway/application/SensorAccessLink.hpp"
#include "test/utilities/data-model/DataModelFixture.h"

using DataModel::SimpleData;
using DataModel::SimpleDataContent;
using SimpleDataList = std::list<SimpleData>;
using TestFunctions::DataTestUtil;

class SensorAccessLinkTest : public ::testing::Test {

protected:
    SensorAccessLinkTest() = default;

    virtual ~SensorAccessLinkTest() = default;

};

using SimpleSensorCommunicationStrategy = SensorCommunication::SensorCommunicationStrategy<SimpleData>;

class MockSensorCommunicationStrategy final : public SimpleSensorCommunicationStrategy {

protected:

    using SimpleSensorCommunicationStrategy::DATA;
    using DataList = std::list<DATA>;

public:

    MockSensorCommunicationStrategy(int minimumNumberOfMessageToCreate) :
            promiseFulfilled(false),
            numberOfMessageCreated(0),
            minimumNumberOfMessageToCreate(minimumNumberOfMessageToCreate) {
    }


    DATA readMessage() override {
        DATA createdData = DataTestUtil::createRandomSimpleData();
        auto copy = DATA(createdData);
        createdDataCopies.push_back(copy);

        ++numberOfMessageCreated;
        if (hasCreatedMinimumNumberOfData() && !promiseFulfilled.load()) {
            promiseFulfilled.store(true);
            minimumNumberOfMessageCreated.set_value(true);
        }

        // WARNING! This mock implementation of readMessage needs to be slowed down because the way gtest works. DO NOT REMOVE.
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        std::this_thread::yield();

        return createdData;
    }

    void waitUntilReadMessageHasBeenCalledEnough() {
        if (!hasCreatedMinimumNumberOfData()) {
            minimumNumberOfMessageCreated.get_future().wait();
        }
    }

    DataList const& getCreatedMessageCopies() const {
        return createdDataCopies;
    }

private:

    bool hasCreatedMinimumNumberOfData() {
        LockGuard guard(numberOfMessageCreatedVerificationMutex);
        return numberOfMessageCreated.load() == minimumNumberOfMessageToCreate.load();
    }

    AtomicCounter numberOfMessageCreated;
    AtomicCounter minimumNumberOfMessageToCreate;

    AtomicFlag promiseFulfilled;
    Mutex numberOfMessageCreatedVerificationMutex;
    mutable BooleanPromise minimumNumberOfMessageCreated;

    DataList createdDataCopies;
};


#endif //SPIRITSENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP

