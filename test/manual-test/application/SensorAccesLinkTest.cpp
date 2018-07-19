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


#ifndef SPIRITSENSORGATEWAY_ENDTOENDCONNECTIONTEST_CPP
#define SPIRITSENSORGATEWAY_ENDTOENDCONNECTIONTEST_CPP

#include <chrono>
#include <thread>
#include "spirit-sensor-gateway/sensor-communication/KvaserCanCommunicationStrategy.h"
#include "spirit-sensor-gateway/server-communication/ServerCommunicationStrategy.hpp"
#include "spirit-sensor-gateway/message-translation/AWLMessageToSpiritMessageTranslationStrategy.h"
#include "spirit-sensor-gateway/server-communication/UWSServerCommunicationStrategy.h"
#include "test/utilities/files/SpiritFramesFileManager.cpp"
#include "spirit-sensor-gateway/application/SensorAccessLink.hpp"

using SensorCommunication::KvaserCanCommunicationStrategy;
using MessageTranslation::AWLMessageToSpiritMessageTranslationStrategy;
using ServerCommunication::ServerCommunicationStrategy;

class ServerCommunicationStrategyMock final : public ServerCommunicationStrategy<DataFlow::Frame> {

public:
    explicit ServerCommunicationStrategyMock() : numberOfReceivedFrames(0), receivedFrames({}){
    }

    ~ServerCommunicationStrategyMock() = default;

    void openConnection() override {}

    void closeConnection() override {}

    void sendMessage(DataFlow::Frame&& message) override {
        if(numberOfReceivedFrames.load() < MAXIMUM_NUMBER_OF_FRAMES_NEEDED_FOR_TEST){
            std::cout << "Frame received" << std::endl;
            receivedFrames.push_back(message);
        }
        numberOfReceivedFrames++;
    }

    std::vector<DataFlow::Frame> getReceivedFrames(){
        return receivedFrames;
    }

private:
    int const MAXIMUM_NUMBER_OF_FRAMES_NEEDED_FOR_TEST = 10;
    AtomicCounter numberOfReceivedFrames;
    std::vector<DataFlow::Frame> receivedFrames;
};




int main(){
    ServerCommunicationStrategyMock serverCommunicationStrategy;
    AWLMessageToSpiritMessageTranslationStrategy messageTranslationStrategy;
    KvaserCanCommunicationStrategy sensorCommunicationStrategy;
    TestUtilities::SpiritFramesFileManager fileManager;

    SpiritSensorGateway::SensorAccessLink<DataFlow::AWLMessage, DataFlow::Frame> sensorAccessLink(&serverCommunicationStrategy,&messageTranslationStrategy, &sensorCommunicationStrategy);

    sensorAccessLink.connect();
    std::this_thread::sleep_for(std::chrono::milliseconds(10000));
    std::cout<< "About to disconnect"<<std::endl;
    sensorAccessLink.disconnect();

    std::cout << "After Disconnection"<< std::endl;
    std::cout << "Number of frames received : "<< serverCommunicationStrategy.getReceivedFrames().size() << std::endl;
    fileManager.writeFileWithMessages(serverCommunicationStrategy.getReceivedFrames(), "SensorAccessLinkOutputFrames.txt");

    return 0;
};
#endif // SPIRITSENSORGATEWAY_ENDTOENDCONNECTIONTEST_CPP
