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
#ifndef SENSORGATEWAY_DATATRANSLATIONSTRATEGY_HPP
#define SENSORGATEWAY_DATATRANSLATIONSTRATEGY_HPP

#include "sensor-gateway/common/data-flow/DataSource.hpp"

namespace DataTranslation {

    template<class SENSOR_STRUCTURES, class SERVER_STRUCTURES>
    class DataTranslationStrategy : public DataFlow::DataSource<typename SERVER_STRUCTURES::Message>,
                                    public DataFlow::DataSource<typename SERVER_STRUCTURES::RawData> {

    protected:
        using SensorMessage = typename SENSOR_STRUCTURES::Message;
        using SensorRawData = typename SENSOR_STRUCTURES::RawData;

        using ServerMessage = typename SERVER_STRUCTURES::Message;
        using ServerRawData = typename SERVER_STRUCTURES::RawData;

        using MessageSource = DataFlow::DataSource<ServerMessage>;
        using RawDataSource = DataFlow::DataSource<ServerRawData>;

    public:
        DataTranslationStrategy() :
                currentOutputMessage(ServerMessage::returnDefaultData()) {}

        virtual ~DataTranslationStrategy() noexcept = default;

        DataTranslationStrategy(DataTranslationStrategy const& other) = delete;

        DataTranslationStrategy(DataTranslationStrategy&& other) noexcept = delete;

        DataTranslationStrategy& operator=(DataTranslationStrategy const& other)& = delete;

        DataTranslationStrategy& operator=(DataTranslationStrategy&& other)& noexcept = delete;

        virtual void translateMessage(SensorMessage&& sensorMessage) = 0;

        virtual void translateRawData(SensorRawData&& sensorRawData) = 0;

        using MessageSource::linkConsumer;

        using RawDataSource::linkConsumer;

    protected:

        ServerMessage currentOutputMessage;
    };
}

#endif //SENSORGATEWAY_DATATRANSLATIONSTRATEGY_HPP
