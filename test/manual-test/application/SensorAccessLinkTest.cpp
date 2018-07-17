#include "spirit-sensor-gateway/application/SensorAccessLink.hpp"
#include "spirit-sensor-gateway/sensor-communication/KvaserCanCommunicationStrategy.h"
#include "spirit-sensor-gateway/message-translation/AWLMessageToSpiritMessageTranslationStrategy.h"
#include "test/utilities/files/SpiritFramesFileManager.h"
#include <chrono>
#include <thread>

using SensorCommunication::KvaserCanCommunicationStrategy;
using MessageTranslation::AWLMessageToSpiritMessageTranslationStrategy;
using TestUtilities::SpiritFramesFileManager;

int main(){

    KvaserCanCommunicationStrategy communicationStrategy;
    AWLMessageToSpiritMessageTranslationStrategy translationStrategy;
    SensorCommunicator<AWLMessage> sensorCommunicator(&communicationStrategy);
    MessageTranslator<AWLMessage, Frame> messageTranslator(&translationStrategy);
    SensorAccessLink sensorAccessLink(&sensorCommunicator, &messageTranslator);
    SpiritFramesFileManager fileManager;
    const int NUMBER_OF_MILLISECONDS = 5000;

    sensorAccessLink.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(NUMBER_OF_MILLISECONDS));
    sensorAccessLink.terminateAndJoin();


};