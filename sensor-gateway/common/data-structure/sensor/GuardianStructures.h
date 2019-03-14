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

#ifndef SENSORGATEWAY_GUARDIANSTRUCTURES_H
#define SENSORGATEWAY_GUARDIANSTRUCTURES_H

#include "sensor-gateway/common/data-structure/DataStructures.h"
#include "AWLMessage.hpp"

namespace Sensor {
    namespace Guardian {

        template<typename ParameterList = Gateway::NoGatewayParameters>
        class Structures final : public Communication::DataStructures {

        public :

            typedef typename
            Metrics::TimeTrackingDefinition<
                    Guardian::NUMBER_OF_SENSOR_TIME_POINTS,
                    Guardian::NUMBER_OF_GATEWAY_TIME_POINTS
            > TimeTrackingDefinition;

            typedef typename
            Sensor::SensorMessageDefinition<
                    Guardian::NUMBER_OF_PIXELS,
                    Guardian::NUMBER_OF_TRACKS_PER_PIXEL,
                    TimeTrackingDefinition
            > MessageDefinition;

            typedef typename
            Sensor::RawDataDefinition<
                    RawDataTypes::GUARDIAN,
                    Guardian::NUMBER_OF_CHANNELS,
                    Guardian::RAW_DATA_SAMPLING_LENGTH
            > RawDataDefinition;

            typedef typename
            Sensor::ControlMessageDefinition<
                    ControlMessagePayloadTypes::GUARDIAN,
                    Guardian::MAX_COMMAND_PAYLOAD_SIZE
            > ControlMessageDefinition;

            typedef typename DataFlow::AWLMessage<MessageDefinition> Message;
            typedef typename DataFlow::RawData<RawDataDefinition> RawData;
            typedef typename DataFlow::ControlMessage<ControlMessageDefinition> ControlMessage;
            using Parameters = ParameterList;

            static constexpr size_t const MAX_NUMBER_OF_BULK_FETCHABLE_MESSAGES = 32;
            static constexpr size_t const MAX_NUMBER_OF_BULK_FETCHABLE_RAW_DATA_CYCLES = 8;
            static constexpr size_t const ASYNC_REQUEST_BUFFER_SIZE_BEFORE_TRANSMISSION_TO_SENSOR = 8;

            static std::array<RawDataTypes::GUARDIAN,
                    Guardian::NUMBER_OF_CHANNELS> constexpr CHANNEL_POSITIONS = {
                    8, 0, 9, 1, 10, 2, 11, 3, 12, 4, 13, 5, 14, 6, 15, 7
            };
        };
    }
}

#endif //SENSORGATEWAY_GUARDIANSTRUCTURES_H
