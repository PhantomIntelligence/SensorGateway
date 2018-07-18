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
#include "spirit-sensor-gateway/message-translation/AWLMessageToSpiritMessageTranslationStrategy.h"
#include "spirit-sensor-gateway/server-communication/UWSServerCommunicationStrategy.h"
#include "test/utilities/files/SpiritFramesFileManager.h"

using SensorCommunication::KvaserCanCommunicationStrategy;
using MessageTranslation::AWLMessageToSpiritMessageTranslationStrategy;
using ServerCommunication::UWSServerCommunicationStrategy;
using TestUtilities::SpiritFramesFileManager;

int main(){
    KvaserCanCommunicationStrategy sensorCommunicationStrategy;
    AWLMessageToSpiritMessageTranslationStrategy messageTranslationStrategy;
    UWSServerCommunicationStrategy serverCommunicationStrategy;


//    SpiritSensorGateway::SensorAccessLink<AWLMessage, Frame> sensorAccessLink(&sensorCommunicationStrategy, &messageTranslationStrategy, &serverCommunicationStrategy);

//    SpiritFramesFileManager fileManager;
//    const int NUMBER_OF_MILLISECONDS = 5000;
//
//    sensorAccessLink.start();
//    std::this_thread::sleep_for(std::chrono::milliseconds(NUMBER_OF_MILLISECONDS));
//    sensorAccessLink.terminateAndJoin();


};
#endif // SPIRITSENSORGATEWAY_ENDTOENDCONNECTIONTEST_CPP
