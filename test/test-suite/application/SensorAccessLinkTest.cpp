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
        timestamp(Timestamp()){
        }

        void openConnection() override {
            numberOfTimesOpenConnectionIsCalled++;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            timestamp = std::chrono::system_clock::now();
        }

        void closeConnection() override {}

        SimpleData readMessage() override {
            SimpleData sommeMessage = TestFunctions::DataTestUtil::createRandomSimpleData();

            // WARNING! This mock implementation of readMessage needs to be slowed down because the way gtest works. DO NOT REMOVE.
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            std::this_thread::yield();
            return sommeMessage;
        }

        bool openConnectionHasBeenCalledOnce(){
            return numberOfTimesOpenConnectionIsCalled.load() == 1;
        }

        Timestamp getTimestamp(){
            return timestamp;
        }

    private:
        AtomicCounter numberOfTimesOpenConnectionIsCalled;
        Timestamp timestamp;

    };


    class MessageTranslationStrategyMock final : public MessageTranslationStrategy<SimpleData, SimpleData> {

    public:

        void translateMessage(SimpleData&& inputMessage) override {
            produce(std::move(inputMessage));
        }
    };


    class ServerCommunicationStrategyMock final : public ServerCommunicationStrategy<SimpleData> {

    public:
        explicit ServerCommunicationStrategyMock() : numberOfTimesOpenConnectionIsCalled(0),
        timestamp(Timestamp()){

        }

        ~ServerCommunicationStrategyMock() = default;

        void openConnection() override {
            numberOfTimesOpenConnectionIsCalled++;
            timestamp = std::chrono::system_clock::now();
        }

        void closeConnection() override {}

        void sendMessage(SimpleData&& message) override {
        }

        bool openConnectionHasBeenCalledOnce(){
            return numberOfTimesOpenConnectionIsCalled.load() == 1;
        }

        Timestamp getTimestamp(){
            return timestamp;
        }

    private:
        AtomicCounter numberOfTimesOpenConnectionIsCalled;
        Timestamp timestamp;
    };

    bool connectionIsValid(SensorCommunicationStrategyMock* sensorCommunicationStrategyMock,
                           ServerCommunicationStrategyMock* serverCommunicationStrategyMock){
        bool openConnectionHasBeenCalledOnceInServerCommunicationStrategy =
                serverCommunicationStrategyMock->openConnectionHasBeenCalledOnce();
        bool openConnectionHasBeenCalledOnceInSensorCommunicationStrategy =
                sensorCommunicationStrategyMock->openConnectionHasBeenCalledOnce();
        bool openConnectionInServerCommunicationStrategyIsCalledFirst =
                (serverCommunicationStrategyMock->getTimestamp() < sensorCommunicationStrategyMock->getTimestamp());

        return openConnectionHasBeenCalledOnceInServerCommunicationStrategy &&
               openConnectionHasBeenCalledOnceInSensorCommunicationStrategy &&
               openConnectionInServerCommunicationStrategyIsCalledFirst;
    }

};


TEST_F(SensorAccessLinkTest, given_noEstablishedConnection_when_starting_then_aValidConnectionIsMade){
    SensorCommunicationStrategyMock sensorCommunicationStrategyMock;
    MessageTranslationStrategyMock messageTranslationStrategyMock;
    ServerCommunicationStrategyMock serverCommunicationStrategyMock;

    SensorAccessLink<SimpleData, SimpleData>  sensorAccessLink(&sensorCommunicationStrategyMock,
                                                               &messageTranslationStrategyMock,
                                                               &serverCommunicationStrategyMock);

    sensorAccessLink.start();
    sensorAccessLink.terminateAndJoin();

    ASSERT_TRUE(connectionIsValid(&sensorCommunicationStrategyMock, &serverCommunicationStrategyMock));
}


#endif //SPIRITSENSORGATEWAY_SENSORCOMMUNICATORTEST_CPP

