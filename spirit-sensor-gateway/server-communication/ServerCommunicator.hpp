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
#ifndef SPIRITSENSORGATEWAY_SERVERCOMMUNICATOR_H
#define SPIRITSENSORGATEWAY_SERVERCOMMUNICATOR_H

#include "spirit-sensor-gateway/common/data-flow/DataProcessingScheduler.hpp"
#include "spirit-sensor-gateway/common/data-flow/DataSink.hpp"
#include "ServerCommunicationStrategy.hpp"

using ServerCommunication::ServerCommunicationStrategy;

namespace SensorAccessLinkElement {

    template<class T>
    class ServerCommunicator : public DataFlow::DataSink<T> {

    protected:

        typedef T MESSAGE;

    public:

        explicit ServerCommunicator(ServerCommunicationStrategy* serverCommunicationProtocolStrategy) :
                serverCommunicationStrategy(serverCommunicationProtocolStrategy),
                communicatorThread(JoinableThread(doNothing)) {
        };

        ~ServerCommunicator() noexcept {};

        ServerCommunicator(ServerCommunicator const& other) = delete;

        ServerCommunicator(ServerCommunicator&& other) noexcept = delete;

        ServerCommunicator& operator=(ServerCommunicator const& other)& = delete;

        ServerCommunicator& operator=(ServerCommunicator&& other)& noexcept = delete;

        void consume(MESSAGE&& message) override {
            serverCommunicationStrategy->sendMessage(message);
        }

        void start() {
            serverCommunicationStrategy->openConnection();
            communicatorThread = JoinableThread(&ServerCommunicator::run, this);
        };


    private:

        void run() {
            while (!terminateOrderHasBeenReceived()) {
                auto message = serverCommunicationStrategy->readMessage();
            }
        }

        bool terminateOrderHasBeenReceived() const {
            return terminateOrderReceived.load();
        }

        AtomicFlag terminateOrderReceived;

        ServerCommunicationStrategy* serverCommunicationStrategy;

        JoinableThread communicatorThread;
    }
}

#endif //SPIRITSENSORGATEWAY_SERVERCOMMUNICATOR_H
