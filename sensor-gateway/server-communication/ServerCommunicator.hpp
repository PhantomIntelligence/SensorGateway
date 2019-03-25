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
        using SetUnsignedIntegerParameterValueRequest = ServerCommunication::RequestTypes::SetUnsignedIntegerParameterValue;
        using SetSignedIntegerParameterValueRequest = ServerCommunication::RequestTypes::SetSignedIntegerParameterValue;
        using SetRealNumberParameterValueRequest = ServerCommunication::RequestTypes::SetRealNumberParameterValue;
        using SetBooleanParameterValueRequest = ServerCommunication::RequestTypes::SetBooleanParameterValue;

        using ParameterName = typename ServerCommunicationStrategy::ParameterName;
        using GetParameterValueContents = typename ServerCommunicationStrategy::GetParameterValueContentBuffer::Contents;
        using SetUnsignedIntegerParameterValueContent = typename ServerCommunicationStrategy::SetUnsignedIntegerParameterValueContentBuffer::Content;
        using SetUnsignedIntegerParameterValueContents = typename ServerCommunicationStrategy::SetUnsignedIntegerParameterValueContentBuffer::Contents;
        using SetSignedIntegerParameterValueContent = typename ServerCommunicationStrategy::SetSignedIntegerParameterValueContentBuffer::Content;
        using SetSignedIntegerParameterValueContents = typename ServerCommunicationStrategy::SetSignedIntegerParameterValueContentBuffer::Contents;
        using SetRealNumberParameterValueContent = typename ServerCommunicationStrategy::SetRealNumberParameterValueContentBuffer::Content;
        using SetRealNumberParameterValueContents = typename ServerCommunicationStrategy::SetRealNumberParameterValueContentBuffer::Contents;
        using SetBooleanParameterValueContent = typename ServerCommunicationStrategy::SetBooleanParameterValueContentBuffer::Content;
        using SetBooleanParameterValueContents = typename ServerCommunicationStrategy::SetBooleanParameterValueContentBuffer::Contents;

        GetParameterValueRequest const DEFAULT_GET_PARAMETER_VALUE_REQUEST = ServerCommunication::RequestTypes::GetParameterValue::returnDefaultData();
        SetUnsignedIntegerParameterValueRequest const DEFAULT_SET_UNSIGNED_INTEGER_PARAMETER_VALUE_REQUEST = ServerCommunication::RequestTypes::SetUnsignedIntegerParameterValue::returnDefaultData();
        SetSignedIntegerParameterValueRequest const DEFAULT_SET_SIGNED_INTEGER_PARAMETER_VALUE_REQUEST = ServerCommunication::RequestTypes::SetSignedIntegerParameterValue::returnDefaultData();
        SetRealNumberParameterValueRequest const DEFAULT_SET_REAL_NUMBER_PARAMETER_VALUE_REQUEST = ServerCommunication::RequestTypes::SetRealNumberParameterValue::returnDefaultData();
        SetBooleanParameterValueRequest const DEFAULT_SET_BOOLEAN_PARAMETER_VALUE_REQUEST = ServerCommunication::RequestTypes::SetBooleanParameterValue::returnDefaultData();

        using ErrorSource = DataFlow::DataSource<ErrorHandling::SensorAccessLinkError>;

    public:

        using HandleGetAllParameterNamesRequest              = StringLiteral<decltype("HandleGetAllParameterNamesRequest"_ToString)>;
        using HandleGetParameterValueRequest                 = StringLiteral<decltype("HandleGetParameterValueRequest"_ToString)>;
        using HandleSetUnsignedIntegerParameterValueRequest  = StringLiteral<decltype("HandleSetUnsignedIntegerParameterValueRequest"_ToString)>;
        using HandleSetSignedIntegerParameterValueRequest    = StringLiteral<decltype("HandleSetSignedIntegerParameterValueRequest"_ToString)>;
        using HandleSetRealNumberParameterValueRequest       = StringLiteral<decltype("HandleSetRealNumberParameterValueRequest"_ToString)>;
        using HandleSetBooleanParameterValueRequest          = StringLiteral<decltype("HandleSetBooleanParameterValueRequest"_ToString)>;
        using HandleCalibrationRequest                       = StringLiteral<decltype("HandleCalibrationRequest"_ToString)>;
        using HandleClearCalibrationRequest                  = StringLiteral<decltype("HandleClearCalibrationRequest"_ToString)>;

        using RequestCallBackStore =
        typename CallBack
                <
                        std::function<void()>,
                        std::function<void(GetParameterValueRequest&&)>,
                        std::function<void(SetUnsignedIntegerParameterValueRequest&&)>,
                        std::function<void(SetSignedIntegerParameterValueRequest&&)>,
                        std::function<void(SetRealNumberParameterValueRequest&&)>,
                        std::function<void(SetBooleanParameterValueRequest&&)>,
                        std::function<void()>,
                        std::function<void()>
                >
        ::UsingArgument
                <
                        EmptyDescription,
                        GetParameterValueRequest&&,
                        SetUnsignedIntegerParameterValueRequest&&,
                        SetSignedIntegerParameterValueRequest&&,
                        SetRealNumberParameterValueRequest&&,
                        SetBooleanParameterValueRequest&&,
                        EmptyDescription,
                        EmptyDescription
                >
        ::Named<
                HandleGetAllParameterNamesRequest,
                HandleGetParameterValueRequest,
                HandleSetUnsignedIntegerParameterValueRequest,
                HandleSetSignedIntegerParameterValueRequest,
                HandleSetRealNumberParameterValueRequest,
                HandleSetBooleanParameterValueRequest,
                HandleCalibrationRequest,
                HandleClearCalibrationRequest
        >;


        using RequestHandlingCallBacks = typename RequestCallBackStore::InstancesTuple;

        explicit ServerCommunicator(ServerCommunicationStrategy* serverCommunicationStrategy,
                                    RequestHandlingCallBacks* requestHandlingCallBacks) :
                serverCommunicationStrategy(serverCommunicationStrategy),
                requestHandlingCallBacks(requestHandlingCallBacks),
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
                handleIncomingSetUnsignedIntegerParameterValueRequests();
                handleIncomingSetSignedIntegerParameterValueRequests();
                handleIncomingSetRealNumberParameterValueRequests();
                handleIncomingSetBooleanParameterValueRequests();
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
            if (hasToSendAllParameterNames) {
                getCallBack<HandleGetAllParameterNamesRequest>()();
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
                getCallBack<HandleGetParameterValueRequest>()(std::move(request));
                std::tie(request, handleRequest) = assembleGetParameterValueRequestFrom(contents[requestCount++]);
            }
        }

        void handleIncomingSetUnsignedIntegerParameterValueRequests() {
            SetUnsignedIntegerParameterValueContents contents;
            try {
                contents = serverCommunicationStrategy->fetchSetUnsignedIntegerParameterValueContents();
            } catch (ErrorHandling::SensorAccessLinkError& strategyError) {
                addOriginAndHandleError(std::move(strategyError),
                                        ErrorHandling::Origin::SERVER_COMMUNICATOR_FETCH_SET_UNSIGNED_INTEGER_PARAMETER_VALUE);
            }

            auto requestCount = 0u;
            bool handleRequest = false;
            SetUnsignedIntegerParameterValueRequest request;

            std::tie(request, handleRequest) = assembleSetUnsignedIntegerParameterValueRequestFrom(
                    contents[requestCount++]);
            while (handleRequest) {
                getCallBack<HandleSetUnsignedIntegerParameterValueRequest>()(std::move(request));
                std::tie(request, handleRequest) = assembleSetUnsignedIntegerParameterValueRequestFrom(
                        contents[requestCount++]);
            }
        }

        void handleIncomingSetSignedIntegerParameterValueRequests() {
            SetSignedIntegerParameterValueContents contents;
            try {
                contents = serverCommunicationStrategy->fetchSetSignedIntegerParameterValueContents();
            } catch (ErrorHandling::SensorAccessLinkError& strategyError) {
                addOriginAndHandleError(std::move(strategyError),
                                        ErrorHandling::Origin::SERVER_COMMUNICATOR_FETCH_SET_SIGNED_INTEGER_PARAMETER_VALUE);
            }

            auto requestCount = 0u;
            bool handleRequest = false;
            SetSignedIntegerParameterValueRequest request;

            std::tie(request, handleRequest) = assembleSetSignedIntegerParameterValueRequestFrom(
                    contents[requestCount++]);
            while (handleRequest) {
                getCallBack<HandleSetSignedIntegerParameterValueRequest>()(std::move(request));
                std::tie(request, handleRequest) = assembleSetSignedIntegerParameterValueRequestFrom(
                        contents[requestCount++]);
            }
        }

        void handleIncomingSetRealNumberParameterValueRequests() {
            SetRealNumberParameterValueContents contents;
            try {
                contents = serverCommunicationStrategy->fetchSetRealNumberParameterValueContents();
            } catch (ErrorHandling::SensorAccessLinkError& strategyError) {
                addOriginAndHandleError(std::move(strategyError),
                                        ErrorHandling::Origin::SERVER_COMMUNICATOR_FETCH_SET_REAL_NUMBER_PARAMETER_VALUE);
            }

            auto requestCount = 0u;
            bool handleRequest = false;
            SetRealNumberParameterValueRequest request;

            std::tie(request, handleRequest) = assembleSetRealNumberParameterValueRequestFrom(
                    contents[requestCount++]);
            while (handleRequest) {
                getCallBack<HandleSetRealNumberParameterValueRequest>()(std::move(request));
                std::tie(request, handleRequest) = assembleSetRealNumberParameterValueRequestFrom(
                        contents[requestCount++]);
            }
        }

        void handleIncomingSetBooleanParameterValueRequests() {
            SetBooleanParameterValueContents contents;
            try {
                contents = serverCommunicationStrategy->fetchSetBooleanParameterValueContents();
            } catch (ErrorHandling::SensorAccessLinkError& strategyError) {
                addOriginAndHandleError(std::move(strategyError),
                                        ErrorHandling::Origin::SERVER_COMMUNICATOR_FETCH_SET_BOOLEAN_PARAMETER_VALUE);
            }

            auto requestCount = 0u;
            bool handleRequest = false;
            SetBooleanParameterValueRequest request;

            std::tie(request, handleRequest) = assembleSetBooleanParameterValueRequestFrom(
                    contents[requestCount++]);
            while (handleRequest) {
                getCallBack<HandleSetBooleanParameterValueRequest>()(std::move(request));
                std::tie(request, handleRequest) = assembleSetBooleanParameterValueRequestFrom(
                        contents[requestCount++]);
            }
        }


        void handleIncomingCalibrationRequests() {
            auto hasToSendAllParameterNames = serverCommunicationStrategy->hasReceivedCalibrationRequest();
            if (hasToSendAllParameterNames) {
                getCallBack<HandleCalibrationRequest>()();
            }
        }

        void handleIncomingClearCalibrationRequests() {
            auto hasToSendAllParameterNames = serverCommunicationStrategy->hasReceivedClearCalibrationRequest();
            if (hasToSendAllParameterNames) {
                getCallBack<HandleClearCalibrationRequest>()();
            }
        }

        auto assembleGetParameterValueRequestFrom(ParameterName const& content) const
        -> std::tuple<GetParameterValueRequest, bool> const {
            GetParameterValueRequest getParameterRequest =
                    RequestAssembler::getParameterValueRequest(content);
            bool hasToBeHandled = getParameterRequest != DEFAULT_GET_PARAMETER_VALUE_REQUEST;
            return std::make_tuple(getParameterRequest, hasToBeHandled);
        };

        auto assembleSetUnsignedIntegerParameterValueRequestFrom(SetUnsignedIntegerParameterValueContent const& content) const
        -> std::tuple<SetUnsignedIntegerParameterValueRequest, bool> const {
            SetUnsignedIntegerParameterValueRequest setUnsignedIntegerParameterValueRequest =
                    RequestAssembler::setUnsignedIntegerParameterValueRequest(content);
            bool hasToBeHandled = setUnsignedIntegerParameterValueRequest != DEFAULT_SET_UNSIGNED_INTEGER_PARAMETER_VALUE_REQUEST;
            return std::make_tuple(setUnsignedIntegerParameterValueRequest, hasToBeHandled);
        };

        auto assembleSetSignedIntegerParameterValueRequestFrom(SetSignedIntegerParameterValueContent const& content) const
        -> std::tuple<SetSignedIntegerParameterValueRequest, bool> const {
            SetSignedIntegerParameterValueRequest setSignedIntegerParameterValueRequest =
                    RequestAssembler::setSignedIntegerParameterValueRequest(content);
            bool hasToBeHandled = setSignedIntegerParameterValueRequest != DEFAULT_SET_SIGNED_INTEGER_PARAMETER_VALUE_REQUEST;
            return std::make_tuple(setSignedIntegerParameterValueRequest, hasToBeHandled);
        };

        auto assembleSetRealNumberParameterValueRequestFrom(SetRealNumberParameterValueContent const& content) const
        -> std::tuple<SetRealNumberParameterValueRequest, bool> const {
            SetRealNumberParameterValueRequest setRealNumberParameterValueRequest =
                    RequestAssembler::setRealNumberParameterValueRequest(content);
            bool hasToBeHandled = setRealNumberParameterValueRequest != DEFAULT_SET_REAL_NUMBER_PARAMETER_VALUE_REQUEST;
            return std::make_tuple(setRealNumberParameterValueRequest, hasToBeHandled);
        };

        auto assembleSetBooleanParameterValueRequestFrom(SetBooleanParameterValueContent const& content) const
        -> std::tuple<SetBooleanParameterValueRequest, bool> const {
            SetBooleanParameterValueRequest setBooleanParameterValueRequest =
                    RequestAssembler::setBooleanParameterValueRequest(content);
            bool hasToBeHandled = setBooleanParameterValueRequest != DEFAULT_SET_BOOLEAN_PARAMETER_VALUE_REQUEST;
            return std::make_tuple(setBooleanParameterValueRequest, hasToBeHandled);
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

        template<typename Name>
        constexpr auto getCallBack() const noexcept -> decltype(RequestCallBackStore::getInstanceNamed<Name>(
                std::declval<RequestHandlingCallBacks&>())) {
            RequestHandlingCallBacks& constantReferenceToCallBacks = *requestHandlingCallBacks;
            return RequestCallBackStore::getInstanceNamed<Name>(constantReferenceToCallBacks);
        }

        JoinableThread requestReceptionThread;

        AtomicFlag serverConnected;
        AtomicFlag terminateOrderReceived;

        ServerCommunicationStrategy* serverCommunicationStrategy;
        RequestHandlingCallBacks* requestHandlingCallBacks;

        std::string serverAddress;

        // TODO : Refactor this outside and make ServerCommunicator receive only callbacks

        // Fetch requests in strategy
        using FetchGetAllParameterNamesRequest              = StringLiteral<decltype("FetchGetAllParameterNamesRequest"_ToString)>;
        using FetchGetParameterValueRequest                 = StringLiteral<decltype("FetchGetParameterValueRequest"_ToString)>;
        using FetchSetUnsignedIntegerParameterValueRequest  = StringLiteral<decltype("FetchSetUnsignedIntegerParameterValueRequest"_ToString)>;
        using FetchSetSignedIntegerParameterValueRequest    = StringLiteral<decltype("FetchSetSignedIntegerParameterValueRequest"_ToString)>;
        using FetchSetRealNumberParameterValueRequest       = StringLiteral<decltype("FetchSetRealNumberParameterValueRequest"_ToString)>;
        using FetchSetBooleanParameterValueRequest          = StringLiteral<decltype("FetchSetBooleanParameterValueRequest"_ToString)>;
        using FetchCalibrationRequest                       = StringLiteral<decltype("FetchCalibrationRequest"_ToString)>;
        using FetchClearCalibrationRequest                  = StringLiteral<decltype("FetchClearCalibrationRequest"_ToString)>;

        using FetchRequestCallBackStore =
        typename CallBack
                <
                        std::function<void()>,
                        std::function<void(GetParameterValueRequest&&)>,
                        std::function<void()>,
                        std::function<void()>
                >
        ::UsingArgument
                <
                        EmptyDescription,
                        GetParameterValueRequest&&,
                        EmptyDescription,
                        EmptyDescription
                >
        ::Named<
                HandleGetAllParameterNamesRequest,
                HandleGetParameterValueRequest,
                HandleCalibrationRequest,
                HandleClearCalibrationRequest
        >;
    };
}

#endif //SENSORGATEWAY_SERVERCOMMUNICATOR_HPP
