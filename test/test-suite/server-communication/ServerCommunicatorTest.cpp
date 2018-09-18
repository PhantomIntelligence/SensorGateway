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

#ifndef SENSORGATEWAY_SERVERCOMMUNICATORTEST_CPP
#define SENSORGATEWAY_SERVERCOMMUNICATORTEST_CPP

#include <gtest/gtest.h>
#include <list>

#include "sensor-gateway/server-communication/ServerCommunicator.hpp"
#include "test/utilities/data-model/DataModelFixture.h"

using TestFunctions::DataTestUtil;

class ServerCommunicatorTest : public ::testing::Test {

protected:

    using ServerCommunicator = SensorAccessLinkElement::ServerCommunicator<Sensor::Test::Simple::Structures>;
    using Message = Sensor::Test::Simple::Structures::Message;
    using RawData = Sensor::Test::Simple::Structures::RawData;

    std::string const SERVER_ADRESS = "I like pears";

    ServerCommunicatorTest() = default;

    virtual ~ServerCommunicatorTest() = default;

};

namespace ServerCommunicatorTestMock {

    using ServerCommunication::ServerCommunicationStrategy;

    class MockServerCommunicatorStrategy final : public ServerCommunicationStrategy<Sensor::Test::Simple::Structures> {

    protected:

        using super = ServerCommunicationStrategy<Sensor::Test::Simple::Structures>;

        using super::Message;
        using super::RawData;

    public:

        MockServerCommunicatorStrategy() :
                openConnectionCalled(false),
                sendMessageCalled(false),
                sendRawDataCalled(false),
                closeConnectionCalled(false),
                sentMessage(Message::returnDefaultData()),
                sentRawData(RawData::returnDefaultData()){

        }

        ~MockServerCommunicatorStrategy() noexcept = default;

        void sendMessage(Message&& message) override {
            sendMessageCalled.store(true);
            sentMessage = message;
        }

        void sendRawData(RawData&& rawData) override {
            sendRawDataCalled.store(true);
            sentRawData = rawData;
        }

        void openConnection(std::string const& serverAddress) override {
            openConnectionCalled.store(true);
        }

        void closeConnection() override {
            closeConnectionCalled.store(true);
        }

        bool hasOpenConnectionBeenCalled() const {
            return openConnectionCalled.load();
        }

        bool hasCloseConnectionBeenCalled() const {
            return closeConnectionCalled.load();
        }

        bool hasSendMessageBeenCalled() const {
            return sendMessageCalled.load();
        }

        bool hasSendRawDataBeenCalled() const {
            return sendRawDataCalled.load();
        }
        Message getSentMessage() const {
            return sentMessage;
        }

        RawData getSentRawData() const {
            return sentRawData;
        }

    private:

        AtomicFlag openConnectionCalled;
        AtomicFlag sendMessageCalled;
        AtomicFlag sendRawDataCalled;
        AtomicFlag closeConnectionCalled;

        Message sentMessage;
        RawData sentRawData;
    };
}


TEST_F(ServerCommunicatorTest, given__when_connect_then_callsOpenConnectionInStrategy) {
    ServerCommunicatorTestMock::MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy);

    serverCommunicator.connect(SERVER_ADRESS);

    auto strategyHasBeenCalled = mockStrategy.hasOpenConnectionBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest, given_aMessageToSend_when_consume_then_callsSendMessageInStrategy) {
    ServerCommunicatorTestMock::MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy);
    auto message = DataTestUtil::createRandomSimpleMessage();

    serverCommunicator.consume(std::move(message));

    auto strategyHasBeenCalled = mockStrategy.hasSendMessageBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest, given_aMessageToSend_when_consume_then_callsSendMessageInStrategyWithTheMessage) {
    ServerCommunicatorTestMock::MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy);
    auto message = DataTestUtil::createRandomSimpleMessage();
    auto copy = Message(message);

    serverCommunicator.consume(std::move(message));

    auto receivedMessage = mockStrategy.getSentMessage();
    ASSERT_EQ(copy, receivedMessage);
}

TEST_F(ServerCommunicatorTest, given_aRawDataCycleToSend_when_consume_then_callsSendRawDataInStrategy) {
    ServerCommunicatorTestMock::MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy);
    auto rawData = DataTestUtil::createRandomSimpleRawData();

    serverCommunicator.consume(std::move(rawData));

    auto strategyHasBeenCalled = mockStrategy.hasSendRawDataBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest, given_aRawDataCycleToSend_when_consume_then_callsSendRawDataInStrategyWithTheRawData) {
    ServerCommunicatorTestMock::MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy);
    auto rawData = DataTestUtil::createRandomSimpleRawData();
    auto copy = RawData(rawData);

    serverCommunicator.consume(std::move(rawData));

    auto receivedRawData = mockStrategy.getSentRawData();
    ASSERT_EQ(copy, receivedRawData);
}

TEST_F(ServerCommunicatorTest, given__when_disconnect_then_callsCloseConnectionInStrategy) {
    ServerCommunicatorTestMock::MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy);

    serverCommunicator.disconnect();

    auto strategyHasBeenCalled = mockStrategy.hasCloseConnectionBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

#endif //SENSORGATEWAY_SERVERCOMMUNICATORTEST_CPP

