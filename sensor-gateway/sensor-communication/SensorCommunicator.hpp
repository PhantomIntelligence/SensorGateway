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
            sensorCommunicationStrategy->openConnection();
            communicatorThread = JoinableThread(&SensorCommunicator::run, this);
        };

        void terminateAndJoin() {
            sensorCommunicationStrategy->closeConnection();
            if (!terminateOrderHasBeenReceived()) {
                terminateOrderReceived.store(true);
            }
            communicatorThread.exitSafely();
        };

        using MessageSource::linkConsumer;

        using RawDataSource::linkConsumer;

        using ErrorSource::linkConsumer;

    private:

        void run() {
            while (!terminateOrderHasBeenReceived()) {
                try {
                    handleIncomingMessages();
                    handleIncomingRawData();
                    // TODO : investigate the avantages of sleep and/or yield here.
                    std::this_thread::yield();
                } catch (ErrorHandling::SensorAccessLinkError& error) {
                    if (error.isCloseConnectionRequired()) {
                        sensorCommunicationStrategy->closeConnection();
                    }
                    if (error.isOpenConnectionRequired()) {
                        sensorCommunicationStrategy->openConnection();
                    }
                    ErrorSource::produce(std::move(error));
                }
            }
        }

        void handleIncomingMessages() {
            auto messages = sensorCommunicationStrategy->fetchMessages();
            for (auto messageIndex = 0; messageIndex < messages.size(); ++messageIndex) {
                auto message = messages[messageIndex];
                if (message != DEFAULT_MESSAGE) {
                    MessageSource::produce(std::move(message));
                }
            }
        }

        void handleIncomingRawData() {
            auto rawDataCycles = sensorCommunicationStrategy->fetchRawDataCycles();
            for (auto rawDataIndex = 0; rawDataIndex < rawDataCycles.size(); ++rawDataIndex) {
                auto rawDataCycle = rawDataCycles[rawDataIndex];
                if (rawDataCycle != DEFAULT_RAW_DATA) {
                    RawDataSource::produce(std::move(rawDataCycle));
                }
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
