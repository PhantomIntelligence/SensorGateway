/**
	Copyright 2014-2019 Phantom Intelligence Inc.

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

#ifndef SENSORGATEWAY_SERVERCOMMUNICATOR_HPP
#define SENSORGATEWAY_SERVERCOMMUNICATOR_HPP

#include "sensor-gateway/data-translation/DataTranslator.hpp"
#include "ServerCommunicationStrategy.hpp"
#include "ServerResponseAssembler.hpp"

namespace SensorAccessLinkElement {

    template<class T>
    class ServerCommunicator : public DataFlow::DataSink<typename T::Message>,
                               public DataFlow::DataSink<typename T::RawData>,
                               public DataFlow::DataSource<ErrorHandling::SensorAccessLinkError> {

    protected:

        using ServerCommunicationStrategy = ServerCommunication::ServerCommunicationStrategy<T>;

        using Message = typename T::Message;
        using RawData = typename T::RawData;

        using MessageSink = DataFlow::DataSink<Message>;
        using RawDataSink = DataFlow::DataSink<RawData>;
        using Parameters = typename T::Parameters;

        using ErrorSource = DataFlow::DataSource<ErrorHandling::SensorAccessLinkError>;

    public:

        explicit ServerCommunicator(ServerCommunicationStrategy* serverCommunicationStrategy) :
                serverCommunicationStrategy(serverCommunicationStrategy),
                serverConnected(false), // TODO: add logic not to send anything untill server is connected
                terminateOrderReceived(false),
                requestReceptionThread(JoinableThread(doNothing)) {
        };

        ~ServerCommunicator() noexcept = default;

        ServerCommunicator(ServerCommunicator const& other) = delete;

        ServerCommunicator(ServerCommunicator&& other) noexcept = delete;

        ServerCommunicator& operator=(ServerCommunicator const& other)& = delete;

        ServerCommunicator& operator=(ServerCommunicator&& other)& noexcept = delete;

        void openConnection(std::string const& serverAddress) {
            this->serverAddress = serverAddress;
            try {
                serverCommunicationStrategy->openConnection(serverAddress);
                serverConnected.store(true);
                yield();
                requestReceptionThread = JoinableThread(&ServerCommunicator::run, this);
            } catch (ErrorHandling::SensorAccessLinkError& strategyError) {
                addOriginAndHandleError(std::move(strategyError),
                                        ErrorHandling::Origin::SERVER_COMMUNICATOR_OPEN_CONNECTION);
            }
        };

        void consume(Message&& message) override {
            try {
                message.addTimePointForGatewayWithLocation(Metrics::LocationNames::SERVER_COMMUNICATOR_SENDING);
                serverCommunicationStrategy->sendMessage(std::move(message));
            } catch (ErrorHandling::SensorAccessLinkError& strategyError) {
                addOriginAndHandleError(std::move(strategyError),
                                        ErrorHandling::Origin::SERVER_COMMUNICATOR_SEND_MESSAGE);
            }
        }

        void consume(RawData&& rawData) override {
            try {
                serverCommunicationStrategy->sendRawData(std::forward<RawData>(rawData));
            } catch (ErrorHandling::SensorAccessLinkError& strategyError) {
                addOriginAndHandleError(std::move(strategyError),
                                        ErrorHandling::Origin::SERVER_COMMUNICATOR_SEND_RAWDATA);
            }
        }

        // TODO : add logic to handle behaviors when server is not connected
        bool const isServerConnected() const noexcept {
            return serverConnected.load();
        }

        void terminateAndJoin() {
            closeConnection();
            if (!terminateOrderHasBeenReceived()) {
                terminateOrderReceived.store(true);
            }
            requestReceptionThread.exitSafely();
        };

        using ErrorSource::linkConsumer;

    private:

        void run() {
            while (!terminateOrderHasBeenReceived()) {
                serverCommunicationStrategy->fetchGetParameterValueContents();
                sleepForTenthOfASecond();
                yield();
            }
        }

        void closeConnection() {
            try {
                serverCommunicationStrategy->closeConnection();
                serverConnected.store(false);
            } catch (ErrorHandling::SensorAccessLinkError& strategyError) {
                addOriginAndHandleError(std::move(strategyError),
                                        ErrorHandling::Origin::SERVER_COMMUNICATOR_CLOSE_CONNECTION);
            }
        }

        void addOriginAndHandleError(ErrorHandling::SensorAccessLinkError&& error, std::string const& originToAdd) {
            auto originAddedError = ErrorHandling::SensorAccessLinkError(
                    originToAdd + ErrorHandling::Message::SEPARATOR + error.getOrigin(),
                    error.getCategory(),
                    error.getSeverity(),
                    error.getErrorCode(),
                    error.getMessage());
            handleError(std::move(originAddedError));
        }

        void handleError(ErrorHandling::SensorAccessLinkError&& error) noexcept {
            auto errorCopy = ErrorHandling::SensorAccessLinkError(error);
            ErrorSource::produce(std::move(errorCopy));
            if (error.isCloseConnectionRequired()) {
                serverCommunicationStrategy->closeConnection();
            }
            if (error.isOpenConnectionRequired()) {
                serverCommunicationStrategy->openConnection(serverAddress);
            }
        }

        bool terminateOrderHasBeenReceived() const {
            return terminateOrderReceived.load();
        }

        JoinableThread requestReceptionThread;

        AtomicFlag serverConnected;
        AtomicFlag terminateOrderReceived;

        ServerCommunicationStrategy* serverCommunicationStrategy;
        std::string serverAddress;

    };
}

#endif //SENSORGATEWAY_SERVERCOMMUNICATOR_HPP
