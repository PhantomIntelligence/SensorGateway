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

#ifndef SENSORGATEWAY_GATEWAYSTRUCTURES_H
#define SENSORGATEWAY_GATEWAYSTRUCTURES_H

#include "sensor-gateway/common/data-structure/DataStructures.h"
#include "sensor-gateway/common/data-structure/parameter/ParameterConstants.hpp"

namespace Sensor {
    namespace Gateway {

        template<typename SensorMessageDefinition, typename RawDataDefinition, typename ControlMessageDefinition, typename ParametersList = NoGatewayParameters>
        class Structures final : public Communication::DataStructures {

        protected:

            // (Beginning + End) of acquisition + (Beginning + End) of transmission on sensor
            static size_t const NUMBER_OF_SENSOR_TIME_POINTS = 4;
            // (Beginning + End) of reception + (Beginning + End) of translation + transmission on gateway
            static size_t const NUMBER_OF_GATEWAY_TIME_POINTS = 5;

        public :

            typedef typename
            Metrics::TimeTrackingDefinition<
                    NUMBER_OF_SENSOR_TIME_POINTS,
                    NUMBER_OF_GATEWAY_TIME_POINTS
            > GatewayTimeTrackingDefinition;

            typedef typename
            Sensor::SensorMessageDefinition<
                    SensorMessageDefinition::NUMBER_OF_PIXELS,
                    GatewayTimeTrackingDefinition
            > GatewaySensorMessageDefinition;

            enum DataType : uint32_t {
//            [0, 499] -> reserved for SensorGateway messages


//            [500, +] -> reserved for Sensor message

                SENSOR_MESSAGE = 500,
                SENSOR_RAW_DATA = 501
            };

            typedef typename DataFlow::SensorMessage<GatewaySensorMessageDefinition> Message;
            typedef typename DataFlow::RawData<RawDataDefinition> RawData;
            typedef typename DataFlow::ControlMessage<ControlMessageDefinition> ControlMessage;
            using Parameters = ParametersList;

            static size_t const MAX_NUMBER_OF_BULK_FETCHABLE_MESSAGES = 32;
            static size_t const MAX_NUMBER_OF_BULK_FETCHABLE_RAW_DATA_CYCLES = 8;

            static size_t const MAX_NUMBER_OF_CONCURRENT_REQUEST_OF_ONE_KIND = 256;

        };
    }
}

#endif //SENSORGATEWAY_GATEWAYSTRUCTURES_H
