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

    /**
     * @note It is expected that Request entering in this class WILL NOT create any error in the SensorAccessLink execution
     */
    template<class SENSOR_STRUCTURES, class SERVER_STRUCTURES>
    class SensorParameterController : public DataFlow::DataSink<typename SENSOR_STRUCTURES::ControlMessage>,
                                      public DataFlow::DataSource<ErrorHandling::SensorAccessLinkError> {

    protected:

        using RequestHandler = SensorAccessLinkElement::RequestHandler<SENSOR_STRUCTURES, SERVER_STRUCTURES>;
        using DataTranslator = SensorAccessLinkElement::DataTranslator<SENSOR_STRUCTURES, SERVER_STRUCTURES>;

        using SensorControlMessage = typename SENSOR_STRUCTURES::ControlMessage;
        using ThisClass = SensorParameterController<SENSOR_STRUCTURES, SERVER_STRUCTURES>;
        using ResponseControlMessageScheduler = DataFlow::DataProcessingScheduler<SensorControlMessage, ThisClass, ONLY_ONE_PRODUCER>;

        using ErrorSource = DataFlow::DataSource<ErrorHandling::SensorAccessLinkError>;

        using GetParameterValueRequest = ServerCommunication::RequestTypes::GetParameterValue;
        using SetUnsignedIntegerParameterValueRequest = ServerCommunication::RequestTypes::SetUnsignedIntegerParameterValue;
        using SetSignedIntegerParameterValueRequest = ServerCommunication::RequestTypes::SetSignedIntegerParameterValue;
        using SetRealNumberParameterValueRequest = ServerCommunication::RequestTypes::SetRealNumberParameterValue;
        using SetBooleanParameterValueRequest = ServerCommunication::RequestTypes::SetBooleanParameterValue;

        using Parameters = typename SERVER_STRUCTURES::Parameters;

        static size_t const ARBITRARILY_BIG_ENOUGH_NUMBER_OF_CONCURRENT_REQUESTS = 256;

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

        template<typename ResponseSource>
        void linkSensorControlMessageResponseSource(ResponseSource* responseSource) {
            responseSource->linkConsumer(&responseControlMessageScheduler);

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

        void processGet(GetParameterValueRequest&& getParameterValueRequest) noexcept {
            auto parameterName = getParameterValueRequest.getPayloadName();
            auto sensorControlMessage = parameters.createGetParameterValueControlMessageFor(parameterName);
            dataTranslator->translateAndSendToSensor(std::move(sensorControlMessage));
        }

        template<typename ParameterValueRequest>
        void processSet(ParameterValueRequest&& parameterValueRequest) noexcept {
//            auto parameterName = getParameterValueRequest.getPayloadName();
//            auto sensorControlMessage = parameters.createGetParameterValueControlMessageFor(parameterName);
//            dataTranslator->translateAndSendToSensor(std::move(sensorControlMessage));
        }

        void processSetUnsignedInteger(
                SetUnsignedIntegerParameterValueRequest&& setUnsignedIntegerParameterValueRequest) noexcept {
            processSet(std::forward<SetUnsignedIntegerParameterValueRequest>(setUnsignedIntegerParameterValueRequest));
        }

        void processSetSignedInteger(
                SetSignedIntegerParameterValueRequest&& setSignedIntegerParameterValueRequest) noexcept {
            processSet(std::forward<SetSignedIntegerParameterValueRequest>(setSignedIntegerParameterValueRequest));
        }

        void processSetRealNumber(SetRealNumberParameterValueRequest&& setRealNumberParameterValueRequest) noexcept {
            processSet(std::forward<SetRealNumberParameterValueRequest>(setRealNumberParameterValueRequest));
        }

        void processSetBoolean(SetBooleanParameterValueRequest&& setBooleanParameterValueRequest) noexcept {
            processSet(std::forward<SetBooleanParameterValueRequest>(setBooleanParameterValueRequest));
        }


        // TODO : test this function
        void calibrate() noexcept {
//            auto parameterName = getParameterValueRequest.getPayloadName();
//            auto sensorControlMessage = parameters.createGetParameterValueControlMessageFor(parameterName);
//            dataTranslator->translateAndSendToSensor(std::move(sensorControlMessage));
        }

        // TODO : test this function
        void clearCalibration() noexcept {
//            auto parameterName = getParameterValueRequest.getPayloadName();
//            auto sensorControlMessage = parameters.createGetParameterValueControlMessageFor(parameterName);
//            dataTranslator->translateAndSendToSensor(std::move(sensorControlMessage));
        }

        using ErrorSource::linkConsumer;

        template<typename RequestHandlerCallBackStore>
        static constexpr auto createCallBacksForRequestHandler(ThisClass* thisClass)
        -> typename RequestHandlerCallBackStore::InstancesTuple {
            return RequestHandlerCallBackStore::createCallBacks(
                    std::bind(&ThisClass::processGet, thisClass, _1),
                    std::bind(&ThisClass::processSetUnsignedInteger, thisClass, _1),
                    std::bind(&ThisClass::processSetSignedInteger, thisClass, _1),
                    std::bind(&ThisClass::processSetRealNumber, thisClass, _1),
                    std::bind(&ThisClass::processSetBoolean, thisClass, _1),
                    std::bind(&ThisClass::calibrate, thisClass),
                    std::bind(&ThisClass::clearCalibration, thisClass)
            );
        }

    private:

        RequestHandler* requestHandler;
        DataTranslator* dataTranslator;

        Parameters parameters;

        SensorControlMessagePointers sensorControlMessageRequestPointers;

        ResponseControlMessageScheduler responseControlMessageScheduler;
    };
}

#endif //SENSORGATEWAY_SENSORPARAMETERCONTROLLER_HPP
