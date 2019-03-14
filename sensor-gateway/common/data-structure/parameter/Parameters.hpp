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

#include "sensor-gateway/common/data-structure/ControlMessage.hpp"

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

            using ControlMessagePayload = std::array<Byte, SENSOR_INTERNAL_TOTAL_LENGTH_IN_BITS / 8>;
        };


        struct ParameterMetadata {
            std::string const name;
            std::string const unit;
        };


        template<typename SensorParameterDefinition>
        class Parameter {

        protected:

            using Definition = SensorParameterDefinition;
            using Name = typename Definition::Name;
            using Unit = typename Definition::Unit;
            using ControlMessagePayload = typename Definition::ControlMessagePayload;

        public:

            explicit Parameter()
//                   : value(),
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

            auto extractMetadata() const noexcept -> ParameterMetadata {
                return {.name = getStringifiedName(), .unit = getStringifiedUnit()};
            }

            auto createGetParameterValueControlMessagePayload() const noexcept -> ControlMessagePayload{
                ControlMessagePayload controlMessagePayload;
                return controlMessagePayload;
            }

        };


        template<class... P>
        class Parameters {

        protected:

            using Params = std::tuple<P...>;

            template<size_t N>
            using getParam = typename std::tuple_element<N, Params>::type;

        public:

            static auto const NUMBER_OF_AVAILABLE_PARAMETERS = sizeof...(P);

            explicit Parameters() : internalParameters(std::make_tuple(P()...)) {}

            constexpr auto isAvailable(std::string const& parameterName) const noexcept {
                auto names = getNames();
                bool containsParameterWithName = std::any_of(names.cbegin(), names.cend(), NameEquals(parameterName));
                return containsParameterWithName;
            }

            /**
             * @warning It is assumed that parameterName correspond to an existing name of parameter in this container
             */
            auto createGetParameterValueControlMessageFor(std::string const& parameterName) const {
                auto const index = getIndexFor(parameterName);
                auto const controlMessagePayloads = createGetParameterValueControlMessagePayloads();
                auto const controlMessagePayload = controlMessagePayloads[index];
                using MessagePayload = decltype(controlMessagePayload);
                using SensorControlMessage = typename DataFlow::ControlMessage<ControlMessageDefinition<typename MessagePayload::value_type, controlMessagePayload.size()>>;
                auto code = SensorControlMessage::ControlMessageCode::GET_VALUE;
                SensorControlMessage controlMessage(code, controlMessagePayload);
                return controlMessage;
            }

            /**
             * @warning It is assumed that parameterName correspond to an existing name of parameter in this container
             */
            constexpr auto getMetadataFor(std::string const& parameterName) const {
                auto const index = getIndexFor(parameterName);
                auto const metadata = convertTupleToArray(getMetadataTuple());
                return metadata[index];
            }

            constexpr auto getNames() const noexcept {
                auto names = convertTupleToArray(getNameTuple());
                return names;
            }

            constexpr auto getUnits() const {
                auto unitsTuple = index_apply<NUMBER_OF_AVAILABLE_PARAMETERS>(
                        [&](auto... Indices) {
                            return std::make_tuple(getParam<Indices>::getStringifiedUnit()...);
                        });
                return convertTupleToArray(unitsTuple);
            }

        private:

            constexpr auto const getIndexFor(std::string const& parameterName) const {
                auto names = getNames();
                auto iterator = std::find_if(names.cbegin(), names.cend(), NameEquals(parameterName));
                auto index = std::distance(names.cbegin(), iterator);
                return index;
            }

            constexpr auto getNameTuple() const noexcept {
                auto nameTuple = index_apply<NUMBER_OF_AVAILABLE_PARAMETERS>(
                        [&](auto... Indices) {
                            return std::make_tuple(getParam<Indices>::getStringifiedName()...);
                        });
                return nameTuple;
            }

            struct NameEquals {
                std::string const value;

                NameEquals(std::string const& s) : value(s) {}

                bool const operator()(std::string const& s) const {
                    return s == value;
                }
            };

            constexpr auto getMetadataTuple() const {
                auto typeTuple = index_apply<NUMBER_OF_AVAILABLE_PARAMETERS>(
                        [&](auto... Indices) {
                            return std::make_tuple(std::get<Indices>(internalParameters).extractMetadata()...);
                        });
                return typeTuple;
            }

            constexpr auto createGetParameterValueControlMessagePayloads() const noexcept {
                auto controlMessages = convertTupleToArray(createGetParameterValueControlMessagesPayloadTuple());
                return controlMessages;
            }

            constexpr auto createGetParameterValueControlMessagesPayloadTuple() const noexcept {
                auto getParameterValueControlMessages = index_apply<NUMBER_OF_AVAILABLE_PARAMETERS>(
                        [&](auto... Indices) {
                            return std::make_tuple(std::get<Indices>(internalParameters).createGetParameterValueControlMessagePayload()...);
                        });
                return getParameterValueControlMessages;
            }

            Params internalParameters;
        };
    }
}

#endif //SENSORGATEWAY_PARAMETERS_HPP
