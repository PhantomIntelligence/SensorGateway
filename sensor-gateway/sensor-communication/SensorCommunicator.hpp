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
#ifndef SENSORGATEWAY_SENSORCOMMUNICATOR_HPP
#define SENSORGATEWAY_SENSORCOMMUNICATOR_HPP

#include "sensor-gateway/common/data-flow/DataProcessingScheduler.hpp"
#include "SensorCommunicationStrategy.hpp"

namespace SensorAccessLinkElement {

    namespace Details {

        template<class T>
        using AsyncRequestBuffer = DataFlow::RingBuffer<typename T::Message, T::ASYNC_REQUEST_BUFFER_SIZE_BEFORE_TRANSMISSION_TO_SENSOR, ONLY_ONE_CONSUMER>;

        template<class T>
        using AsyncRequestBufferLink = DataFlow::ConsumerLink<typename T::Message, T::ASYNC_REQUEST_BUFFER_SIZE_BEFORE_TRANSMISSION_TO_SENSOR, ONLY_ONE_CONSUMER>;
    }

    template<class T>
    class SensorCommunicator : public DataFlow::DataSource<typename T::Message>,
                               public DataFlow::DataSource<typename T::RawData>,
                               public DataFlow::DataSink<typename T::Message>, // SensorRequests
                               public Details::AsyncRequestBufferLink<T>, // SensorRequests
                               public DataFlow::DataSource<ErrorHandling::SensorAccessLinkError> {

    public:

        typedef SensorCommunication::SensorCommunicationStrategy<T> SensorCommunicationStrategy;

    protected:

        using Message = typename T::Message;
        using RawData = typename T::RawData;
        using Request = typename T::Message; //SensorRequests
        using Messages = typename SensorCommunicationStrategy::Messages;
        using RawDataCycles = typename SensorCommunicationStrategy::RawDataCycles;

        using MessageSource = DataFlow::DataSource<Message>;
        using RawDataSource = DataFlow::DataSource<RawData>;
        using RequestSink = DataFlow::DataSink<Message>;
        using ErrorSource = DataFlow::DataSource<ErrorHandling::SensorAccessLinkError>;

        Message const DEFAULT_MESSAGE = T::Message::returnDefaultData();
        RawData const DEFAULT_RAW_DATA = T::RawData::returnDefaultData();

        using AsyncRequestBuffer = Details::AsyncRequestBuffer<T>;
        using ThisClass = SensorCommunicator<T>;

    public:

        using RequestProcessingScheduler = typename DataFlow::DataProcessingScheduler<typename T::Message, ThisClass, ONLY_ONE_PRODUCER, ONLY_ONE_CONSUMER, T::ASYNC_REQUEST_BUFFER_SIZE_BEFORE_TRANSMISSION_TO_SENSOR>;

        explicit SensorCommunicator(SensorCommunicationStrategy* sensorCommunicationStrategy) :
                terminateOrderReceived(false),
                sensorCommunicationStrategy(sensorCommunicationStrategy),
                hasToSendRequest(false),
                communicatorThread(JoinableThread(doNothing)) {
            linkWith(&asyncRequestBuffer);
            communicatorThread.exitSafely();
        }

        ~SensorCommunicator() noexcept = default;

        SensorCommunicator(SensorCommunicator const& other) = delete;

        SensorCommunicator(SensorCommunicator&& other) noexcept = delete;

        SensorCommunicator& operator=(SensorCommunicator const& other)& = delete;

        SensorCommunicator& operator=(SensorCommunicator&& other)& noexcept = delete;

        /**
         * @warning It is important that this function remains not implemented
         */
        void linkWith(AsyncRequestBuffer* asyncRequestBuffer) override {
            asyncRequestBuffer->linkWith(this);
        }

        void activateFor(AsyncRequestBuffer* asyncRequestBuffer) {
            hasToSendRequest.store(true);
        }

        void deactivateFor(AsyncRequestBuffer* asyncRequestBuffer) {
            hasToSendRequest.store(false);
        }

        void start() {
            openConnection();
            yield();
            communicatorThread = JoinableThread(&SensorCommunicator::run, this);
        };

        void consume(Request&& request) override {
            if (!terminateOrderHasBeenReceived()) {
                asyncRequestBuffer.write(std::forward<Request>(request));
            }
        }

        void terminateAndJoin() {
            closeConnection();
            if (!terminateOrderHasBeenReceived()) {
                terminateOrderReceived.store(true);
            }
            communicatorThread.exitSafely();
        };

        using MessageSource::linkConsumer;
        using RawDataSource::linkConsumer;
        using ErrorSource::linkConsumer;

    private:

        void openConnection() noexcept {
            try {
                sensorCommunicationStrategy->openConnection();
            } catch (ErrorHandling::SensorAccessLinkError& strategyError) {
                addOriginAndHandleError(std::move(strategyError),
                                        ErrorHandling::Origin::SENSOR_COMMUNICATOR_OPEN_CONNECTION);
            }
        }

        void closeConnection() noexcept {
            try {
                sensorCommunicationStrategy->closeConnection();
            } catch (ErrorHandling::SensorAccessLinkError& strategyError) {
                addOriginAndHandleError(std::move(strategyError),
                                        ErrorHandling::Origin::SENSOR_COMMUNICATOR_CLOSE_CONNECTION);
            }
        }

        void run() {
            while (!terminateOrderHasBeenReceived()) {
                handleIncomingMessages();
                handleIncomingRawData();
                if (hasToSendRequest.load()) {
                    sendRequestsToSensor();
                }
                yield();
            }
        }

        void handleIncomingMessages() {
            Messages messages;
            try {
                messages = sensorCommunicationStrategy->fetchMessages();
            } catch (ErrorHandling::SensorAccessLinkError& strategyError) {
                addOriginAndHandleError(std::move(strategyError),
                                        ErrorHandling::Origin::SENSOR_COMMUNICATOR_HANDLE_MESSAGE);
            }

            for (auto messageIndex = 0; messageIndex < messages.size(); ++messageIndex) {
                auto message = messages[messageIndex];
                if (message != DEFAULT_MESSAGE) {
                    MessageSource::produce(std::move(message));
                }
            }
        }

        void handleIncomingRawData() {
            RawDataCycles rawDataCycles;
            try {
                rawDataCycles = sensorCommunicationStrategy->fetchRawDataCycles();
            } catch (ErrorHandling::SensorAccessLinkError& strategyError) {
                addOriginAndHandleError(std::move(strategyError),
                                        ErrorHandling::Origin::SENSOR_COMMUNICATOR_HANDLE_RAWDATA);
            }

            for (auto rawDataIndex = 0; rawDataIndex < rawDataCycles.size(); ++rawDataIndex) {
                auto rawDataCycle = rawDataCycles[rawDataIndex];
                if (rawDataCycle != DEFAULT_RAW_DATA) {
                    RawDataSource::produce(std::move(rawDataCycle));
                }
            }
        }

        void sendRequestsToSensor() {
            auto requestToSend = asyncRequestBuffer.consumeNextDataFor(this);
            sensorCommunicationStrategy->sendRequest(std::move(requestToSend));
        }

        void addOriginAndHandleError(ErrorHandling::SensorAccessLinkError&& error, std::string const& originToAdd) {
            auto originAddedError = ErrorHandling::SensorAccessLinkError(
                    originToAdd + ErrorHandling::Message::SEPARATOR + error.getOrigin(),
                    error.getCategory(),
                    error.getSeverity(),
                    error.getErrorCode(),
                    error.getMessage());
            handleCaughtError(std::move(originAddedError));
        }

        void handleCaughtError(ErrorHandling::SensorAccessLinkError&& error) {
            auto errorCopy = ErrorHandling::SensorAccessLinkError(error);
            ErrorSource::produce(std::move(errorCopy));
            if (error.isCloseConnectionRequired()) {
                closeConnection();
            }
            if (error.isOpenConnectionRequired()) {
                openConnection();
            }
        }

        bool terminateOrderHasBeenReceived() const {
            return terminateOrderReceived.load();
        }

        JoinableThread communicatorThread;

        AtomicFlag terminateOrderReceived;

        SensorCommunicationStrategy* sensorCommunicationStrategy;

        AsyncRequestBuffer asyncRequestBuffer;
        AtomicFlag hasToSendRequest;
    };
}

#endif //SENSORGATEWAY_SENSORCOMMUNICATOR_HPP
