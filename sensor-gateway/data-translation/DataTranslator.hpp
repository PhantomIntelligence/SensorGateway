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
#ifndef SENSORGATEWAY_DATATRANSLATOR_H
#define SENSORGATEWAY_DATATRANSLATOR_H

#include "sensor-gateway/sensor-communication/SensorCommunicator.hpp"
#include "DataTranslationStrategy.hpp"

namespace SensorAccessLinkElement {

    template<class SENSOR_STRUCTURES, class SERVER_STRUCTURES>
    class DataTranslator : public DataFlow::DataSink<typename SENSOR_STRUCTURES::Message>,
                           public DataFlow::DataSink<typename SENSOR_STRUCTURES::RawData>,
                           public DataFlow::DataSource<ErrorHandling::SensorAccessLinkError> {

    protected:

        using DataTranslationStrategy = DataTranslation::DataTranslationStrategy<SENSOR_STRUCTURES, SERVER_STRUCTURES>;

        using SensorMessage = typename SENSOR_STRUCTURES::Message;
        using SensorRawData = typename SENSOR_STRUCTURES::RawData;

        using MessageSink = DataFlow::DataSink<SensorMessage>;
        using RawDataSink = DataFlow::DataSink<SensorRawData>;

    public:

        explicit DataTranslator(DataTranslationStrategy* dataTranslationStrategy) :
                dataTranslationStrategy(dataTranslationStrategy) {};

        ~DataTranslator() noexcept = default;

        DataTranslator(DataTranslator const& other) = delete;

        DataTranslator(DataTranslator&& other) noexcept = delete;

        DataTranslator& operator=(DataTranslator const& other)& = delete;

        DataTranslator& operator=(DataTranslator&& other)& noexcept = delete;

        void consume(SensorMessage&& message) override {
            dataTranslationStrategy->translateMessage(std::forward<SensorMessage>(message));
        };

        void consume(SensorRawData&& rawData) override {
            dataTranslationStrategy->translateRawData(std::forward<SensorRawData>(rawData));
        };

    private:

        DataTranslationStrategy* dataTranslationStrategy;

    };
}

#endif //SENSORGATEWAY_DATATRANSLATOR_H
