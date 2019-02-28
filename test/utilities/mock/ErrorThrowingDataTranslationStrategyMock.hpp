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

#ifndef SENSORGATEWAY_ERRORTHROWINGDATATRANSLATIONSTRATEGYMOCK_HPP
#define SENSORGATEWAY_ERRORTHROWINGDATATRANSLATIONSTRATEGYMOCK_HPP

#include "test/utilities/data-model/DataModelFixture.h"
#include "sensor-gateway/data-translation/DataTranslationStrategy.hpp"

namespace Mock {

    template<typename T>
    class ErrorThrowingDataTranslationStrategyMock :
            public DataTranslation::DataTranslationStrategy<T, T> {

    protected:

        using super = DataTranslation::DataTranslationStrategy<T, T>;
        using SensorMessage = typename super::SensorMessage;
        using SensorRawData = typename super::SensorRawData;
        using ParameterControlMessage = typename super::ParameterControlMessage;

        using MessageSource = typename super::MessageSource;
        using RawDataSource = typename super::RawDataSource;

    public:

        ErrorThrowingDataTranslationStrategyMock() = default;

        ~ErrorThrowingDataTranslationStrategyMock() noexcept = default;

        void translateMessage(SensorMessage&& sensorMessage) override {
            throw ErrorHandling::SensorAccessLinkError(ORIGIN,
                                                       ErrorHandling::Category::EMPTY_CATEGORY,
                                                       ErrorHandling::Severity::EMPTY_SEVERITY,
                                                       ERROR_CODE,
                                                       MESSAGE_ERROR_MESSAGE);
        };

        void translateRawData(SensorRawData&& sensorRawData) override {
            throw ErrorHandling::SensorAccessLinkError(ORIGIN,
                                                       ErrorHandling::Category::EMPTY_CATEGORY,
                                                       ErrorHandling::Severity::EMPTY_SEVERITY,
                                                       ERROR_CODE,
                                                       RAW_DATA_ERROR_MESSAGE);
        }

        SensorMessage
        translateControlMessageToSensorMessageRequest(ParameterControlMessage&& parameterControlMessage) override {
            std::cout << "\n\nFrom file : ErrorThrowingDatatTranslationStrategyMock... "
                      << "\n\n function : translateControlMessageToSensorMessageRequest... "
                      << "\n\n     TODO : have this function throw something and remove return --> do when needed in tests"
                      << "\n\n" << std::endl;
            return super::SensorMessage::returnDefaultData();
        }

        ParameterControlMessage translateSensorMessageToControlMessageResult(SensorMessage&& sensorMessage) override {
            std::cout << "\n\nFrom file : ErrorThrowingDatatTranslationStrategyMock... "
                      << "\n\n function : translateSensorMessageToControlMessageResult... "
                      << "\n\n     TODO : have this function throw something and remove return --> do when needed in tests"
                      << "\n\n" << std::endl;
            return super::ParameterControlMessage::returnDefaultData();
        }

        std::string const ORIGIN = "from the throwing strategy";
        ErrorHandling::ErrorCode const ERROR_CODE = ErrorHandling::GatewayErrorCode::DATA_NOT_RECOGNIZED;
        std::string const MESSAGE_ERROR_MESSAGE = "Error message from the message consumption strategy";
        std::string const RAW_DATA_ERROR_MESSAGE = "Error message from the rawdata consumption strategy";
    };
}

#endif //SENSORGATEWAY_ERRORTHROWINGDATATRANSLATIONSTRATEGYMOCK_HPP
