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

#include "sensor-gateway/common/ConstantFunctionsDefinition.h"

namespace Sensor {
    namespace Gateway {

        template<
                typename N,
                typename T,
                typename U
        >
        struct GatewayParameterDefinition {
            using Name = N;
            using ValueType = T;
            using Unit = U;
        };

        // WARNING: Do not remove this to-do unless a more general structure has been created
        // TODO : take revision # into account when creating Parameters list definition
        template<
                typename GatewayParameterDefinition,
                Byte SENSOR_INTERNAL_COMMAND,
                Byte SENSOR_INTERNAL_ADDRESS,
                uint8_t SENSOR_INTERNAL_TOTAL_LENGTH_IN_BITS,
                uint8_t PARAMETER_VALUE_OFFSET_IN_BITS,
                uint8_t PARAMETER_VALUE_LENGTH_IN_BITS
        >
        struct SensorParameterDefinition {
            using Name = typename GatewayParameterDefinition::Name;
            using ValueType = typename GatewayParameterDefinition::ValueType;
            using Unit = typename GatewayParameterDefinition::Unit;
            static Byte const sensorInternalCommand = SENSOR_INTERNAL_COMMAND;
            static Byte const sensorInternalAddress = SENSOR_INTERNAL_ADDRESS;
            static uint8_t const sensorInternalTotalLengthInBits = SENSOR_INTERNAL_TOTAL_LENGTH_IN_BITS;
            static uint8_t const valueOffsetInBits = PARAMETER_VALUE_OFFSET_IN_BITS;
            static uint8_t const valueLengthInBits = PARAMETER_VALUE_LENGTH_IN_BITS;
        };

        template<typename SensorParameterDefinition>
        class Parameter {

        protected:

            using Definition = SensorParameterDefinition;
            using Name = typename Definition::Name;
            using Unit = typename Definition::Unit;

        public:

            explicit Parameter()
//                   : value()
            {};

            static constexpr bool nameEquals(std::string const& otherName) {
                return getStringifiedName() == otherName;
            };

            static constexpr std::string const& getStringifiedName() noexcept {
                return Name::toString();
            }

            static constexpr std::string const& getStringifiedUnit() noexcept {
                return Unit::toString();
            }

        private:

//            typename Definition::ValueType const value;
        };


        template<class... P>
        class Parameters {

            using Params = std::tuple<P...>;

        public:

            static auto const NUMBER_OF_AVAILABLE_PARAMETERS = sizeof...(P);

            explicit Parameters() : internalParameters(std::make_tuple(P()...)) {}

            constexpr auto isAvailable(std::string const& parameterName) {
                auto names = getNames();
                bool containsParameterWithName = false;
                for (size_t i = 0; i < NUMBER_OF_AVAILABLE_PARAMETERS && !containsParameterWithName; ++i) {
                    containsParameterWithName = containsParameterWithName || names[i] == parameterName;
                }

                return containsParameterWithName;
            }

            constexpr auto getNames() const {
                auto names = convertTupleToArray(getNameTuple());
                return names;
            }

            constexpr auto getUnits() const {
                auto unitsTuple = index_apply<NUMBER_OF_AVAILABLE_PARAMETERS>(
                        [&](auto... Is) {
                            return std::make_tuple(std::get<Is>(internalParameters).getStringifiedUnit()...);
                        });
                return convertTupleToArray(unitsTuple);
            }

        private:

            constexpr auto getNameTuple() const {
                auto nameTuple = index_apply<NUMBER_OF_AVAILABLE_PARAMETERS>(
                        [&](auto... Is) {
                            return std::make_tuple(std::get<Is>(internalParameters).getStringifiedName()...);
                        });
                return nameTuple;
            }

            Params internalParameters;
        };
    }
}

#endif //SENSORGATEWAY_PARAMETERS_HPP
