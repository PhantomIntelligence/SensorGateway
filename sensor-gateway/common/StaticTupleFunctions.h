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

#ifndef SENSORGATEWAY_STATICTUPLEFUNCTION_H
#define SENSORGATEWAY_STATICTUPLEFUNCTION_H

#include "ConstantValuesDefinition.h"

namespace {

    template<typename T>
    using Bare = typename std::remove_cv<typename std::remove_reference<T>::type>::type;

    template<typename Tuple, size_t... Indices>
    std::array<typename std::tuple_element<0, Bare<Tuple>>::type,
            std::tuple_size<Bare<Tuple>>::value>
    convertTupleToArray(Tuple&& tuple, std::index_sequence<Indices...>) {
        return {{std::get<Indices>(std::forward<Tuple>(tuple))...}};
    }

    template<typename Tuple>
    auto convertTupleToArray(Tuple&& tuple)
    -> decltype(convertTupleToArray(std::declval<Tuple>(),
                                    std::make_index_sequence<std::tuple_size<Bare<Tuple>>::value>{})) {
        return convertTupleToArray(std::forward<Tuple>(tuple),
                                   std::make_index_sequence<std::tuple_size<Bare<Tuple>>::value>{});
    }

    // Tuple and static index function helper
    template<class F, size_t... Indices>
    constexpr auto index_apply_impl(F f, std::index_sequence<Indices...>) {
        return f(std::integral_constant<size_t, Indices>{}...);
    }

    // Tuple and static index function helper
    template<size_t N, class F>
    constexpr auto index_apply(F f) {
        return index_apply_impl(f, std::make_index_sequence<N>{});
    }

    // Tuple and static index function
    template<class Tuple, class F>
    constexpr auto apply(Tuple t, F f) {
        return index_apply<std::tuple_size<Tuple>{}>(
                [&](auto... Indices) { return f(std::get<Indices>(t)...); }
        );
    }

}

#endif //SENSORGATEWAY_STATICTUPLEFUNCTION_H
