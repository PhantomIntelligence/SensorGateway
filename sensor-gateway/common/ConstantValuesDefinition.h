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

#ifndef SENSORGATEWAY_CONSTANTVALUESDEFINITION_H
#define SENSORGATEWAY_CONSTANTVALUESDEFINITION_H

#include "TypeDefinition.h"

namespace DataFlow {
    uint8_t const NUMBER_OF_CONCURRENT_INPUT_FOR_SENSOR_ACCESS_LINK_ELEMENTS = 1;
    size_t const RING_BUFFER_SIZE = 256;
};

namespace CommandId {
    using CommandId = uint16_t;
    CommandId const SET_PARAMETER = 0xC0;
    CommandId const QUERY_PARAMETER = 0xC1;
    CommandId const RESPONSE_PARAMETER = 0x2;
    CommandId const ERROR_PARAMETER = 0xC3;
    CommandId const RECORD_RAW = 0xD0;
    CommandId const PLAYBACK_RAW = 0xD1;
    CommandId const RECORD_CALIBRATION = 0xDA;
    CommandId const CLEAR_CALIBRATION = 0xDB;
    CommandId const DEBUG_DUMP = 0xF0;
    CommandId const DEBUG_VERBOSE = 0xF1;
    CommandId const DEBUG_FORCE_REFRESH = 0xF2;
};

namespace CommandType {
    using CommandType = uint16_t;
    CommandType const ALGO_SELECTED = 0x01;
    CommandType const ALGO_PARAMETER = 0x02;
    CommandType const SENSOR_REGISTER = 0x03;
    CommandType const BIAS = 0x04;
    CommandType const ADC_REGISTER = 0x05;
    CommandType const PRESET = 0x06;
    CommandType const GLOBAL_PARAMETER = 0x07;
    CommandType const GPIO = 0x08;
    CommandType const TRACKING_ALGO_SELECTED = 0x11;
    CommandType const TRACKING_ALGO_PARAMETER = 0x12;
    CommandType const DATE_TIME = 0x20;
    CommandType const ERROR = 0x70;
    CommandType const RECORD = 0XD0;
    CommandType const PLAYBACK = 0xD1;
    CommandType const TRANSMIT_RAW = 0xE0;
    CommandType const TRANSMIT_COOKED = 0xE1;
}

namespace Sensor {
    using SensorId = uint16_t;

    namespace Guardian {
        static size_t const NUMBER_OF_CHANNELS = 16;
        static size_t const NUMBER_OF_DETECTION_PER_CHANNEL = 16;
        static size_t const RAW_DATA_SAMPLING_LENGTH = 100;
        size_t const DETECTIONS_SIZE = NUMBER_OF_CHANNELS * NUMBER_OF_DETECTION_PER_CHANNEL;

        static size_t const MAX_COMMAND_PAYLOAD_SIZE = 4096; // 1024 * floats == 1024 * 4 Bytes
    }

    namespace AWL {
        using MessageId = uint64_t;
        MessageId const END_OF_FRAME = 0x09;
        MessageId const DETECTION_TRACK = 0x0A;
        MessageId const DETECTION_VELOCITY = 0x0B;
        int const NUMBER_OF_DATA_BYTES = 8;

        /**
         * @note: The AWL raw data is currently not supported. This is why the RAW_DATA_SAMPLING_LENGTH
         * real value (1024) is commented out. Nontheless, the templates for DataStructures still require
         * one to be defined, this is why the value is set to 1, which minimizes the memory usage for an AWLAccessLink
         */
//        static size_t const RAW_DATA_SAMPLING_LENGTH = 1024;
        static size_t const RAW_DATA_SAMPLING_LENGTH = 1;

        static size_t const MAX_COMMAND_PAYLOAD_SIZE = 4; // 4 Bytes

        namespace _7 {
            SensorId const SENSOR_ID = 0x0010;
        }
        namespace _16 {
            static size_t const NUMBER_OF_CHANNELS = 16;
            size_t const MAX_RAW_BUFFER_SIZE = 16384;
            int const MULTIPLICATIVE_CONSTANT = 10;  // Because all values are multiple with 1 decimal, 10 is sufficient to work with non-floating point representation
            int const HORIZONTAL_FIELD_OF_VIEW = static_cast<int>(30.4 * MULTIPLICATIVE_CONSTANT);
            int const NUMBER_OF_LAYER = 2;
            int const NUMBER_OF_PIXELS_IN_LAYER = 8;
            int const NUMBER_OF_PIXELS_IN_FRAME = 16;
            int const NUMBER_OF_TRACKS_IN_PIXEL = 16;
            int const ANGLE_RANGE = HORIZONTAL_FIELD_OF_VIEW / NUMBER_OF_PIXELS_IN_LAYER;
            SensorId const SENSOR_ID = 0x0010;
        }
    }
}


#endif //SENSORGATEWAY_CONSTANTVALUESDEFINITION_H
