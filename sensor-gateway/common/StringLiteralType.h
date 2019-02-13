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

#ifndef SENSORGATEWAY_STRINGLITERALTYPE_H
#define SENSORGATEWAY_STRINGLITERALTYPE_H

#include "TypeDefinition.h"

/**
 * @warning: DO NOT MODIFY
 * @brief: Internal type to allow easy conversion of string to literal type. This is used to pass a string as template parameter.
 */
namespace CustomType {

    template<char... chars>
    using StringType = std::integer_sequence<char, chars...>;

    template<typename T, T... chars>
    constexpr StringType<chars...> operator ""_ToString() { return {}; }

    template<typename>
    struct StringLiteral;

    template<char... elements>
    struct StringLiteral<StringType<elements...>> {
        static char const* toString_internals() {
            static constexpr char stringValue[sizeof...(elements) + 1] = {elements..., '\0'};
            return stringValue;
        }

        static std::string const& toString() {
            static std::string const string = toString_internals();
            return string;
        }
    };
}

using namespace CustomType;

#endif //SENSORGATEWAY_STRINGLITERALTYPE_H
