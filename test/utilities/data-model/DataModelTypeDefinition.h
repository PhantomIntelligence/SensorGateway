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
#include <functional>

namespace { // Values

    const uint16_t ZERO = 0U;

    const uint16_t EXAMPLE_DATA_NATIVE_SAMPLE_SIZE = 4;
    const uint16_t EXAMPLE_DATA_ARRAY_SIZE = 100;
    const uint16_t ONE_HUNDRED_CENTIMETERS_PER_SECONDS = 2778;

    const uint16_t DEFAULT_FWHM = 22;
    const uint16_t DEFAULT_SAMPLING_TIME = 2;

}

/**
 * @brief A NativeSample is an array of 4 unsigned shorts in which the raw data from a physical sensor (referred as NativeData) will be stored.
 * @note This is currently only valid for example purposes.
 */
typedef std::array<uint16_t, EXAMPLE_DATA_NATIVE_SAMPLE_SIZE> NativeSample;

/**
 * @brief A Sample is an array of 100 unsigned shorts in which data computed by Spectre is placed for transport.
 * @note This is currently only valid for example purposes.
 */
typedef std::array<uint16_t, EXAMPLE_DATA_ARRAY_SIZE> Sample;

namespace Defaults {
    const Sample EMPTY_SAMPLE = Sample();
    const NativeSample EMPTY_NATIVE_SAMPLE = NativeSample();
}


#endif //SPIRITSENSORGATEWAY_DATAMODELTYPEDEFINITION_H
