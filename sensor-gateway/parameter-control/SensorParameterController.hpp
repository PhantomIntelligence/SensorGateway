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

#ifndef SENSORGATEWAY_SENSORPARAMETERCONTROLLER_HPP
#define SENSORGATEWAY_SENSORPARAMETERCONTROLLER_HPP

#include "sensor-gateway/data-translation/DataTranslator.hpp"
#include "sensor-gateway/server-communication/ServerCommunicator.hpp"

namespace SensorAccessLinkElement {

    template<class SENSOR_STRUCTURES, class SERVER_STRUCTURES>
    class SensorParameterController : public DataFlow::DataSink<typename SENSOR_STRUCTURES::ControlMessage>,
                                      public DataFlow::DataSource<ErrorHandling::SensorAccessLinkError> {

    protected:

        using RequestHandler = SensorAccessLinkElement::RequestHandler<SENSOR_STRUCTURES, SERVER_STRUCTURES>;
        using DataTranslator = SensorAccessLinkElement::DataTranslator<SENSOR_STRUCTURES, SERVER_STRUCTURES>;

        using SensorControlMessage = typename SENSOR_STRUCTURES::ControlMessage;
        using ThisClass = SensorParameterController<SENSOR_STRUCTURES, SERVER_STRUCTURES>;
        using ResponseControlMessageScheduler = DataFlow::DataProcessingScheduler<SensorControlMessage, ThisClass, 1>;

        using ErrorSource = DataFlow::DataSource<ErrorHandling::SensorAccessLinkError>;

        static size_t const ARBITRARILY_BIG_ENOUGH_NUMBER_OF_CONCURRENT_REQUESTS = 256;
        using SensorControlMessages = std::array<SensorControlMessage, ARBITRARILY_BIG_ENOUGH_NUMBER_OF_CONCURRENT_REQUESTS>;
        using SensorControlMessagePointers = Container::ConstantSizedPointerList<SensorControlMessage, ARBITRARILY_BIG_ENOUGH_NUMBER_OF_CONCURRENT_REQUESTS>;


    public:

        explicit SensorParameterController(RequestHandler* requestHandler,
                                           DataTranslator* dataTranslator) :
                requestHandler(requestHandler),
                dataTranslator(dataTranslator),
                responseControlMessageScheduler(this) {}

        ~SensorParameterController() noexcept {
            terminateAndJoin();
        };

        SensorParameterController(SensorParameterController const& other) = delete;

        SensorParameterController(SensorParameterController&& other) noexcept = delete;

        SensorParameterController& operator=(SensorParameterController const& other)& = delete;

        SensorParameterController& operator=(SensorParameterController&& other)& noexcept = delete;

        void linkElements() {
            dataTranslator->linkConsumer(&responseControlMessageScheduler);

            // TODO: implement a while(!allUpAndRunning()) {...
            // Yield and sleep to allow correct connection
            yield();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            // TODO: implement }
        }

        void terminateAndJoin() {
            responseControlMessageScheduler.terminateAndJoin();
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

        RequestHandler* requestHandler;
        DataTranslator* dataTranslator;

        SensorControlMessages sensorControlMessageRequests;
        SensorControlMessagePointers sensorControlMessageRequestPointers;

        ResponseControlMessageScheduler responseControlMessageScheduler;
    };
}

#endif //SENSORGATEWAY_SENSORPARAMETERCONTROLLER_HPP
