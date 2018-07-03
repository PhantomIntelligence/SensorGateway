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


#ifndef SPIRITSENSORGATEWAY_NATIVEDATA_H
#define SPIRITSENSORGATEWAY_NATIVEDATA_H

#include "Parameters.h"

namespace {
    const char* const SAMPLE_MUST_HAVE_A_VALUE_MESSAGE = "To access this Sample, it must have values";
}

namespace ExampleDataModel {

    struct NativeComplement {
        const NativeSample distance;
        const NativeSample velocity;
        const NativeSample intensity;
        const NativeSample confidenceLevel;
    };


    class NativeData {
    public:

        NativeData(Parameters const& parameter, NativeComplement const& complement);

        ~NativeData() = default;

        NativeData(NativeData const& other);

        NativeData(NativeData&& other) noexcept;

        NativeData& operator=(NativeData other)& {
            swap(*this, other);
            return *this;
        }

        NativeData& operator=(NativeData&& other)& noexcept {
            NativeData temporary(std::move(other));
            swap(*this, temporary);
            return *this;
        }

        /**
         *
         * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
         * @see Data.h for more details
         */
        void swap(NativeData& current, NativeData& other) noexcept;

        /**
         * @return a constant handle to the NativeData parameter
         */
        Parameters const& getParameters() const;

        /**
         * @return The position of the detections (in a 0 to 99 value representation,
         * which have to be tranformed into centimeters in order to compute more Sample)
         */
        NativeSample const& getDistance() const;

        /**
         * @return The velocity of the detections (in a 0 to 99 value representation)
         */
        NativeSample const& getVelocity() const;

        /**
         * @return The intensity of the detections (in a 0 to 99 value representation)
         */
        NativeSample const& getIntensity() const;

        /**
         * @return The confidence level with wich the detections is made (in a 0 to 99
         * value representation)
         */
        NativeSample const& getConfidenceLevel() const;

        bool operator==(NativeData const& other) const;

        bool operator!=(NativeData const& other) const;

        NativeData static const& returnDefaultData() noexcept;

    private:
        Parameters parameters;
        NativeSample distance;
        NativeSample velocity;
        NativeSample intensity;
        NativeSample confidenceLevel;
    };

}

/**
 * Default and/or empty values to avoid nullptr and allow empty buffer construction
 */
namespace Defaults {
    using ExampleDataModel::NativeComplement;
    using ExampleDataModel::NativeData;

    const NativeComplement DEFAULT_NATIVE_COMPLEMENT = NativeComplement{EMPTY_NATIVE_SAMPLE,
                                                                        EMPTY_NATIVE_SAMPLE,
                                                                        EMPTY_NATIVE_SAMPLE,
                                                                        EMPTY_NATIVE_SAMPLE};
    const NativeData DEFAULT_NATIVE_DATA = NativeData(DEFAULT_PARAMETERS, DEFAULT_NATIVE_COMPLEMENT);
}


#endif //SPIRITSENSORGATEWAY_NATIVEDATA_H
