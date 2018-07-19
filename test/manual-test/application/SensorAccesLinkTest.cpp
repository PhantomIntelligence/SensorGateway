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
#include "test/utilities/files/SpiritFramesFileManager.h"
#include "spirit-sensor-gateway/application/SensorAccessLink.hpp"

using SensorCommunication::KvaserCanCommunicationStrategy;
using MessageTranslation::AWLMessageToSpiritMessageTranslationStrategy;
using ServerCommunication::ServerCommunicationStrategy;
using TestUtilities::SpiritFramesFileManager;

class ServerCommunicationStrategyMock final : public ServerCommunicationStrategy<Frame> {

public:
    explicit ServerCommunicationStrategyMock() : numberOfReceivedFrames(0), receivedFrames({}){}

    ~ServerCommunicationStrategyMock() = default;

    void openConnection() override {}

    void closeConnection() override {}

    void sendMessage(Frame&& message) override {
        if(numberOfReceivedFrames.load() != MAXIMUM_NUMBER_OF_FRAMES_NEEDED_FOR_TEST){
            receivedFrames.at(numberOfReceivedFrames) = message;
            numberOfReceivedFrames++;
        }
    }

    std::vector<Frame> getReceivedFrames(){
        return receivedFrames;
    }

private:
    int const MAXIMUM_NUMBER_OF_FRAMES_NEEDED_FOR_TEST = 10;
    AtomicCounter numberOfReceivedFrames;
    std::vector<Frame> receivedFrames;
};




int main(){
    KvaserCanCommunicationStrategy sensorCommunicationStrategy;
    AWLMessageToSpiritMessageTranslationStrategy messageTranslationStrategy;
    ServerCommunicationStrategyMock serverCommunicationStrategy;
    SpiritFramesFileManager fileManager;

    //SpiritSensorGateway::SensorAccessLink<AWLMessage, Frame> sensorAccessLink(&sensorCommunicationStrategy, &messageTranslationStrategy, &serverCommunicationStrategy);

    //sensorAccessLink.connect();
    //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    //sensorAccessLink.disconnect();

    //fileManager.writeFileWithMessages(serverCommunicationStrategy.getReceivedFrames(), "SensorAccessLinkOutputFrames.txt");

    return 0;
};
#endif // SPIRITSENSORGATEWAY_ENDTOENDCONNECTIONTEST_CPP
