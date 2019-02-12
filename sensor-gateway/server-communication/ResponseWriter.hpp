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

#ifndef SENSORGATEWAY_RESPONSEWRITER_HPP
#define SENSORGATEWAY_RESPONSEWRITER_HPP

#include "sensor-gateway/server-communication/ServerCommunicator.hpp"

namespace SensorAccessLinkElement {

    template<class SENSOR_STRUCTURES, class SERVER_STRUCTURES>
    class ResponseWriter : public DataFlow::DataSink<typename SENSOR_STRUCTURES::ControlMessage>,
                           public DataFlow::DataSource<ErrorHandling::SensorAccessLinkError> {

    protected:

        using ServerCommunicator = SensorAccessLinkElement::ServerCommunicator<SERVER_STRUCTURES>;

        using ThisClass = ResponseWriter<SENSOR_STRUCTURES, SERVER_STRUCTURES>;

        using SensorControlMessage = typename SENSOR_STRUCTURES::ControlMessage;

        using ResponseControlMessageScheduler = DataFlow::DataProcessingScheduler<SensorControlMessage, ThisClass, 1>;

        using ErrorSource = DataFlow::DataSource<ErrorHandling::SensorAccessLinkError>;

        static size_t const ARBITRARILY_BIG_ENOUGH_NUMBER_OF_CONCURRENT_REQUESTS = 128;
        using SensorControlMessages = std::array<SensorControlMessage, ARBITRARILY_BIG_ENOUGH_NUMBER_OF_CONCURRENT_REQUESTS>;
        using SensorControlMessagePointers = Container::ConstantSizedPointerList<SensorControlMessage, ARBITRARILY_BIG_ENOUGH_NUMBER_OF_CONCURRENT_REQUESTS>;

    public:

        explicit ResponseWriter(ServerCommunicator* serverCommunicator) :
                serverCommunicator(serverCommunicator) {}

        ~ResponseWriter() noexcept {};

        ResponseWriter(ResponseWriter const& other) = delete;

        ResponseWriter(ResponseWriter&& other) noexcept = delete;

        ResponseWriter& operator=(ResponseWriter const& other)& = delete;

        ResponseWriter& operator=(ResponseWriter&& other)& noexcept = delete;

        void storeSensorControlMessage(SensorControlMessage sensorControlMessage) {

        }

        void consume(SensorControlMessage&& sensorControlMessageResponse) override {
            /**
             *  TODO:
             *  - Create schedulers and link'em in SensorAccessLink
             *  - receive and process ControlMessage // from sensor
             *  - transmit to server w/ ServerCommunicator*
             */

        }

        using ErrorSource::linkConsumer;

    private:

        ServerCommunicator* serverCommunicator;

        SensorControlMessages sensorControlMessageRequests;
        SensorControlMessagePointers sensorControlMessageRequestPointers;
    };
}

#endif //SENSORGATEWAY_RESPONSEWRITER_HPP
