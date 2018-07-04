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

#ifndef SPIRITSENSORGATEWAY_TYPEDEFINITION_H
#define SPIRITSENSORGATEWAY_TYPEDEFINITION_H

#include <array>
#include <mutex>
#include <iostream>
#include <unordered_map>
#include <exception>

#include "ExceptionMessages.h"




namespace {
    typedef std::mutex Mutex;
    typedef std::lock_guard<Mutex> LockGuard;
};

namespace  SpiritProtocol {
    typedef int16_t Acceleration;
    typedef uint8_t ConfidenceLevel;
    typedef uint16_t Distance;
    typedef uint16_t FrameID;
    typedef uint16_t Intensity;
    typedef uint16_t PixelID;
    typedef int16_t Speed;
    typedef uint16_t SystemID;
    typedef uint16_t TrackID;
};

#endif //SPIRITSENSORGATEWAY_TYPEDEFINITION_H
