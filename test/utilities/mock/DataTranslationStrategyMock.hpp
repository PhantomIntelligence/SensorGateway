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

#ifndef SENSORGATEWAY_DATATRANSLATIONSTRATEGYMOCK_HPP
#define SENSORGATEWAY_DATATRANSLATIONSTRATEGYMOCK_HPP

#include "Function.hpp"
#include "sensor-gateway/data-translation/DataTranslationStrategy.hpp"

namespace Mock {

    template<typename SENSOR_STRUCTURE, typename SERVER_STRUCTURE>
    class DataTranslationStrategyMock final :
            public DataTranslation::DataTranslationStrategy<SENSOR_STRUCTURE, SERVER_STRUCTURE> {

    protected:

        using super = DataTranslation::DataTranslationStrategy<SENSOR_STRUCTURE, SERVER_STRUCTURE>;
        using ThisClass = DataTranslationStrategyMock<SENSOR_STRUCTURE, SERVER_STRUCTURE>;

        using super::MessageSource;
        using super::RawDataSource;

        using SensorMessage = typename super::SensorMessage;
        using SensorRawData = typename super::SensorRawData;
        using ParameterControlMessage = typename super::ParameterControlMessage;

        using MockFunctionControlMessageToSensorMessageRequest = Function<StringLiteral<decltype("mock->translateControlMessageToSensorMessageRequest"_ToString)>, SensorMessage, ParameterControlMessage>;
        using MockFunctionSensorMessageToControlMessageResult = Function<StringLiteral<decltype("mock->translateSensorMessageToControlMessageResult"_ToString)>, ParameterControlMessage, SensorMessage>;

    public:

        explicit DataTranslationStrategyMock()
        {};


        ~DataTranslationStrategyMock() noexcept = default;

        void translateMessage(SensorMessage&& sensorMessage) override {}

        void translateRawData(SensorRawData&& sensorRawData) override {}

        SensorMessage
        translateControlMessageToSensorMessageRequest(ParameterControlMessage&& parameterControlMessage) override {
            SensorMessage sensorMessage = mockFunctionControlMessageToSensorMessageRequest.invoke(
                    std::forward<ParameterControlMessage>(parameterControlMessage));
            return sensorMessage;
        }

        ParameterControlMessage translateSensorMessageToControlMessageResult(SensorMessage&& sensorMessage) override {
            ParameterControlMessage parameterControlMessage = mockFunctionSensorMessageToControlMessageResult.invoke(
                    std::forward<SensorMessage>(sensorMessage));
            return parameterControlMessage;
        }

        ThisClass* onTranslateControlMessageToSensorMessageRequest(ParameterControlMessage&& parameterControlMessage) {
            mockFunctionControlMessageToSensorMessageRequest.onCall(std::forward<ParameterControlMessage>(parameterControlMessage));
            return this;
        }

        void returnThisSensorMessage(SensorMessage const& sensorMessageToMap) {
            mockFunctionControlMessageToSensorMessageRequest.returnThis(sensorMessageToMap);
        }

        ThisClass* onTranslateSensorMessageToControlMessageResult(SensorMessage&& sensorMessage) {
            mockFunctionSensorMessageToControlMessageResult.onCall(std::forward<SensorMessage>(sensorMessage));
            return this;
        }

        void returnThisControlMessage(ParameterControlMessage const& parameterControlMessageToMap) {
            mockFunctionSensorMessageToControlMessageResult.returnThis(parameterControlMessageToMap);
        }

    private:

        MockFunctionControlMessageToSensorMessageRequest mockFunctionControlMessageToSensorMessageRequest;
        MockFunctionSensorMessageToControlMessageResult mockFunctionSensorMessageToControlMessageResult;

    };
}

#endif //SENSORGATEWAY_DATATRANSLATIONSTRATEGYMOCK_HPP
