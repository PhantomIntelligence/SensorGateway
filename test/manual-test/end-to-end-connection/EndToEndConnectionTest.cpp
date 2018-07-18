#ifndef SPIRITSENSORGATEWAY_ENDTOENDCONNECTIONTEST_CPP
#define SPIRITSENSORGATEWAY_ENDTOENDCONNECTIONTEST_CPP

#include "spirit-sensor-gateway/application/SensorAccessLink.hpp"
#include "spirit-sensor-gateway/sensor-communication/KvaserCanCommunicationStrategy.h"
#include "spirit-sensor-gateway/message-translation/AWLMessageToSpiritMessageTranslationStrategy.h"
#include "test/utilities/files/SpiritFramesFileManager.h"
#include <chrono>
#include <thread>
#include <test/utilities/mock/FrameSinkMock.h>

using SensorCommunication::KvaserCanCommunicationStrategy;
using MessageTranslation::AWLMessageToSpiritMessageTranslationStrategy;
using TestUtilities::SpiritFramesFileManager;
using Mock::FrameSinkMock;

int main(){

    KvaserCanCommunicationStrategy sensorCommunicationStrategy;
    AWLMessageToSpiritMessageTranslationStrategy messageTranslationStrategy;
    FrameSinkMock serverCommunicationStrategy(100);

//    SpiritSensorGateway::SensorAccessLink<AWLMessage, Frame> sensorAccessLink(&sensorCommunicationStrategy, &messageTranslationStrategy, &serverCommunicationStrategy);
//    SpiritFramesFileManager fileManager;
//    const int NUMBER_OF_MILLISECONDS = 5000;
//
//    sensorAccessLink.start();
//    std::this_thread::sleep_for(std::chrono::milliseconds(NUMBER_OF_MILLISECONDS));
//    sensorAccessLink.terminateAndJoin();


};
#endif // SPIRITSENSORGATEWAY_ENDTOENDCONNECTIONTEST_CPP
