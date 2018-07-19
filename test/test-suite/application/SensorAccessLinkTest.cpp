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
#include <chrono>

#include "spirit-sensor-gateway/application/SensorAccessLink.hpp"
#include "test/utilities/data-model/DataModelFixture.h"

using DataModel::SimpleData;
using SimpleDataList = std::list<SimpleData>;
using SensorCommunication::SensorCommunicationStrategy;
using MessageTranslation::MessageTranslationStrategy;
using ServerCommunication::ServerCommunicationStrategy;
using SpiritSensorGateway::SensorAccessLink;
using namespace std::chrono;

class SensorAccessLinkTest : public ::testing::Test {
protected:

    typedef std::chrono::time_point<std::chrono::system_clock> Timestamp;

    class SensorCommunicationStrategyMock final: public SensorCommunicationStrategy<SimpleData> {

    public:

        explicit SensorCommunicationStrategyMock() : numberOfTimesOpenConnectionIsCalled(0),
                                                     numberOfTimesCloseConnectionIsCalled(0),
                                                     readMessageHasBeenCalled(false),
                                                     readMessageFunctionCallTimestamp(Timestamp()),
                                                     openConnectionFunctionCallTimestamp(Timestamp()),
                                                     closeConnectionFunctionCallTimestamp(Timestamp()){
        }

        ~ SensorCommunicationStrategyMock() = default;

        void openConnection() override {
            numberOfTimesOpenConnectionIsCalled++;
            openConnectionFunctionCallTimestamp = std::chrono::system_clock::now();
        }

        void closeConnection() override {
            numberOfTimesCloseConnectionIsCalled++;
            closeConnectionFunctionCallTimestamp = std::chrono::system_clock::now();
        }

        SimpleData readMessage() override {
            SimpleData sommeMessage = TestFunctions::DataTestUtil::createRandomSimpleData();
            if(!readMessageHasBeenCalled.load()){
                readMessageHasBeenCalled.store(true);
                readMessageFunctionCallTimestamp = std::chrono::system_clock::now();
            }

            // WARNING! This mock implementation of readMessage needs to be slowed down because the way gtest works. DO NOT REMOVE.
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            std::this_thread::yield();

            return sommeMessage;
        }

        bool openConnectionHasBeenCalledOnce(){
            return numberOfTimesOpenConnectionIsCalled.load() == 1;
        }

        bool closeConnectionHasBeenCalledOnce(){
            return numberOfTimesCloseConnectionIsCalled.load() == 1;
        }

        Timestamp getOpenConnectionFunctionCallTimestamp(){
            return openConnectionFunctionCallTimestamp;
        }

        Timestamp getCloseConnectionFunctionCallTimestamp(){
            return closeConnectionFunctionCallTimestamp;
        }

        Timestamp getReadMessageFunctionCallTimestamp(){
            return readMessageFunctionCallTimestamp;
        }

    private:
        AtomicCounter numberOfTimesOpenConnectionIsCalled;
        AtomicCounter numberOfTimesCloseConnectionIsCalled;
        AtomicFlag readMessageHasBeenCalled;
        Timestamp openConnectionFunctionCallTimestamp;
        Timestamp closeConnectionFunctionCallTimestamp;
        Timestamp readMessageFunctionCallTimestamp;

    };

    class MessageTranslationStrategyMock final : public MessageTranslationStrategy<SimpleData, SimpleData> {

    public:

        explicit MessageTranslationStrategyMock(): translateMessageHasBeenCalled(false),
                                                   translateMessageFunctionCallTimestamp(Timestamp()){

        }

        ~ MessageTranslationStrategyMock() = default;

        void translateMessage(SimpleData&& inputMessage) override {
            produce(std::move(inputMessage));
            if(!translateMessageHasBeenCalled.load()){
                translateMessageHasBeenCalled.store(true);
                translateMessageFunctionCallTimestamp = std::chrono::system_clock::now();
            }
        }

        Timestamp getTranslateMessageFunctionCallTimestamp(){
            return translateMessageFunctionCallTimestamp;
        }

    private:
        AtomicFlag translateMessageHasBeenCalled;
        Timestamp translateMessageFunctionCallTimestamp;
    };


    class ServerCommunicationStrategyMock final : public ServerCommunicationStrategy<SimpleData> {

    public:
        explicit ServerCommunicationStrategyMock() : numberOfTimesOpenConnectionIsCalled(0),
                                                     numberOfTimesCloseConnectionIsCalled(0),
                                                     sendMessageHasBeenCalled(false),
                                                     openConnectionFunctionCallTimestamp(Timestamp()),
                                                     closeConnectionFunctionCallTimestamp(Timestamp()){

        }

        ~ServerCommunicationStrategyMock() = default;

        void openConnection() override {
            numberOfTimesOpenConnectionIsCalled++;
            openConnectionFunctionCallTimestamp = std::chrono::system_clock::now();
        }

        void closeConnection() override {
            numberOfTimesCloseConnectionIsCalled++;
            closeConnectionFunctionCallTimestamp = std::chrono::system_clock::now();
        }

        void sendMessage(SimpleData&& message) override {
            if(!sendMessageHasBeenCalled.load()){
                sendMessageHasBeenCalled.store(true);
                sendMessageFunctionCallTimestamp = std::chrono::system_clock::now();
            }
        }

        bool openConnectionHasBeenCalledOnce(){
            return numberOfTimesOpenConnectionIsCalled.load() == 1;
        }

        bool closeConnectionHasBeenCalledOnce(){
            return numberOfTimesCloseConnectionIsCalled.load() == 1;
        }

        Timestamp getOpenConnectionFunctionCallTimestamp(){
            return openConnectionFunctionCallTimestamp;
        }

        Timestamp getCloseConnectionFunctionCallTimestamp(){
            return closeConnectionFunctionCallTimestamp;
        }

        Timestamp getSendMessageFunctionCallTimestamp(){
            return sendMessageFunctionCallTimestamp;
        }

    private:
        AtomicCounter numberOfTimesOpenConnectionIsCalled;
        AtomicCounter numberOfTimesCloseConnectionIsCalled;
        AtomicFlag sendMessageHasBeenCalled;
        Timestamp openConnectionFunctionCallTimestamp;
        Timestamp closeConnectionFunctionCallTimestamp;
        Timestamp sendMessageFunctionCallTimestamp;
    };

    bool connectionOpeningIsValid(SensorCommunicationStrategyMock* sensorCommunicationStrategyMock,
                                  ServerCommunicationStrategyMock* serverCommunicationStrategyMock){
        bool openConnectionHasBeenCalledOnceInServerCommunicationStrategy =
                serverCommunicationStrategyMock->openConnectionHasBeenCalledOnce();
        bool openConnectionHasBeenCalledOnceInSensorCommunicationStrategy =
                sensorCommunicationStrategyMock->openConnectionHasBeenCalledOnce();
        bool openConnectionInServerCommunicationStrategyIsCalledFirst =
                (serverCommunicationStrategyMock->getOpenConnectionFunctionCallTimestamp() <
                 sensorCommunicationStrategyMock->getOpenConnectionFunctionCallTimestamp());

        return openConnectionHasBeenCalledOnceInServerCommunicationStrategy &&
               openConnectionHasBeenCalledOnceInSensorCommunicationStrategy &&
               openConnectionInServerCommunicationStrategyIsCalledFirst;
    }

    bool dataFlowIsValid(SensorCommunicationStrategyMock* sensorCommunicationStrategyMock,
                         MessageTranslationStrategyMock* messageTranslationStrategyMock,
                         ServerCommunicationStrategyMock* serverCommunicationStrategyMock) {

        bool readMessageIsCalledFirst =
                (sensorCommunicationStrategyMock-> getReadMessageFunctionCallTimestamp() <
                 serverCommunicationStrategyMock-> getSendMessageFunctionCallTimestamp()) &&
                (sensorCommunicationStrategyMock-> getReadMessageFunctionCallTimestamp() <
                 messageTranslationStrategyMock-> getTranslateMessageFunctionCallTimestamp());

        bool sendMessageIsCalledLast =
                (sensorCommunicationStrategyMock-> getReadMessageFunctionCallTimestamp() <
                 serverCommunicationStrategyMock-> getSendMessageFunctionCallTimestamp()) &&
                (messageTranslationStrategyMock-> getTranslateMessageFunctionCallTimestamp() <
                 serverCommunicationStrategyMock-> getSendMessageFunctionCallTimestamp());

        return (readMessageIsCalledFirst && sendMessageIsCalledLast);
    }

    bool connectionClosingIsValid(SensorCommunicationStrategyMock* sensorCommunicationStrategyMock,
                                  ServerCommunicationStrategyMock* serverCommunicationStrategyMock) {

        bool closeConnectionHasBeenCalledOnceInServerCommunicationStrategy =
                serverCommunicationStrategyMock->closeConnectionHasBeenCalledOnce();
        bool closeConnectionHasBeenCalledOnceInSensorCommunicationStrategy =
                sensorCommunicationStrategyMock->closeConnectionHasBeenCalledOnce();
        bool closeConnectionInSensorCommunicationStrategyIsCalledFirst =
                (sensorCommunicationStrategyMock->getCloseConnectionFunctionCallTimestamp() <
                 serverCommunicationStrategyMock->getCloseConnectionFunctionCallTimestamp());

        return closeConnectionHasBeenCalledOnceInServerCommunicationStrategy &&
               closeConnectionHasBeenCalledOnceInSensorCommunicationStrategy &&
               closeConnectionInSensorCommunicationStrategyIsCalledFirst;

    }
};


TEST_F(SensorAccessLinkTest, given__when_connecting_then_aValidConnectionIsEstablished){
    SensorCommunicationStrategyMock sensorCommunicationStrategyMock;
    MessageTranslationStrategyMock messageTranslationStrategyMock;
    ServerCommunicationStrategyMock serverCommunicationStrategyMock;

    SensorAccessLink<SimpleData, SimpleData>  sensorAccessLink(&serverCommunicationStrategyMock,
                                                               &messageTranslationStrategyMock,
                                                               &sensorCommunicationStrategyMock);

    sensorAccessLink.connect();
    sensorAccessLink.disconnect();

    ASSERT_TRUE(connectionOpeningIsValid(&sensorCommunicationStrategyMock, &serverCommunicationStrategyMock));
}

TEST_F(SensorAccessLinkTest, given__when_connecting_then_aValidDataFlowIsInstantiatedThroughTheGateway){
    SensorCommunicationStrategyMock sensorCommunicationStrategyMock;
    MessageTranslationStrategyMock messageTranslationStrategyMock;
    ServerCommunicationStrategyMock serverCommunicationStrategyMock;

    SensorAccessLink<SimpleData, SimpleData>  sensorAccessLink(&serverCommunicationStrategyMock,
                                                               &messageTranslationStrategyMock,
                                                               &sensorCommunicationStrategyMock);

    sensorAccessLink.connect();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sensorAccessLink.disconnect();

    ASSERT_TRUE(dataFlowIsValid(&sensorCommunicationStrategyMock, &messageTranslationStrategyMock,
                                &serverCommunicationStrategyMock));
}

TEST_F(SensorAccessLinkTest, given_aPreviouslyEstablisedConnection_when_disconnecting_then_theConnectionIsCorrectlyClosed){
    SensorCommunicationStrategyMock sensorCommunicationStrategyMock;
    MessageTranslationStrategyMock messageTranslationStrategyMock;
    ServerCommunicationStrategyMock serverCommunicationStrategyMock;

    SensorAccessLink<SimpleData, SimpleData>  sensorAccessLink(&serverCommunicationStrategyMock,
                                                               &messageTranslationStrategyMock,
                                                               &sensorCommunicationStrategyMock);

    sensorAccessLink.connect();
    sensorAccessLink.disconnect();

    ASSERT_TRUE(connectionClosingIsValid(&sensorCommunicationStrategyMock, &serverCommunicationStrategyMock));
}

#endif //SPIRITSENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP

