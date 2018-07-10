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
#ifndef SPIRITSENSORGATEWAY_SENSORCOMMUNICATOR_HPP
#define SPIRITSENSORGATEWAY_SENSORCOMMUNICATOR_HPP

#include "spirit-sensor-gateway/common/data-flow/DataProcessingScheduler.hpp"
#include "SensorCommunicationStrategy.hpp"

namespace SensorAccessLinkElement {

    template<class T>
    class SensorCommunicator : public DataFlow::DataSource<T> {

    protected:
        typedef T DATA;
        typedef SensorCommunication::SensorCommunicationStrategy<DATA> CommunicationProtocolStrategy;

        using super = DataFlow::DataSource<DATA>;
        using super::produce;

    public:
        explicit SensorCommunicator(CommunicationProtocolStrategy* communicationProtocolStrategy) :
                terminateOrderReceived(false),
                communicationProtocolStrategy(communicationProtocolStrategy),
                communicatorThread(JoinableThread(voidAction)) {
            communicatorThread.exitSafely();
        }

        ~SensorCommunicator() {
        };

        void start() {
            communicationProtocolStrategy->openConnection();
            communicatorThread = JoinableThread(&SensorCommunicator::run, this);
        };

        void terminateAndJoin() {
            communicationProtocolStrategy->closeConnection();

            if (!terminateOrderHasBeenReceived()) {
                terminateOrderReceived.store(true);
            }

            communicatorThread.exitSafely();
        };


    private:

        void run() {
            while (!terminateOrderHasBeenReceived()) {
                auto message = communicationProtocolStrategy->readMessage();
                produce(std::move(message));
            }
        }

        bool terminateOrderHasBeenReceived() const {
            return terminateOrderReceived.load();
        }

        JoinableThread communicatorThread;

        AtomicFlag terminateOrderReceived;

        CommunicationProtocolStrategy* communicationProtocolStrategy;
    };
}

#endif //SPIRITSENSORGATEWAY_SENSORCOMMUNICATOR_HPP
