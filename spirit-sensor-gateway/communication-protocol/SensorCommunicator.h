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

*/#ifndef SPIRITSENSORGATEWAY_SENSORCOMMUNICATOR_H
#define SPIRITSENSORGATEWAY_SENSORCOMMUNICATOR_H

#include <spirit-sensor-gateway/common/DataSource.h>
#include "CommunicationProtocolStrategy.h"

namespace SensorAccessLinkElement {

    class SensorCommunicator : public DataFlow::DataSource<AWLMessage> {
    public:
        DataSource() = default;
        ~DataSource() = default;




        sensorCommunicationStrategy(communicationStrategy), schedulerThread(JoinableThread(voidAction)) {
            schedulerThread.safeExit();
            schedulerThread = JoinableThread(&WorkScheduler::startWorkScheduler, this);
        };

        void execute() {
            while() { //appelle le thread dana sle boucle while pour permettre de le lancer tant que c est pas fini
                auto sensorMessage = sensorCommunicationStrategy->readMessage();
                outputBuffer.write(std::move(sensorMessage));
                break;
            }
        }

    private:
        SensorMessageTranslation::CommunicationProtocolStrategy<TypeOut>* sensorCommunicationStrategy;




    };


}

#endif //SPIRITSENSORGATEWAY_SENSORCOMMUNICATOR_H
