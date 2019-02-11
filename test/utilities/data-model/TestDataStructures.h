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

namespace Sensor {
    namespace Test {
        namespace Simple {
            class Structures final : public Sensor::Communication::DataStructures {

            public :

                typedef typename DataModel::SimpleMessage Message;
                typedef typename DataModel::SimpleRawData RawData;
                typedef typename DataModel::SimpleControlMessage ControlMessage;
                using Parameters = Gateway::NoGatewayParametersDefinition; // TODO : change for actual test ones

                static size_t const MAX_NUMBER_OF_BULK_FETCHABLE_MESSAGES = 1; // IMPORTANT: this has to stay = 1!
                static size_t const MAX_NUMBER_OF_BULK_FETCHABLE_RAW_DATA_CYCLES = 1;
            };
        }

        namespace RealisticImplementation {

            namespace Constants {

                static size_t const NUMBER_OF_SENSOR_TIME_POINTS = 0;
                static size_t const NUMBER_OF_GATEWAY_TIME_POINTS = 2;

                static size_t const NUMBER_OF_PIXELS = 4;
                static size_t const NUMBER_OF_CHANNELS = 2;
                static size_t const NUMBER_OF_DETECTION_PER_CHANNEL = 4;
                static size_t const RAW_DATA_SAMPLING_LENGTH = 16;
                static size_t const MAX_COMMAND_PAYLOAD_SIZE = 8;
            }

            class Structures final : public Sensor::Communication::DataStructures {

            public :

                typedef typename
                Metrics::TimeTrackingDefinition<
                        Constants::NUMBER_OF_SENSOR_TIME_POINTS,
                        Constants::NUMBER_OF_GATEWAY_TIME_POINTS
                > StubTimeTrackingDefinition;

                typedef typename
                Sensor::SensorMessageDefinition<
                        Constants::NUMBER_OF_PIXELS,
                        StubTimeTrackingDefinition
                > StubMessageDefinition;

                typedef typename
                Sensor::RawDataDefinition<
                        RawDataTypes::GUARDIAN,
                        Constants::NUMBER_OF_CHANNELS,
                        Constants::RAW_DATA_SAMPLING_LENGTH
                > StubRawDataDefinition;

                typedef typename
                Sensor::ControlMessageDefinition<
                        ControlMessagePayloadTypes::NO_SENSOR,
                        Constants::MAX_COMMAND_PAYLOAD_SIZE
                > StubControlMessageDefinition;

                typedef typename DataFlow::SensorMessage<StubMessageDefinition> Message;
                typedef typename DataFlow::RawData<StubRawDataDefinition> RawData;
                typedef typename DataFlow::ControlMessage<StubControlMessageDefinition> ControlMessage;
                using Parameters = Gateway::NoGatewayParametersDefinition; // TODO : change for actual test ones

                static size_t const MAX_NUMBER_OF_BULK_FETCHABLE_MESSAGES = 1; // IMPORTANT: this has to stay = 1!
                static size_t const MAX_NUMBER_OF_BULK_FETCHABLE_RAW_DATA_CYCLES = 1;
            };
        }
    }
}

#endif //SENSORGATEWAY_TESTDATASTRUCTURES_H
