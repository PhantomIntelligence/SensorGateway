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

#ifndef SPIRITSENSORGATEWAY_APPLICATION_CPP
#define SPIRITSENSORGATEWAY_APPLICATION_CPP

#include "spirit-sensor-gateway/application/SensorAccessLink.hpp"
#include "spirit-sensor-gateway/sensor-communication/KvaserCanCommunicationStrategy.h"
#include "spirit-sensor-gateway/message-translation/AWLMessageToSpiritMessageTranslationStrategy.h"
#include "spirit-sensor-gateway/server-communication/UWSServerCommunicationStrategy.h"

int main() {
    std::string const SERVER_ADDRESS = "ws://localhost:8080/connect-gateway";

    using SensorCommunicationStrategy = SensorCommunication::KvaserCanCommunicationStrategy;
    using MessageTranslationStrategy = MessageTranslation::AWLMessageToSpiritMessageTranslationStrategy;
    using ServerCommunicationStrategy = ServerCommunication::UWSServerCommunicationStrategy;

    using SensorMessageType = DataFlow::AWLMessage;
    using SpiritMessageType = DataFlow::Frame;
    using SensorAccessLink = SpiritSensorGateway::SensorAccessLink<SensorMessageType, SpiritMessageType>;

    SensorCommunicationStrategy sensorCommunicationStrategy;
    MessageTranslationStrategy messageTranslationStrategy;
    ServerCommunicationStrategy serverCommunicationStrategy;

    try {
        SensorAccessLink sensorAccessLink(&serverCommunicationStrategy, &messageTranslationStrategy,
                                          &sensorCommunicationStrategy);

        sensorAccessLink.connect(SERVER_ADDRESS);

        std::this_thread::sleep_for(std::chrono::seconds(60));

        sensorAccessLink.disconnect();
    } catch (std::exception& e) {
        std::cout << "e.what() : " << e.what() << std::endl;
    }

    return 0;
}

#endif // SPIRITSENSORGATEWAY_APPLICATION_CPP
