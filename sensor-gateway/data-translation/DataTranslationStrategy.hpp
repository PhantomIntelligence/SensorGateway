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
#ifndef SENSORGATEWAY_DATATRANSLATIONSTRATEGY_HPP
#define SENSORGATEWAY_DATATRANSLATIONSTRATEGY_HPP

#include "sensor-gateway/common/data-flow/DataSource.hpp"

namespace DataTranslation {

    template<class I, class O>
    class DataTranslationStrategy : public DataFlow::DataSource<O> {

    protected:
        typedef I INPUT;
        typedef O OUTPUT;

    public:
        DataTranslationStrategy() :
                currentOutputMessage(OUTPUT::returnDefaultData()) {}

        virtual ~DataTranslationStrategy() noexcept = default;

        DataTranslationStrategy(DataTranslationStrategy const& other) = delete;

        DataTranslationStrategy(DataTranslationStrategy&& other) noexcept = delete;

        DataTranslationStrategy& operator=(DataTranslationStrategy const& other)& = delete;

        DataTranslationStrategy& operator=(DataTranslationStrategy&& other)& noexcept = delete;

        virtual void translateMessage(INPUT&& inputMessage) = 0;

    protected:

        OUTPUT currentOutputMessage;
    };
}

#endif //SENSORGATEWAY_DATATRANSLATIONSTRATEGY_HPP
