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


#ifndef SPIRITSENSORGATEWAY_DATAMODELTYPEDEFINITION_H
#define SPIRITSENSORGATEWAY_DATAMODELTYPEDEFINITION_H

#include <array>
#include <atomic>
#include <future>
//#include "HICpp/HighIntegrityThread.h"
//#include "Processing/ExceptionMessages.h"

namespace { // Types

    typedef bool Boolean;
    typedef unsigned char unsignedChar;

    typedef uint8_t uData8;
    typedef uint16_t uData16;
    typedef uint32_t uData32;
    typedef uint64_t uData64;

    typedef float Data32;
    typedef double Data64;
    typedef long double DataExtended;

    typedef std::chrono::milliseconds Millisecond;
    typedef std::chrono::high_resolution_clock::time_point TimePoint;
    typedef std::chrono::high_resolution_clock HighResolutionClock;

    typedef std::atomic_uint8_t AtomicCounter;
    typedef std::atomic<Boolean> AtomicFlag;
    typedef std::promise<Boolean> BooleanPromise;

    typedef std::mutex Mutex;
    typedef std::lock_guard<Mutex> LockGuard;
}

namespace { // Values

    const Data64 PI = 3.141592653589793238462643383279502884197169399;
    const uData16 ZERO = 0U;

    const uData16 EXAMPLE_DATA_NATIVE_SAMPLE_SIZE = 4;
    const uData16 EXAMPLE_DATA_ARRAY_SIZE = 100;
    const uData16 ONE_HUNDRED_CENTIMETERS_PER_SECONDS = 2778;

    const uData16 DEFAULT_FWHM = 22;
    const uData16 DEFAULT_SAMPLING_TIME = 2;

}

namespace { // Functions
    /**
     * @brief Allows to start the various JoinableThreads in the constructors without blocking anything
     */
    void voidAction() {}

    /**
     * @brief Throws a runtime error with the specified message
     * @param message that will be in the error
     */
    [[noreturn]] void throwIllegalActionException(char const* message) {
        throw std::runtime_error(message);
    }

}

/**
 * @brief A NativeSample is an array of 4 unsigned shorts in which the raw data from a physical sensor (referred as NativeData) will be stored.
 * @note This is currently only valid for example purposes.
 */
typedef std::array<uData16, EXAMPLE_DATA_NATIVE_SAMPLE_SIZE> NativeSample;

/**
 * @brief A Sample is an array of 100 unsigned shorts in which data computed by Spectre is placed for transport.
 * @note This is currently only valid for example purposes.
 */
typedef std::array<uData16, EXAMPLE_DATA_ARRAY_SIZE> Sample;

namespace Defaults {
    const Sample EMPTY_SAMPLE = Sample();
    const NativeSample EMPTY_NATIVE_SAMPLE = NativeSample();
}


#endif //SPIRITSENSORGATEWAY_DATAMODELTYPEDEFINITION_H
