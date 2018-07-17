#include <chrono>
#include <thread>
#include <test/mock/FrameSinkMock.h>
#include "spirit-sensor-gateway/application/SensorAccessLink.hpp"
#include "spirit-sensor-gateway/sensor-communication/KvaserCanCommunicationStrategy.h"
#include "spirit-sensor-gateway/message-translation/AWLMessageToSpiritMessageTranslationStrategy.h"
#include "spirit-sensor-gateway/server-communication/UWSServerCommunicationStrategy.h"
#include "test/utilities/files/SpiritFramesFileManager.h"

using SensorCommunication::KvaserCanCommunicationStrategy;
using MessageTranslation::AWLMessageToSpiritMessageTranslationStrategy;
using ServerCommunication::UWSServerCommunicationStrategy;
using TestUtilities::SpiritFramesFileManager;
using Mock::FrameSinkMock;

int main(){

    typedef DataProcessingScheduler<AWLMessage, MessageTranslator<AWLMessage, Frame>, 1> MessageTranslationScheduler;
    typedef DataProcessingScheduler<Frame, ServerCommunicator<Frame>, 1> ServerCommunicationScheduler;

    KvaserCanCommunicationStrategy sensorCommunicationStrategy;
    AWLMessageToSpiritMessageTranslationStrategy messageTranslationStrategy;
    UWSServerCommunicationStrategy serverCommunicationStrategy;
    MessageTranslator<AWLMessage, Frame> messageTranslator(&messageTranslationStrategy);

    MessageTranslationScheduler messageTranslationScheduler(&messageTranslator);

//    SensorAccessLink<AWLMessage, Frame> sensorAccessLink(&sensorCommunicationStrategy, &messageTranslationStrategy, &serverCommunicationStrategy);
//    SpiritFramesFileManager fileManager;
//    const int NUMBER_OF_MILLISECONDS = 5000;
//
//    sensorAccessLink.start();
//    std::this_thread::sleep_for(std::chrono::milliseconds(NUMBER_OF_MILLISECONDS));
//    sensorAccessLink.terminateAndJoin();


};