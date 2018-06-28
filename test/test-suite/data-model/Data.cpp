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

#include "data-model/Data.h"

using ExampleDataModel::Data;
using ExampleDataModel::Complement;
using Defaults::EMPTY_SAMPLE;

Data::Data(ProcessedData const& processed, Complement const& complement) :
        ProcessedData(processed),
        convolutedDetection(complement.convolutedDetection) {}


Data::Data(Data const& other) :
        ProcessedData(other),
        convolutedDetection(other.convolutedDetection) {}

Data::Data(Data&& other) noexcept:
        ProcessedData(std::move(other)),
        convolutedDetection(std::move(other.convolutedDetection)) {
    other.convolutedDetection = EMPTY_SAMPLE;
}

void Data::swap(Data& current, Data& other) noexcept {
    ProcessedData::swap(current, other);
    std::swap(current.convolutedDetection, other.convolutedDetection);
}


Sample const& Data::getConvolutedDetection() const {
    return convolutedDetection;
}

bool Data::operator==(Data const& other) const {
    auto sameProcessedData = static_cast<ProcessedData>(*this).operator==(static_cast<ProcessedData>(other));
    auto sameConvolutedDetection = getConvolutedDetection() == other.getConvolutedDetection();
    bool equal = (sameProcessedData && sameConvolutedDetection);
    return equal;
}

bool Data::operator!=(Data const& other) const {
    return !operator==(other);
}

Data const& Data::returnDefaultData() noexcept {
    return Defaults::DEFAULT_DATA;
}
