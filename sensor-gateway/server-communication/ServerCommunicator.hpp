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

#include "RequestHandler.hpp"
#include "ServerCommunicationStrategy.hpp"

namespace SensorAccessLinkElement {

    template<class T>
    class ServerCommunicator : public DataFlow::DataSink<typename T::Message>,
                               public DataFlow::DataSink<typename T::RawData>,
                               public DataFlow::DataSource<ErrorHandling::SensorAccessLinkError> {
    public:

        using ServerCommunicationStrategy = ServerCommunication::ServerCommunicationStrategy<T>;

    protected:

        using Message = typename T::Message;
        using RawData = typename T::RawData;

        using MessageSink = DataFlow::DataSink<Message>;
        using RawDataSink = DataFlow::DataSink<RawData>;

        using RequestAssembler = Assemble::ServerRequestAssembler;

        using GetParameterValueRequest = ServerCommunication::RequestTypes::GetParameterValue;
        using GetParameterValueContent = typename ServerCommunicationStrategy::GetParameterValueContent;
        using GetParameterValueContents = typename ServerCommunicationStrategy::GetParameterValueContents;

        GetParameterValueRequest const DEFAULT_GET_PARAMETER_VALUE_REQUEST = ServerCommunication::RequestTypes::GetParameterValue::returnDefaultData();

        using HandleGetAllParameterNamesRequest = std::function<void()>;
        using HandleGetParameterValueRequest = std::function<void(GetParameterValueRequest&&)>;
        using HandleCalibrationRequest = std::function<void()>;
        using HandleClearCalibrationRequest = std::function<void()>;

        using ErrorSource = DataFlow::DataSource<ErrorHandling::SensorAccessLinkError>;

    public:

        explicit ServerCommunicator(ServerCommunicationStrategy* serverCommunicationStrategy,
                                    HandleGetAllParameterNamesRequest handleGetAllParameterNamesRequest,
                                    HandleGetParameterValueRequest handleGetParameterValueRequest,
                                    HandleCalibrationRequest handleCalibrationRequest,
                                    HandleClearCalibrationRequest handleClearCalibrationRequest) :
                serverCommunicationStrategy(serverCommunicationStrategy),
                handleGetAllParameterNamesRequest(handleGetAllParameterNamesRequest),
                handleGetParameterValueRequest(handleGetParameterValueRequest),
                handleCalibrationRequest(handleCalibrationRequest),
                handleClearCalibrationRequest(handleClearCalibrationRequest),
                serverConnected(false), // TODO: add logic not to send anything until server is connected
                terminateOrderReceived(false),
                requestReceptionThread(JoinableThread(doNothing)) {
            requestReceptionThread.exitSafely();
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

        // TODO : Add template specialization for non standard Response type so it always throws
        template<typename Response>
        void sendResponse(Response&& response) {
            try {
                serverCommunicationStrategy->sendResponse(std::forward<Response>(response));
            } catch (ErrorHandling::SensorAccessLinkError& strategyError) {
                addOriginAndHandleError(std::move(strategyError),
                                        ErrorHandling::Origin::SERVER_COMMUNICATOR_SEND_RESPONSE);
            }
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
                handleIncomingGetAllParameterNamesRequests();
                handleIncomingGetParameterValueRequests();
                handleIncomingCalibrationRequests();
                handleIncomingClearCalibrationRequests();
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

        void handleIncomingGetAllParameterNamesRequests() {
            auto hasToSendAllParameterNames = serverCommunicationStrategy->hasReceivedGetAllParameterNamesRequest();
            if(hasToSendAllParameterNames) {
                handleGetAllParameterNamesRequest();
            }
        }

        void handleIncomingGetParameterValueRequests() {
            GetParameterValueContents contents;
            try {
                contents = serverCommunicationStrategy->fetchGetParameterValueContents();
            } catch (ErrorHandling::SensorAccessLinkError& strategyError) {
                addOriginAndHandleError(std::move(strategyError),
                                        ErrorHandling::Origin::SERVER_COMMUNICATOR_FETCH_GET_PARAMETER_VALUE);
            }

            // TODO: Add logic to handle requests in bulk instead of one by one
            // TODO: This will be needed to avoid multiple & useless requests for adjacent parameters
            auto requestCount = 0u;
            bool handleRequest = false;
            GetParameterValueRequest request;

            std::tie(request, handleRequest) = assembleGetParameterValueRequestFrom(contents[requestCount++]);
            while (handleRequest) {
                handleGetParameterValueRequest(std::move(request));
                std::tie(request, handleRequest) = assembleGetParameterValueRequestFrom(contents[requestCount++]);
            }
        }

        void handleIncomingCalibrationRequests() {
            auto hasToSendAllParameterNames = serverCommunicationStrategy->hasReceivedCalibrationRequest();
            if(hasToSendAllParameterNames) {
                handleCalibrationRequest();
            }
        }

        void handleIncomingClearCalibrationRequests() {
            auto hasToSendAllParameterNames = serverCommunicationStrategy->hasReceivedClearCalibrationRequest();
            if(hasToSendAllParameterNames) {
                handleClearCalibrationRequest();
            }
        }

        auto assembleGetParameterValueRequestFrom(
                GetParameterValueContent const& content) const -> std::tuple<GetParameterValueRequest, bool> const {
            GetParameterValueRequest getParameterRequest = RequestAssembler::getParameterValueRequest(content);
            bool hasToBeHandled = getParameterRequest != DEFAULT_GET_PARAMETER_VALUE_REQUEST;
            return std::make_tuple(getParameterRequest, hasToBeHandled);
        };


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
        HandleGetAllParameterNamesRequest handleGetAllParameterNamesRequest;
        HandleGetParameterValueRequest handleGetParameterValueRequest;
        HandleCalibrationRequest handleCalibrationRequest;
        HandleClearCalibrationRequest handleClearCalibrationRequest;

        std::string serverAddress;
    };
}

#endif //SENSORGATEWAY_SERVERCOMMUNICATOR_HPP
