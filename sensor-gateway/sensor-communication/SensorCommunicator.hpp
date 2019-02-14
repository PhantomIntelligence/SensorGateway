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

    template<class T>
    class SensorCommunicator : public DataFlow::DataSource<typename T::Message>,
                               public DataFlow::DataSource<typename T::RawData>,
                               public DataFlow::DataSource<ErrorHandling::SensorAccessLinkError> {

    protected:

        typedef SensorCommunication::SensorCommunicationStrategy<T> SensorCommunicationStrategy;

        using MESSAGE = typename T::Message;
        using RAW_DATA = typename T::RawData;
        using MESSAGES = typename SensorCommunicationStrategy::Messages;
        using RAW_DATA_CYCLES = typename SensorCommunicationStrategy::RawDataCycles;

        using MessageSource = DataFlow::DataSource<MESSAGE>;
        using RawDataSource = DataFlow::DataSource<RAW_DATA>;
        using ErrorSource = DataFlow::DataSource<ErrorHandling::SensorAccessLinkError>;

        MESSAGE const DEFAULT_MESSAGE = T::Message::returnDefaultData();
        RAW_DATA const DEFAULT_RAW_DATA = T::RawData::returnDefaultData();

    public:
        explicit SensorCommunicator(SensorCommunicationStrategy* sensorCommunicationStrategy) :
                terminateOrderReceived(false),
                sensorCommunicationStrategy(sensorCommunicationStrategy),
                communicatorThread(JoinableThread(doNothing)) {
            communicatorThread.exitSafely();
        }

        ~SensorCommunicator() noexcept = default;

        SensorCommunicator(SensorCommunicator const& other) = delete;

        SensorCommunicator(SensorCommunicator&& other) noexcept = delete;

        SensorCommunicator& operator=(SensorCommunicator const& other)& = delete;

        SensorCommunicator& operator=(SensorCommunicator&& other)& noexcept = delete;

        void start() {
            openConnection();
            yield();
            communicatorThread = JoinableThread(&SensorCommunicator::run, this);
        };

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
                // TODO : investigate the advantages of sleep and/or yield here.
                std::this_thread::yield();
            }
        }

        void handleIncomingMessages() {
            MESSAGES messages;
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
            RAW_DATA_CYCLES rawDataCycles;
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
    };
}

#endif //SENSORGATEWAY_SENSORCOMMUNICATOR_HPP
