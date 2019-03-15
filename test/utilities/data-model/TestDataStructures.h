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

#ifndef SENSORGATEWAY_TESTDATASTRUCTURES_H
#define SENSORGATEWAY_TESTDATASTRUCTURES_H

#include "SimpleControlMessage.h"
#include "FakeParameterConstants.hpp"

namespace Sensor {
    namespace Test {
        namespace Simple {
            class Structures final : public Sensor::Communication::DataStructures {

            public :

                typedef typename DataModel::SimpleMessage Message;
                typedef typename DataModel::SimpleRawData RawData;
                typedef typename DataModel::SimpleControlMessage ControlMessage;
                using Parameters = Sensor::FakeParameter::FruitBasketParameters;

                static constexpr size_t const MAX_NUMBER_OF_BULK_FETCHABLE_MESSAGES = 1;
                static constexpr size_t const MAX_NUMBER_OF_BULK_FETCHABLE_RAW_DATA_CYCLES = 1;
                static constexpr size_t const ASYNC_REQUEST_BUFFER_SIZE_BEFORE_TRANSMISSION_TO_SENSOR = 2;

                static size_t const MAX_NUMBER_OF_CONCURRENT_REQUEST_OF_ONE_KIND = 16;
            };
        }

        namespace RealisticImplementation {

            namespace Constants {

                static size_t const NUMBER_OF_SENSOR_TIME_POINTS = 0;
                static size_t const NUMBER_OF_GATEWAY_TIME_POINTS = 2;

                static size_t const NUMBER_OF_PIXELS = 4;
                static size_t const NUMBER_OF_TRACKS_PER_PIXEL = 2;
                static size_t const NUMBER_OF_CHANNELS = 2;
                static size_t const RAW_DATA_SAMPLING_LENGTH = 16;
                static size_t const MAX_COMMAND_PAYLOAD_SIZE = FakeParameter::PARAMETER_CONTROL_MESSAGE_PAYLOAD_LENGTH_IN_BITS / 8;

                typedef Byte STUB_SENSOR_DATA_TYPE;
            }

            class Structures final : public Sensor::Communication::DataStructures {

            public :

                typedef typename
                Metrics::TimeTrackingDefinition<
                        Constants::NUMBER_OF_SENSOR_TIME_POINTS,
                        Constants::NUMBER_OF_GATEWAY_TIME_POINTS
                > TimeTrackingDefinition;

                typedef typename
                Sensor::SensorMessageDefinition<
                        Constants::NUMBER_OF_PIXELS,
                        Constants::NUMBER_OF_TRACKS_PER_PIXEL,
                        TimeTrackingDefinition
                > MessageDefinition;

                typedef typename
                Sensor::RawDataDefinition<
                        Constants::STUB_SENSOR_DATA_TYPE,
                        Constants::NUMBER_OF_CHANNELS,
                        Constants::RAW_DATA_SAMPLING_LENGTH
                > RawDataDefinition;

                typedef typename
                Sensor::ControlMessageDefinition<
                        Constants::STUB_SENSOR_DATA_TYPE,
                        Constants::MAX_COMMAND_PAYLOAD_SIZE
                > ControlMessageDefinition;

                typedef typename DataFlow::SensorMessage<MessageDefinition> Message;
                typedef typename DataFlow::RawData<RawDataDefinition> RawData;
                typedef typename DataFlow::ControlMessage<ControlMessageDefinition> ControlMessage;
                using Parameters = Sensor::FakeParameter::FruitBasketParameters;

                static constexpr size_t const MAX_NUMBER_OF_BULK_FETCHABLE_MESSAGES = 2;
                static constexpr size_t const MAX_NUMBER_OF_BULK_FETCHABLE_RAW_DATA_CYCLES = 2;
                static constexpr size_t const ASYNC_REQUEST_BUFFER_SIZE_BEFORE_TRANSMISSION_TO_SENSOR = 8;

                static constexpr size_t const MAX_NUMBER_OF_CONCURRENT_REQUEST_OF_ONE_KIND = 16;
            };
        }
    }
}
#endif //SENSORGATEWAY_TESTDATASTRUCTURES_H
