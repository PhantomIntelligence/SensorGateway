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

#ifndef SPIRITSENSORGATEWAY_CONSTANTDEFINITION_H
#define SPIRITSENSORGATEWAY_CONSTANTDEFINITION_H

#include <iostream>


namespace {
    using CommandID = uint16_t;
    CommandID const TRANSMIT_RAW  = 0xE0;
    CommandID const TRANSMIT_COOKED = 0xE1;
    CommandID const FRAME_DONE = 0x09;
    CommandID const DETECTION_TRACK = 0x0A;
    CommandID const DETECTION_VELOCITY = 0x0B;
    const int MESSAGE_DATA_LENGTH = 8;
    const unsigned long READ_WAIT_INFINITE = -1;


};
namespace SensorSystemID{
    uint16_t const AWL16 = 0x0010;
    uint16_t const AWL7 = 0x0010;
}

#endif //SPIRITSENSORGATEWAY_CONSTANTDEFINITION_H
