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

#ifndef SENSORGATEWAY_SERVERCOMMUNICATOR_H
#define SENSORGATEWAY_SERVERCOMMUNICATOR_H

#include "sensor-gateway/data-translation/DataTranslator.hpp"
#include "ServerCommunicationStrategy.hpp"

namespace SensorAccessLinkElement {


    template<class T>
    class ServerCommunicator : public DataFlow::DataSink<typename T::Message>,
                               public DataFlow::DataSink<typename T::RawData> {

    protected:

        using ServerCommunicationStrategy = ServerCommunication::ServerCommunicationStrategy<T>;

        using Message = typename T::Message;
        using RawData = typename T::RawData;

        using MessageSink = DataFlow::DataSink<Message>;
        using RawDataSink = DataFlow::DataSink<RawData>;

    public:

        explicit ServerCommunicator(ServerCommunicationStrategy* serverCommunicationStrategy) :
                serverCommunicationStrategy(serverCommunicationStrategy) {
        };

        ~ServerCommunicator() noexcept = default;

        ServerCommunicator(ServerCommunicator const& other) = delete;

        ServerCommunicator(ServerCommunicator&& other) noexcept = delete;

        ServerCommunicator& operator=(ServerCommunicator const& other)& = delete;

        ServerCommunicator& operator=(ServerCommunicator&& other)& noexcept = delete;

        void connect(std::string const& serverAddress) {
            serverCommunicationStrategy->openConnection(serverAddress);
        };

        void disconnect() {
            serverCommunicationStrategy->closeConnection();
        }

        void consume(Message&& message) override {
            serverCommunicationStrategy->sendMessage(std::forward<Message>(message));
        }

        void consume(RawData&& rawData) override {
            serverCommunicationStrategy->sendRawData(std::forward<RawData>(rawData));
        }

    private:

        ServerCommunicationStrategy* serverCommunicationStrategy;
    };
}

#endif //SENSORGATEWAY_SERVERCOMMUNICATOR_H
