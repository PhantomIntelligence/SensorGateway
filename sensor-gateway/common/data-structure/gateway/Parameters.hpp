/**
	Copyright 2014-2019 Phantom Intelligence Inc.

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

#ifndef SENSORGATEWAY_PARAMETERS_HPP
#define SENSORGATEWAY_PARAMETERS_HPP

#include "SensorMessage.hpp"
#include "sensor-gateway/common/StringLiteralType.h"

namespace Sensor {
    namespace Gateway {

        template<
                typename N,
                typename T,
                Byte SENSOR_INTERNAL_ADDRESS,
                uint8_t SENSOR_INTERNAL_TOTAL_LENGTH_IN_BITS,
                uint8_t PARAMETER_VALUE_OFFSET_IN_BITS,
                uint8_t PARAMETER_VALUE_LENGTH_IN_BITS
        >
        struct SensorParameterDefinition {
            static N const name;
            using ValueType = T;
            static Byte const sensorInternalAddress = SENSOR_INTERNAL_ADDRESS;
            static uint8_t const sensorInternalTotalLengthInBits = SENSOR_INTERNAL_TOTAL_LENGTH_IN_BITS;
            static uint8_t const valueOffsetInBits = PARAMETER_VALUE_OFFSET_IN_BITS;
            static uint8_t const valueLengthInBits = PARAMETER_VALUE_LENGTH_IN_BITS;
        };

        template<typename SensorParameterDefinition>
        class Parameter {

        public:

            explicit Parameter() :
                    name(SensorParameterDefinition::name.toString()),
                    value() {};

            constexpr std::string const& getName() const noexcept{
                return name;
            }

        private:

            std::string const name;
            typename SensorParameterDefinition::ValueType const value;
        };


        template<class... P>
        class Parameters {
            using Params = std::tuple<P...>;

        public:

            static auto const NUMBER_OF_AVAILABLE_PARAMETERS = sizeof...(P);

            explicit Parameters() : params(std::make_tuple(P()...)) {}

            constexpr auto getNames() const {
                return applyToAllParameters(
                        [&](auto... Is) {
                            return std::make_tuple(std::get<Is>(params).getName()...);
                        });
            }

        private:


            template<class F>
            constexpr auto apply(F f) {
                return applyToAllParameters(
                        [&](auto... Is) { return f(std::get<Is>(params)...); });
            }

            template<class F>
            static constexpr auto applyToAllParameters(F f) {
                return applyToAllParametersDetails(f, std::make_index_sequence<NUMBER_OF_AVAILABLE_PARAMETERS>{});
            }

            template<class F, size_t... Is>
            static constexpr auto applyToAllParametersDetails(F f, std::index_sequence<Is...>) {
                return f(std::integral_constant<size_t, Is>{}...);
            }


//            template<class Ch, class Tr, const std::size_t... Is>
//            static void printOnlyOneParameter(std::basic_ostream<Ch, Tr>& os, Params const& p, std::index_sequence<Is...>) {
//                ((os << "\n" << std::get<Is>(p));
//            }

            Params params;
        };
    }
}

#endif //SENSORGATEWAY_PARAMETERS_HPP
