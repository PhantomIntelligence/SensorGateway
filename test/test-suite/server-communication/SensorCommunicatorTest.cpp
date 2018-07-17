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

#ifndef SPIRITSERVERGATEWAY_SERVERCOMMUNICATORTEST_CPP
#define SPIRITSERVERGATEWAY_SERVERCOMMUNICATORTEST_CPP

#include <gtest/gtest.h>
#include <list>

#include "spirit-sensor-gateway/server-communication/ServerCommunicator.hpp"
#include "data-model/DataModelFixture.h"
#include "test/stub/SpiritFramesStub.h"

using DataFlow::Frame;
using Frames = std::list<Frame>;
using ServerCommunicator = SensorAccessLinkElement::ServerCommunicator<Frame>;
using ServerCommunication::ServerCommunicationStrategy;
using TestFunctions::DataTestUtil;
using Stub::createArbitrarySpiritFrame;

class ServerCommunicatorTest : public ::testing::Test {

protected:
    ServerCommunicatorTest() = default;

    virtual ~ServerCommunicatorTest() = default;

};

class MockServerCommunicatorStrategy final : public ServerCommunicationStrategy<Frame> {
public:
    MockServerCommunicatorStrategy() :
            openConnectionCalled(false),
            sendMessageCalled(false),
            closeConnectionCalled(false),
            sentMessage(Frame::returnDefaultData()) {

    }

    ~MockServerCommunicatorStrategy() noexcept = default;

    void sendMessage(MESSAGE&& message) {
        sendMessageCalled.store(true);
        sentMessage = message;
    }

    void openConnection() override {
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

    bool hasReadMessageBeenCalled() const {
        return sendMessageCalled.load();
    }

    Frame getSentMessage() const {
        return sentMessage;
    }

private:
    AtomicFlag openConnectionCalled;
    AtomicFlag sendMessageCalled;
    AtomicFlag closeConnectionCalled;

    Frame sentMessage;
};

TEST_F(ServerCommunicatorTest, given__when_connect_then_callsOpenConnectionInStrategy) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy);

    serverCommunicator.connect();

    auto strategyHasBeenCalled = mockStrategy.hasOpenConnectionBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest, given_aMessageToSend_when_consume_then_callsSendMessageInStrategy) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy);
    auto frame = createArbitrarySpiritFrame();

    serverCommunicator.consume(std::move(frame));

    auto strategyHasBeenCalled = mockStrategy.hasReadMessageBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

TEST_F(ServerCommunicatorTest, given_aMessageToSend_when_consume_then_callsSendMessageInStrategyWithTheMessage) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy);
    auto frame = createArbitrarySpiritFrame();
    auto frameCopy = Frame(frame);

    serverCommunicator.consume(std::move(frame));

    auto receivedFrame = mockStrategy.getSentMessage();
    ASSERT_EQ(frameCopy, receivedFrame);
}

TEST_F(ServerCommunicatorTest, given__when_disconnect_then_callsCloseConnectionInStrategy) {
    MockServerCommunicatorStrategy mockStrategy;
    ServerCommunicator serverCommunicator(&mockStrategy);
    
    serverCommunicator.disconnect();

    auto strategyHasBeenCalled = mockStrategy.hasCloseConnectionBeenCalled();
    ASSERT_TRUE(strategyHasBeenCalled);
}

#endif //SPIRITSERVERGATEWAY_SERVERCOMMUNICATORTEST_CPP

