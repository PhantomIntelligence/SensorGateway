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
#include "SensorParameterErrorFactory.h"

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
        using ParameterName = typename Parameters::ParameterName;
        using SensorResponseStatus = typename Parameters::ParameterResponseStatus;
        using RequestedValue = typename Parameters::RequestedValue;

        using SensorParameterStatuses = typename Parameters::ParameterResponseStatuses;
        using GetParameterRequests = std::unordered_map<ParameterName, GetParameterValueRequest>;
        using SetUnsignedIntegerParameterRequests = std::unordered_map<ParameterName, SetUnsignedIntegerParameterValueRequest>;
        using SetSignedIntegerParameterRequests = std::unordered_map<ParameterName, SetSignedIntegerParameterValueRequest>;
        using SetRealNumberParameterRequests = std::unordered_map<ParameterName, SetRealNumberParameterValueRequest>;
        using SetBooleanParameterRequests = std::unordered_map<ParameterName, SetBooleanParameterValueRequest>;

        using RequestedParameterGetValues = typename Parameters::RequestedParameterNames;
        using RequestedParameterSetValues = typename Parameters::RequestedParameterValues;

    public:

        explicit SensorParameterController(RequestHandler* requestHandler,
                                           DataTranslator* dataTranslator) :
                requestHandler(requestHandler),
                dataTranslator(dataTranslator),
                calibrationRequested(false),
                clearCalibrationRequested(false),
                responseControlMessageScheduler(this) {
        }

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

        // TODO: test this code
        template<typename Metadata, typename RequestMap>
        void sendErrorToRequestHandler(Metadata metadata, RequestMap* map) noexcept {
            auto name = metadata.name;
            bool requestedInThisMap = map->find(name) != map->end();
            if (requestedInThisMap) {
                auto originalRequest = map->at(name);
                requestHandler->writeAndSendParameterErrorResponse(metadata, std::move(originalRequest));
                map->erase(name);
            }
        }

        // TODO: test this code
        template<typename RequestMap>
        void sendParameterValueToRequestHandler(ParameterName name, SensorResponseStatus sensorResponseStatus, RequestMap* map) noexcept {
            bool requestedInThisMap = map->find(name) != map->end();
            if (requestedInThisMap) {
                auto originalRequest = map->at(name);
                auto metadata = sensorResponseStatus.metadata;
                auto parameterValue = sensorResponseStatus.receivedValue;
                if (isUnsignedInteger(parameterValue)) {
                    auto value = getUnsignedInteger(parameterValue);
                    requestHandler->writeAndSendParameterValueResponse(metadata, value, std::move(originalRequest));
                } else if (isSignedInteger(parameterValue)) {
                    auto value = getSignedInteger(parameterValue);
                    requestHandler->writeAndSendParameterValueResponse(metadata, value, std::move(originalRequest));
                } else if (isRealNumber(parameterValue)) {
                    auto value = getRealNumber(parameterValue);
                    requestHandler->writeAndSendParameterValueResponse(metadata, value, std::move(originalRequest));
                } else if (isBoolean(parameterValue)) {
                    auto value = getBoolean(parameterValue);
                    requestHandler->writeAndSendParameterValueResponse(metadata, value, std::move(originalRequest));
                } else {
                    ErrorSource::produce(
                            std::move(
                                    ErrorHandling::createSensorParameterError(
                                            ErrorHandling::INVALID_SENSOR_PARAMETER_VALUE_TYPE,
                                            ErrorHandling::Origin::SENSOR_PARAMETER_CONTROLLER_PARSE_VALUE_BEFORE_SEND_TO_REQUEST,
                                            ErrorHandling::Message::PARAMETER_VALUE_TYPE_NOT_RECOGNIZED)
                            )
                    );
                }
                map->erase(name);
            }
        }

        // TODO: test this code
        void consume(SensorControlMessage&& sensorControlMessageResponse) override {
            LockGuard guard(requestMutex);
            using ControlCode = typename SensorControlMessage::ControlMessageCode;
            if (sensorControlMessageResponse.isResponse()) {
                bool hasToSendResponseToServer = false;
                bool hasToSendControlMessageToSensor = false;
                SensorControlMessage setValuesSensorControlMessage;
                std::tie(hasToSendResponseToServer, hasToSendControlMessageToSensor, setValuesSensorControlMessage)
                        = parameters.updateStatusForResponse(
                        sensorControlMessageResponse,
                        &requestedParameterGetValues,
                        &requestedParameterSetValues,
                        &sensorParameterStatuses
                );
                if (hasToSendResponseToServer) {
                    for (auto&& statusPair :sensorParameterStatuses) {
                        auto parameterName = statusPair.first;
                        auto status = statusPair.second;
                        sendParameterValueToRequestHandler(parameterName, status, &getParameterRequests);
                        sendParameterValueToRequestHandler(parameterName, status, &setUnsignedIntegerParameterRequests);
                        sendParameterValueToRequestHandler(parameterName, status, &setSignedIntegerParameterRequests);
                        sendParameterValueToRequestHandler(parameterName, status, &setRealNumberParameterRequests);
                        sendParameterValueToRequestHandler(parameterName, status, &setBooleanParameterRequests);
                    }
                }
                if (hasToSendControlMessageToSensor) {
                    dataTranslator->translateAndSendToSensor(std::move(setValuesSensorControlMessage));
                }
            }

            if (sensorControlMessageResponse.isError()) {
                parameters.updateStatusForError(std::move(sensorControlMessageResponse), &sensorParameterStatuses);

                for (auto&& statusPair :sensorParameterStatuses) {
                    auto metadata = statusPair.second.metadata;
                    sendErrorToRequestHandler(metadata, &getParameterRequests);
                    sendErrorToRequestHandler(metadata, &setUnsignedIntegerParameterRequests);
                    sendErrorToRequestHandler(metadata, &setSignedIntegerParameterRequests);
                    sendErrorToRequestHandler(metadata, &setRealNumberParameterRequests);
                    sendErrorToRequestHandler(metadata, &setBooleanParameterRequests);
                }
            }
        }

        // TODO: test this code
        void processGet(GetParameterValueRequest&& request) noexcept {
            LockGuard guard(requestMutex);
            auto name = request.getPayloadName();
            getParameterRequests[name] = request;
            addRequestedGetValue(name);
            auto sensorControlMessage = parameters.createGetParameterValueControlMessageFor(name);
            dataTranslator->translateAndSendToSensor(std::move(sensorControlMessage));
        }

        // TODO: test this code
        template<typename ParameterValueRequest>
        void processSet(ParameterValueRequest&& parameterValueRequest) noexcept {
            LockGuard guard(requestMutex);
            auto parameterName = parameterValueRequest.getPayloadName();
            auto requestedValue = parameterValueRequest.getRequestedValue();
            addRequestedSetValue(parameterName, requestedValue);
            auto sensorControlMessage = parameters.createGetParameterValueControlMessageFor(parameterName);

            dataTranslator->translateAndSendToSensor(std::move(sensorControlMessage));
        }

        void processSetUnsignedInteger(SetUnsignedIntegerParameterValueRequest&& request) noexcept {
            auto name = request.getPayloadName();
            setUnsignedIntegerParameterRequests[name] = request;
            processSet(std::move(request));
        }

        void processSetSignedInteger(SetSignedIntegerParameterValueRequest&& request) noexcept {
            auto name = request.getPayloadName();
            setSignedIntegerParameterRequests[name] = request;
            processSet(std::move(request));
        }

        void processSetRealNumber(SetRealNumberParameterValueRequest&& request) noexcept {
            auto name = request.getPayloadName();
            setRealNumberParameterRequests[name] = request;
            processSet(std::move(request));
        }

        void processSetBoolean(SetBooleanParameterValueRequest&& request) noexcept {
            auto name = request.getPayloadName();
            setBooleanParameterRequests[name] = request;
            processSet(std::move(request));
        }


        // TODO : test this function
        void calibrate() noexcept {
            calibrationRequested.store(true);
            SensorControlMessage sensorControlMessage;
            sensorControlMessage.calibrate();
            dataTranslator->translateAndSendToSensor(std::move(sensorControlMessage));
        }

        // TODO : test this function
        void clearCalibration() noexcept {
            clearCalibrationRequested.store(true);
            SensorControlMessage sensorControlMessage;
            sensorControlMessage.clearCalibration();
            dataTranslator->translateAndSendToSensor(std::move(sensorControlMessage));
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

        void addRequestedGetValue(ParameterName const& parameterName) {
            requestedParameterGetValues.insert(parameterName);
        }

        template<typename T>
        void addRequestedSetValue(ParameterName const& parameterName, T&& requestedValue) {
            requestedParameterSetValues.at(parameterName) = createRequestedValue(requestedValue);
        }

        static constexpr auto createRequestedValue(UnsignedInteger value) noexcept -> RequestedValue {
            return std::make_tuple(
                    true, value,
                    false, 0,
                    false, 0.0,
                    false, false
            );
        }

        static constexpr auto createRequestedValue(SignedInteger value) noexcept -> RequestedValue {
            return std::make_tuple(
                    false, 0,
                    true, value,
                    false, 0.0,
                    false, false
            );
        }

        static constexpr auto createRequestedValue(RealNumber value) noexcept -> RequestedValue {
            return std::make_tuple(
                    false, 0,
                    false, 0,
                    true, value,
                    false, false
            );
        }

        static constexpr auto createRequestedValue(bool value) noexcept -> RequestedValue {
            return std::make_tuple(
                    false, 0,
                    false, 0,
                    false, 0.0,
                    true, value
            );
        }

        RequestHandler* requestHandler;
        DataTranslator* dataTranslator;

        Mutex requestMutex;

        Parameters parameters;
        SensorParameterStatuses sensorParameterStatuses;

        GetParameterRequests getParameterRequests;
        SetUnsignedIntegerParameterRequests setUnsignedIntegerParameterRequests;
        SetSignedIntegerParameterRequests setSignedIntegerParameterRequests;
        SetRealNumberParameterRequests setRealNumberParameterRequests;
        SetBooleanParameterRequests setBooleanParameterRequests;

        RequestedParameterGetValues requestedParameterGetValues;
        RequestedParameterSetValues requestedParameterSetValues;

        AtomicFlag calibrationRequested;
        AtomicFlag clearCalibrationRequested;

        ResponseControlMessageScheduler responseControlMessageScheduler;
    };
}

#endif //SENSORGATEWAY_SENSORPARAMETERCONTROLLER_HPP
