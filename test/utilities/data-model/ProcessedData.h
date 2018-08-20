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


#ifndef SENSORGATEWAY_PROCESSEDDATA_H
#define SENSORGATEWAY_PROCESSEDDATA_H

#include "NativeData.h"

namespace ExampleDataModel {

    struct ProcessedComplement {
        const Sample discreteDetection;
        const uint16_t samplingDistance;
    };

    class ProcessedData : public NativeData {
    public:

        ProcessedData(NativeData const& nativeData, ProcessedComplement const& complement);

        ~ProcessedData() = default;

        ProcessedData(ProcessedData const& other);

        ProcessedData(ProcessedData&& other) noexcept;

        ProcessedData& operator=(ProcessedData other)& {
            swap(*this, other);
            return *this;
        }

        ProcessedData& operator=(ProcessedData&& other)& noexcept {
            ProcessedData temporary(std::move(other));
            swap(*this, temporary);
            return *this;
        }

        /**
         *
         * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
         * @see Data.h for more details
         */
        void swap(ProcessedData& current, ProcessedData& other) noexcept;

        Sample const& getDiscreteDetections() const;

        uint16_t const& getSamplingDistance() const;

        bool operator==(ProcessedData const& other) const;

        bool operator!=(ProcessedData const& other) const;

        ProcessedData static const& returnDefaultData() noexcept;

    private:
        Sample discreteDetections;
        uint16_t samplingDistance;
    };

}

/**
 * Default and/or empty values to avoid nullptr and allow empty buffer construction
 */
namespace Defaults {
    using ExampleDataModel::ProcessedComplement;
    using ExampleDataModel::ProcessedData;

    const ProcessedComplement DEFAULT_PROCESSED_COMPLEMENT = ProcessedComplement{EMPTY_SAMPLE, ZERO};
    const ProcessedData DEFAULT_PROCESSED_DATA = ProcessedData(DEFAULT_NATIVE_DATA, DEFAULT_PROCESSED_COMPLEMENT);
}

#endif //SENSORGATEWAY_PROCESSEDDATA_H
