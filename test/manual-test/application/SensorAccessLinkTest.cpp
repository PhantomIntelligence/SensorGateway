#include "spirit-sensor-gateway/application/SensorAccessLink.hpp"
#include "spirit-sensor-gateway/sensor-communication/KvaserCanCommunicationStrategy.h"
#include "spirit-sensor-gateway/message-translation/AWLMessageToSpiritMessageTranslationStrategy.h"
#include "test/utilities/files/SpiritFramesFileManager.h"
#include <chrono>
#include <thread>
#include <test/mock/FrameSinkMock.h>

using SensorCommunication::KvaserCanCommunicationStrategy;
using MessageTranslation::AWLMessageToSpiritMessageTranslationStrategy;
using TestUtilities::SpiritFramesFileManager;
using Mock::FrameSinkMock;

int main(){

    KvaserCanCommunicationStrategy sensorCommunicationStrategy;
    AWLMessageToSpiritMessageTranslationStrategy messageTranslationStrategy;
    FrameSinkMock serverCommunicationStrategy(1);

    SensorAccessLink sensorAccessLink(&sensorCommunicationStrategy, &messageTranslationStrategy,
                                      &serverCommunicationStrategy);
    SpiritFramesFileManager fileManager;
    const int NUMBER_OF_MILLISECONDS = 5000;

    sensorAccessLink.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(NUMBER_OF_MILLISECONDS));
    sensorAccessLink.terminateAndJoin();


};