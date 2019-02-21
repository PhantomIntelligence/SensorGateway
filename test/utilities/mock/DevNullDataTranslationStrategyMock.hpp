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

#ifndef SENSORGATEWAY_DEVNULLDATATRANSLATIONSTRATEGYMOCK_HPP
#define SENSORGATEWAY_DEVNULLDATATRANSLATIONSTRATEGYMOCK_HPP

#include "test/utilities/data-model/DataModelFixture.h"
#include "sensor-gateway/data-translation/DataTranslationStrategy.hpp"

namespace Mock {

    template<typename T>
    class DevNullDataTranslationStrategyMock :
            public DataTranslation::DataTranslationStrategy<T, T> {

    protected:

        using super = DataTranslation::DataTranslationStrategy<T, T>;

        using super::MessageSource;
        using super::RawDataSource;

    public:

        DevNullDataTranslationStrategyMock() = default;

        ~DevNullDataTranslationStrategyMock() noexcept = default;

        void translateMessage(typename super::SensorMessage&& sensorMessage) override {};

        void translateRawData(typename super::SensorRawData&& sensorRawData) override {}

    };
}

#endif //SENSORGATEWAY_DEVNULLDATATRANSLATIONSTRATEGYMOCK_HPP