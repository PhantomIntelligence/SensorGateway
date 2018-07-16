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

#ifndef SPIRITSENSORGATEWAY_CONSTANTVALUESDEFINITION_H
#define SPIRITSENSORGATEWAY_CONSTANTVALUESDEFINITION_H

#include "TypeDefinition.h"

namespace DataFlow {
    uint8_t const NUMBER_OF_CONCURRENT_INPUT_FOR_SENSOR_ACCESS_LINK_ELEMENTS = 1;
    size_t const RING_BUFFER_SIZE = 64;
};

namespace CommandID {
    using CommandID = uint16_t;
    CommandID const SET_PARAMETER = 0xC0;
    CommandID const QUERY_PARAMETER = 0xC1;
    CommandID const RESPONSE_PARAMETER = 0x2;
    CommandID const ERROR_PARAMETER = 0xC3;
    CommandID const RECORD_RAW = 0xD0;
    CommandID const PLAYBACK_RAW = 0xD1;
    CommandID const RECORD_CALIBRATION = 0xDA;
    CommandID const CLEAR_CALIBRATION = 0xDB;
    CommandID const DEBUG_DUMP = 0xF0;
    CommandID const DEBUG_VERBOSE = 0xF1;
    CommandID const DEBUG_FORCE_REFRESH = 0xF2;
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
    using SensorID = uint16_t;
    namespace AWL {
        using MessageID = uint64_t;
        MessageID const END_OF_FRAME = 0x09;
        MessageID const DETECTION_TRACK = 0x0A;
        MessageID const DETECTION_VELOCITY = 0x0B;
        int const MAXIMUM_NUMBER_OF_DATA_IN_MESSAGE = 8;
        namespace _7 {
            SensorID const SENSOR_ID = 0x0010;
        }
        namespace _16 {
            int const MULTIPLICATIVE_CONSTANT = 1000;
            int const HORIZONTAL_FIELD_OF_VIEW = static_cast<int>(30.4*MULTIPLICATIVE_CONSTANT);
            int const NUMBER_OF_LAYER = 2;
            int const NUMBER_OF_PIXELS_IN_LAYER = 8;
            int const NUMBER_OF_PIXELS_IN_FRAME = 16;
            int const NUMBER_OF_TRACKS_IN_PIXEL = 16;
            int const ANGLE_RANGE = HORIZONTAL_FIELD_OF_VIEW / NUMBER_OF_PIXELS_IN_LAYER;
            SensorID const SENSOR_ID = 0x0010;
        }
    }
}



#endif //SPIRITSENSORGATEWAY_CONSTANTVALUESDEFINITION_H
