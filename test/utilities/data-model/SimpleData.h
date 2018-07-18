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

#ifndef SPIRITSENSORGATEWAY_SIMPLEDATA_H
#define SPIRITSENSORGATEWAY_SIMPLEDATA_H

#include "Data.h"

namespace DataModel {

    int const NUMBER_OF_SIMPLE_DATA_CONTENT = 2;

    typedef std::array<std::string, NUMBER_OF_SIMPLE_DATA_CONTENT> SimpleDataContent;

    class SimpleData {

    public:

        explicit SimpleData(SimpleDataContent simpleDataContent);

        ~SimpleData() noexcept = default;

        SimpleData(SimpleData const& other) = default;

        SimpleData(SimpleData&& other) noexcept;

        SimpleData& operator=(SimpleData const& other)& ;

        SimpleData& operator=(SimpleData&& other)& noexcept;

        void swap(SimpleData& current, SimpleData& other) noexcept;

        bool operator==(SimpleData const& other) const;

        bool operator!=(SimpleData const& other) const;

        SimpleData static const returnDefaultData() noexcept;

    private:

        SimpleDataContent content;
    };

}

namespace Defaults {
    using DataModel::SimpleData;
    using DataModel::SimpleDataContent;

    std::string const DEFAULT_FIRST_SIMPLE_DATA_CONTENT = "Some first simple data content";
    std::string const DEFAULT_SECOND_SIMPLE_DATA_CONTENT = "Some second simple data content.";
    SimpleDataContent const DEFAULT_SIMPLE_DATA_CONTENT = SimpleDataContent({DEFAULT_FIRST_SIMPLE_DATA_CONTENT,
                                                                             DEFAULT_SECOND_SIMPLE_DATA_CONTENT});
    SimpleData const DEFAULT_SIMPLE_DATA = SimpleData(DEFAULT_SIMPLE_DATA_CONTENT);
}

#endif //SPIRITSENSORGATEWAY_SIMPLEDATA_H
