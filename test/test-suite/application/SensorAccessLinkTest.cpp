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

#ifndef SPIRITSERVERGATEWAY_SENSORACCESSLINKTEST_CPP
#define SPIRITSERVERGATEWAY_SENSORACCESSLINKTEST_CPP

#include <gtest/gtest.h>
#include <chrono>

#include "spirit-sensor-gateway/application/SensorAccessLink.hpp"
#include "test/utilities/data-model/DataModelFixture.h"

using DataModel::SimpleData;
using SimpleDataList = std::list<SimpleData>;
using SensorCommunication::SensorCommunicationStrategy;
using MessageTranslation::MessageTranslationStrategy;
using ServerCommunication::ServerCommunicationStrategy;
using SpiritSensorGateway::SensorAccessLink;
using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;

namespace SensorAccessLinkTestMock {

    std::string const SERVER_ADDRESS = "This is a fake address";

    class SensorCommunicationStrategyMock final : public SensorCommunicationStrategy<SimpleData> {

    public:
        explicit SensorCommunicationStrategyMock() :
                numberOfTimesOpenConnectionIsCalled(0),
                numberOfTimesCloseConnectionIsCalled(0),
                readMessageHasBeenCalled(false),
                readMessageFunctionCallTimePoint(TimePoint()),
                openConnectionFunctionCallTimePoint(TimePoint()),
                closeConnectionFunctionCallTimePoint(TimePoint()) {
        }

        ~ SensorCommunicationStrategyMock() = default;

        void openConnection() override {
            numberOfTimesOpenConnectionIsCalled++;
            openConnectionFunctionCallTimePoint = std::chrono::steady_clock::now();
        }

        void closeConnection() override {
            numberOfTimesCloseConnectionIsCalled++;
            closeConnectionFunctionCallTimePoint = std::chrono::steady_clock::now();
        }

        SimpleData readMessage() override {
            SimpleData someMessage = TestFunctions::DataTestUtil::createRandomSimpleData();
            if (!readMessageHasBeenCalled.load()) {
                readMessageHasBeenCalled.store(true);
                readMessageFunctionCallTimePoint = std::chrono::steady_clock::now();
            }

            // WARNING! This mock implementation of readMessage needs to be slowed down because the way gtest works. DO NOT REMOVE.
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            std::this_thread::yield();

            return someMessage;
        }

        bool openConnectionHasBeenCalledOnce() {
            return numberOfTimesOpenConnectionIsCalled.load() == 1;
        }

        bool closeConnectionHasBeenCalledOnce() {
            return numberOfTimesCloseConnectionIsCalled.load() == 1;
        }

        TimePoint getOpenConnectionFunctionCallTimePoint() {
            return openConnectionFunctionCallTimePoint;
        }

        TimePoint getCloseConnectionFunctionCallTimePoint() {
            return closeConnectionFunctionCallTimePoint;
        }

        TimePoint getReadMessageFunctionCallTimePoint() {
            return readMessageFunctionCallTimePoint;
        }

    private:
        AtomicCounter numberOfTimesOpenConnectionIsCalled;
        AtomicCounter numberOfTimesCloseConnectionIsCalled;
        AtomicFlag readMessageHasBeenCalled;
        TimePoint openConnectionFunctionCallTimePoint;
        TimePoint closeConnectionFunctionCallTimePoint;
        TimePoint readMessageFunctionCallTimePoint;

    };

    class MessageTranslationStrategyMock final : public MessageTranslationStrategy<SimpleData, SimpleData> {

    public:

        explicit MessageTranslationStrategyMock() :
                translateMessageHasBeenCalled(false),
                translateMessageFunctionCallTimePoint(TimePoint()) {

        }

        ~ MessageTranslationStrategyMock() = default;

        void translateMessage(SimpleData&& inputMessage) override {
            produce(std::move(inputMessage));
            if (!translateMessageHasBeenCalled.load()) {
                translateMessageHasBeenCalled.store(true);
                translateMessageFunctionCallTimePoint = std::chrono::steady_clock::now();
            }
        }

        TimePoint getTranslateMessageFunctionCallTimePoint() {
            return translateMessageFunctionCallTimePoint;
        }

    private:
        AtomicFlag translateMessageHasBeenCalled;
        TimePoint translateMessageFunctionCallTimePoint;
    };


    class ServerCommunicationStrategyMock final : public ServerCommunicationStrategy<SimpleData> {

    public:
        explicit ServerCommunicationStrategyMock() :
                numberOfTimesOpenConnectionIsCalled(0),
                numberOfTimesCloseConnectionIsCalled(0),
                sendMessageHasBeenCalled(false),
                openConnectionFunctionCallTimePoint(TimePoint()),
                closeConnectionFunctionCallTimePoint(TimePoint()) {}

        ~ServerCommunicationStrategyMock() = default;

        void openConnection(std::string const& serverAddress) override {
            numberOfTimesOpenConnectionIsCalled++;
            openConnectionFunctionCallTimePoint = std::chrono::steady_clock::now();
        }

        void closeConnection() override {
            numberOfTimesCloseConnectionIsCalled++;
            closeConnectionFunctionCallTimePoint = std::chrono::steady_clock::now();
        }

        void sendMessage(SimpleData&& message) override {
            if (!sendMessageHasBeenCalled.load()) {
                sendMessageHasBeenCalled.store(true);
                sendMessageFunctionCallTimePoint = std::chrono::steady_clock::now();
            }
        }

        bool openConnectionHasBeenCalledOnce() {
            return numberOfTimesOpenConnectionIsCalled.load() == 1;
        }

        bool closeConnectionHasBeenCalledOnce() {
            return numberOfTimesCloseConnectionIsCalled.load() == 1;
        }

        TimePoint getOpenConnectionFunctionCallTimePoint() {
            return openConnectionFunctionCallTimePoint;
        }

        TimePoint getCloseConnectionFunctionCallTimePoint() {
            return closeConnectionFunctionCallTimePoint;
        }

        TimePoint getSendMessageFunctionCallTimePoint() {
            return sendMessageFunctionCallTimePoint;
        }

    private:
        AtomicCounter numberOfTimesOpenConnectionIsCalled;
        AtomicCounter numberOfTimesCloseConnectionIsCalled;
        AtomicFlag sendMessageHasBeenCalled;
        TimePoint openConnectionFunctionCallTimePoint;
        TimePoint closeConnectionFunctionCallTimePoint;
        TimePoint sendMessageFunctionCallTimePoint;
    };

    bool connectionOpeningIsValid(SensorCommunicationStrategyMock* sensorCommunicationStrategyMock,
                                  ServerCommunicationStrategyMock* serverCommunicationStrategyMock) {
        bool openConnectionHasBeenCalledOnceInServerCommunicationStrategy =
                serverCommunicationStrategyMock->openConnectionHasBeenCalledOnce();
        bool openConnectionHasBeenCalledOnceInSensorCommunicationStrategy =
                sensorCommunicationStrategyMock->openConnectionHasBeenCalledOnce();
        bool openConnectionInServerCommunicationStrategyIsCalledFirst =
                (serverCommunicationStrategyMock->getOpenConnectionFunctionCallTimePoint() <
                 sensorCommunicationStrategyMock->getOpenConnectionFunctionCallTimePoint());

        return openConnectionHasBeenCalledOnceInServerCommunicationStrategy &&
               openConnectionHasBeenCalledOnceInSensorCommunicationStrategy &&
               openConnectionInServerCommunicationStrategyIsCalledFirst;
    }
}

class SensorAccessLinkTest : public ::testing::Test {
protected:

    bool dataFlowIsValid(SensorAccessLinkTestMock::SensorCommunicationStrategyMock* sensorCommunicationStrategyMock,
                         SensorAccessLinkTestMock::MessageTranslationStrategyMock* messageTranslationStrategyMock,
                         SensorAccessLinkTestMock::ServerCommunicationStrategyMock* serverCommunicationStrategyMock) {

        bool readMessageIsCalledFirst =
                (sensorCommunicationStrategyMock->getReadMessageFunctionCallTimePoint() <
                 serverCommunicationStrategyMock->getSendMessageFunctionCallTimePoint()) &&
                (sensorCommunicationStrategyMock->getReadMessageFunctionCallTimePoint() <
                 messageTranslationStrategyMock->getTranslateMessageFunctionCallTimePoint());

        bool sendMessageIsCalledLast =
                (sensorCommunicationStrategyMock->getReadMessageFunctionCallTimePoint() <
                 serverCommunicationStrategyMock->getSendMessageFunctionCallTimePoint()) &&
                (messageTranslationStrategyMock->getTranslateMessageFunctionCallTimePoint() <
                 serverCommunicationStrategyMock->getSendMessageFunctionCallTimePoint());

        return (readMessageIsCalledFirst && sendMessageIsCalledLast);
    }

    bool connectionClosingIsValid(SensorAccessLinkTestMock::SensorCommunicationStrategyMock* sensorCommunicationStrategyMock,
                                  SensorAccessLinkTestMock::ServerCommunicationStrategyMock* serverCommunicationStrategyMock) {

        bool closeConnectionHasBeenCalledOnceInServerCommunicationStrategy =
                serverCommunicationStrategyMock->closeConnectionHasBeenCalledOnce();
        bool closeConnectionHasBeenCalledOnceInSensorCommunicationStrategy =
                sensorCommunicationStrategyMock->closeConnectionHasBeenCalledOnce();
        bool closeConnectionInSensorCommunicationStrategyIsCalledFirst =
                (sensorCommunicationStrategyMock->getCloseConnectionFunctionCallTimePoint() <
                 serverCommunicationStrategyMock->getCloseConnectionFunctionCallTimePoint());

        return closeConnectionHasBeenCalledOnceInServerCommunicationStrategy &&
               closeConnectionHasBeenCalledOnceInSensorCommunicationStrategy &&
               closeConnectionInSensorCommunicationStrategyIsCalledFirst;

    }

};

TEST_F(SensorAccessLinkTest, given__when_connecting_then_aValidConnectionIsEstablished) {
    SensorAccessLinkTestMock::SensorCommunicationStrategyMock sensorCommunicationStrategyMock;
    SensorAccessLinkTestMock::MessageTranslationStrategyMock messageTranslationStrategyMock;
    SensorAccessLinkTestMock::ServerCommunicationStrategyMock serverCommunicationStrategyMock;

    SensorAccessLink<SimpleData, SimpleData> sensorAccessLink(&serverCommunicationStrategyMock,
                                                              &messageTranslationStrategyMock,
                                                              &sensorCommunicationStrategyMock);

    sensorAccessLink.connect(SensorAccessLinkTestMock::SERVER_ADDRESS);
    sensorAccessLink.disconnect();

    ASSERT_TRUE(connectionOpeningIsValid(&sensorCommunicationStrategyMock, &serverCommunicationStrategyMock));
}

TEST_F(SensorAccessLinkTest, given__when_connecting_then_aValidDataFlowIsInstantiatedThroughTheGateway) {
    SensorAccessLinkTestMock::SensorCommunicationStrategyMock sensorCommunicationStrategyMock;
    SensorAccessLinkTestMock::MessageTranslationStrategyMock messageTranslationStrategyMock;
    SensorAccessLinkTestMock::ServerCommunicationStrategyMock serverCommunicationStrategyMock;

    SensorAccessLink<SimpleData, SimpleData> sensorAccessLink(&serverCommunicationStrategyMock,
                                                              &messageTranslationStrategyMock,
                                                              &sensorCommunicationStrategyMock);

    sensorAccessLink.connect(SensorAccessLinkTestMock::SERVER_ADDRESS);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sensorAccessLink.disconnect();

    ASSERT_TRUE(dataFlowIsValid(&sensorCommunicationStrategyMock, &messageTranslationStrategyMock,
                                &serverCommunicationStrategyMock));
}

TEST_F(SensorAccessLinkTest,
       given_aPreviouslyEstablisedConnection_when_disconnecting_then_theConnectionIsCorrectlyClosed) {
    SensorAccessLinkTestMock::SensorCommunicationStrategyMock sensorCommunicationStrategyMock;
    SensorAccessLinkTestMock::MessageTranslationStrategyMock messageTranslationStrategyMock;
    SensorAccessLinkTestMock::ServerCommunicationStrategyMock serverCommunicationStrategyMock;

    SensorAccessLink<SimpleData, SimpleData> sensorAccessLink(&serverCommunicationStrategyMock,
                                                              &messageTranslationStrategyMock,
                                                              &sensorCommunicationStrategyMock);

    sensorAccessLink.connect(SensorAccessLinkTestMock::SERVER_ADDRESS);
    sensorAccessLink.disconnect();

    ASSERT_TRUE(connectionClosingIsValid(&sensorCommunicationStrategyMock, &serverCommunicationStrategyMock));
}

#endif //SPIRITSERVERGATEWAY_SENSORACCESSLINKTEST_CPP
