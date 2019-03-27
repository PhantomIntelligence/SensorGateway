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
                Byte COMMAND_TYPE,
                uint16_t INTERNAL_ADDRESS,
                uint8_t SENSOR_COMMUNICATION_HEADER_LENGTH_IN_BITS,
                uint8_t SENSOR_INTERNAL_DATA_LENGTH_IN_BITS,
                uint8_t PARAMETER_VALUE_OFFSET_IN_BITS,
                uint8_t PARAMETER_VALUE_LENGTH_IN_BITS
        >
        struct SensorParameterDefinition {
            using Name = typename GatewayParameterDefinition::Name;
            using ValueType = typename GatewayParameterDefinition::ValueType;
            using Unit = typename GatewayParameterDefinition::Unit;
            static Byte const commandType = COMMAND_TYPE;
            static uint16_t const internalAddress = INTERNAL_ADDRESS;
            static uint8_t const sensorCommunicationHeaderLengthInBits = SENSOR_COMMUNICATION_HEADER_LENGTH_IN_BITS;
            static uint8_t const sensorInternalDataLengthInBits = SENSOR_INTERNAL_DATA_LENGTH_IN_BITS;
            static uint8_t const totalControlMessageLengthInBits =
                    sensorCommunicationHeaderLengthInBits + sensorInternalDataLengthInBits;
            static uint8_t const valueOffsetInBits = PARAMETER_VALUE_OFFSET_IN_BITS;
            static uint8_t const valueLengthInBits = PARAMETER_VALUE_LENGTH_IN_BITS;

            using ControlMessageHeader = std::array<Byte, sensorCommunicationHeaderLengthInBits / 8>;
            using ControlMessageData = std::array<Byte, sensorInternalDataLengthInBits / 8>;
            using ControlMessagePayload = std::array<Byte, totalControlMessageLengthInBits / 8>;
        };


        struct ParameterMetadata {
            std::string const name;
            std::string const unit;
        };

        struct ParameterInfoForBulkSet {
            bool isOfInterest;
            std::string const name;
            uint8_t const valueOffsetInBits;
            uint8_t const valueLengthInBits;
        };

        template<typename SensorParameterDefinition>
        class Parameter {

        protected:

            using Definition = SensorParameterDefinition;
            using ValueType = typename Definition::ValueType;
            using Unit = typename Definition::Unit;
            using ControlMessageHeader = typename Definition::ControlMessageHeader;
            using ControlMessageData = typename Definition::ControlMessageData;
            using ControlMessagePayload = typename Definition::ControlMessagePayload;

        public:

            using Name = typename Definition::Name;

            explicit Parameter() {
            };

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

            auto createGetParameterValueControlMessagePayload() const noexcept -> ControlMessagePayload {
                auto header = getControlMessageHeader();
                auto data = createControlMessageDataForGet();
                ControlMessagePayload controlMessagePayload;

                auto payloadIterator = controlMessagePayload.begin();
                auto numberOfBytesInHeader = header.size();
                std::copy_n(header.begin(), numberOfBytesInHeader, payloadIterator);
                for (auto i = 0u; i < numberOfBytesInHeader; ++i) {
                    ++payloadIterator;
                }
                std::copy_n(data.begin(), data.size(), payloadIterator);
                return controlMessagePayload;
            }

            auto fetchParameterInfoForBulkSet(ControlMessagePayload const& currentSensorValues) const noexcept
            -> ParameterInfoForBulkSet {
                ParameterInfoForBulkSet info = createParameterInfoForBulkSet();
                info.isOfInterest = isOfInterest(currentSensorValues);
                return info;
            };

        private:

            // Currently works for AWL16
            static constexpr auto getControlMessageHeader() noexcept -> ControlMessageHeader {
                ControlMessageHeader controlMessageHeader{};
                controlMessageHeader[0] = 0; //Reserved for later
                controlMessageHeader[1] = Definition::commandType;
                // 2 should be the address size in byte in this context... --> ok for AWL 16 only
                Byte address[2]{
                        ((Byte*) &Definition::internalAddress)[0],
                        ((Byte*) &Definition::internalAddress)[1]
                };
                controlMessageHeader[2] = address[0];
                controlMessageHeader[3] = address[1];
                return controlMessageHeader;
            }

            static constexpr auto createControlMessageDataForGet() noexcept -> ControlMessageData {
                ControlMessageData controlMessageData{};
                controlMessageData.fill(0);
                return controlMessageData;
            }

            auto createParameterInfoForBulkSet() noexcept -> ParameterInfoForBulkSet {
                ParameterInfoForBulkSet parameterInfoForBulkSet{
                        false,
                        Name::toString(),
                        Definition::valueOffsetInBits,
                        Definition::valueLengthInBits
                };
                return parameterInfoForBulkSet;
            }

            auto isOfInterest(ControlMessagePayload const& currentSensorValues) const noexcept -> bool {
                Byte type = currentSensorValues[1];
                uint16_t address = ((uint16_t*) currentSensorValues.data())[1];
                bool isOfInterest = type == Definition::commandType &&
                                    address == Definition::internalAddress;
                return isOfInterest;
            };
        };


        template<class... P>
        class Parameters {

        protected:

            using Params = std::tuple<P...>;

            template<size_t N>
            using getParam = typename std::tuple_element_t<N, Params>;

            using FirstParameterType = getParam<0>; // usefull for type introspection, since all parameters should return types of similar attributes

            using ControlMessagePayload =
            decltype(std::declval<FirstParameterType>().createGetParameterValueControlMessagePayload());

            using ControlMessagePayloadType = typename ControlMessagePayload::value_type;
            static constexpr size_t const ControlMessagePayloadSize = std::tuple_size<ControlMessagePayload>::value;

            using SensorControlMessage = typename DataFlow::ControlMessage<
                    ControlMessageDefinition<
                            typename ControlMessagePayload::value_type,
                            ControlMessagePayloadSize
                    >
            >;

            using ControlCode = typename SensorControlMessage::ControlMessageCode;


            using ValueIsUnsignedInteger = bool;
            using UnsignedIntegerValue = UnsignedInteger;
            using ValueIsSignedInteger = bool;
            using SignedIntegerValue = SignedInteger;
            using ValueIsRealNumber = bool;
            using RealNumberValue = RealNumber;
            using ValueIsBoolean = bool;
            using BooleanValue = bool;

            template<size_t N>
            static constexpr auto toBitSet(std::array<Byte, N> byteArray) -> std::bitset<N * 8> {
                constexpr size_t numberOfBits = N * 8;
                constexpr size_t numberOfBytes = N;
                using BitSetByte = std::bitset<8>;
                auto bitSetBytes = index_apply<numberOfBytes>(
                        [&](auto... Indices) {
                            return std::array<BitSetByte, numberOfBytes>{
                                    BitSetByte(byteArray[Indices]) ...
                            };
                        });
                std::bitset<numberOfBits> bitSet;
                for (auto index = 0u; index < numberOfBits; ++index) {
                    auto byteIndex = index / 8;
                    auto bitIndex = index % 8;
                    bitSet[index] = bitSetBytes[byteIndex][bitIndex];
                }
                return bitSet;
            }

            template<size_t N>
            static constexpr auto toByteArray(std::bitset<N> bitsetValue) -> std::array<Byte, N / 8> {
                static_assert(N > 7, "For conversion to work, the bitset be at least of 8 bits! --> 1 byte");
                static_assert(N % 8 == 0,
                              "For conversion to work, the bitset must have a number of bit divisible by 8! (to convert to bytes)");
                constexpr size_t numberOfBytes = N / 8;
                using BitSetByte = std::bitset<8>;
                auto bytes = index_apply<numberOfBytes>(
                        [&](auto... Indices) {
                            return std::array<Byte, numberOfBytes>{
                                    static_cast<Byte>(
                                            BitSetByte(bitsetValue[8 * Indices],
                                                       bitsetValue[8 * Indices + 1],
                                                       bitsetValue[8 * Indices + 2],
                                                       bitsetValue[8 * Indices + 3],
                                                       bitsetValue[8 * Indices + 4],
                                                       bitsetValue[8 * Indices + 5],
                                                       bitsetValue[8 * Indices + 6],
                                                       bitsetValue[8 * Indices + 7])
                                                    .to_ulong()
                                    )
                                            ...
                            };
                        });
                return bytes;
            }

        public:

            // Helpers for creating set request
            using ParameterName = std::string;

            using RequestedValue = std::tuple<
                    ValueIsUnsignedInteger,
                    UnsignedIntegerValue,
                    ValueIsSignedInteger,
                    SignedIntegerValue,
                    ValueIsRealNumber,
                    RealNumberValue,
                    ValueIsBoolean,
                    BooleanValue
            >;

            static constexpr auto isUnsignedInteger(RequestedValue const& requestedValue) -> bool {
                return std::get<0>(requestedValue);
            }

            static constexpr auto getUnsignedInteger(RequestedValue const& requestedValue) -> UnsignedIntegerValue {
                return std::get<1>(requestedValue);
            }

            static constexpr auto isSignedInteger(RequestedValue const& requestedValue) -> bool {
                return std::get<2>(requestedValue);
            }

            static constexpr auto getSignedInteger(RequestedValue const& requestedValue) -> SignedIntegerValue {
                return std::get<3>(requestedValue);
            }

            static constexpr auto isRealNumber(RequestedValue const& requestedValue) -> bool {
                return std::get<4>(requestedValue);
            }

            static constexpr auto getRealNumber(RequestedValue const& requestedValue) -> RealNumberValue {
                return std::get<5>(requestedValue);
            }

            static constexpr auto isBoolean(RequestedValue const& requestedValue) -> bool {
                return std::get<6>(requestedValue);
            }

            static constexpr auto getBoolean(RequestedValue const& requestedValue) -> BooleanValue {
                return std::get<7>(requestedValue);
            }


            using RequestedParameterValues = std::unordered_map<ParameterName, RequestedValue>;

            static constexpr auto const NUMBER_OF_AVAILABLE_PARAMETERS = sizeof...(P);
            using AllParameterMetadata = std::array<ParameterMetadata, NUMBER_OF_AVAILABLE_PARAMETERS>;

            explicit Parameters() : internalParameters(std::make_tuple(P()...)) {
            }

            constexpr auto isAvailable(ParameterName const& parameterName) const noexcept {
                auto names = getNames();
                bool containsParameterWithName = std::any_of(names.cbegin(), names.cend(), NameEquals(parameterName));
                return containsParameterWithName;
            }

            /**
             * @warning It is assumed that parameterName correspond to an existing name of parameter in this container
             */
            auto createGetParameterValueControlMessageFor(ParameterName const& parameterName) const {
                auto const index = getIndexFor(parameterName);
                auto const controlMessagePayloads = createGetParameterValueControlMessagePayloads();

                ControlMessagePayload const controlMessagePayload = controlMessagePayloads[index];
                ControlCode code = ControlCode::GET_VALUE;

                SensorControlMessage controlMessage(code, controlMessagePayload);
                return controlMessage;
            }

            /**
             * @warning It is assumed that parameterName correspond to an existing name of parameter in this container
             * @warning also this function might modify the RequestedParameterValues. Please apply multi-threaded protection before calling if needed
             */
            auto createSetParameterValueControlMessageFor(ControlMessagePayload currentSensorValues,
                                                          RequestedParameterValues* requestedParameterValues) const
            -> std::tuple<bool, ControlMessagePayload> {

                bool hasAnyParameterOfInterest = false;
                std::array<ParameterInfoForBulkSet, NUMBER_OF_AVAILABLE_PARAMETERS> allParametersInfoForBulkSet =
                        fetchAllParameterInfoForBulkSet(currentSensorValues);

                // works for AWL16, improve for other sensors
                std::array<Byte, 4> valuePayload{
                        currentSensorValues[4],
                        currentSensorValues[5],
                        currentSensorValues[6],
                        currentSensorValues[7]
                };
                auto payloadInBitSet = toBitSet(valuePayload);
                for (auto& info : allParametersInfoForBulkSet) {
                    if (info.isOfInterest) {
                        hasAnyParameterOfInterest = true;
                        try {
                            auto requestedValue = requestedParameterValues->at(info.name);
                            requestedParameterValues->erase(info.name);

                            using BitSet = std::bitset<info.valueLengthInBits>;

                            BitSet valueContainer;
                            if (isUnsignedInteger(requestedValue)) {
                                auto value = getUnsignedInteger(requestedValue);
                                valueContainer = BitSet(value);
                            } else if (isSignedInteger(requestedValue)) {
                                auto value = getSignedInteger(requestedValue);
                                valueContainer = BitSet(value);
                            } else if (isRealNumber(requestedValue)) {
                                auto value = getRealNumber(requestedValue);
                                valueContainer = BitSet(value);
                            } else if (isBoolean(requestedValue)) {
                                auto value = getBoolean(requestedValue);
                                valueContainer = BitSet(value);
                            } else {
                                // todo: throw, this is error
                            }

                            for (auto i = 0u; i < info.valueLengthInBits; ++i) {
                                payloadInBitSet[info.valueOffsetInBits + i] = valueContainer[i];
                            }

                        } catch (std::out_of_range& outOfRange) {
                            // Do nothing, this happens in case the parameter was of interest but no new value was requested for it.
                        }
                    }
                }
                valuePayload = toByteArray(payloadInBitSet);

                for (auto i = 4u; i < currentSensorValues.size(); ++i) {
                    currentSensorValues[i] = valuePayload[i];
                }

                ControlCode code = ControlCode::SET_VALUE;

                SensorControlMessage controlMessage(code, currentSensorValues);
                return std::make_tuple(hasAnyParameterOfInterest, controlMessage);
            }

            /**
             * @warning It is assumed that parameterName correspond to an existing name of parameter in this container
             */
            constexpr auto getMetadataFor(ParameterName const& parameterName) const {
                auto const index = getIndexFor(parameterName);
                auto const metadata = getMetadataForAllParameters();
                return metadata[index];
            }

            /**
             * @warning It is assumed that parameterName correspond to an existing name of parameter in this container
             */
            constexpr auto getMetadataForAllParameters() const {
                auto const metadata = convertTupleToArray(getMetadataTuple());
                return metadata;
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

            constexpr auto const getIndexFor(ParameterName const& parameterName) const {
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
                ParameterName const value;

                NameEquals(ParameterName const& s) : value(s) {
                }

                bool const operator()(ParameterName const& s) const {
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
                            return std::make_tuple(std::get<Indices>(internalParameters)
                                                           .createGetParameterValueControlMessagePayload()...);
                        });
                return getParameterValueControlMessages;
            }

            constexpr auto fetchAllParameterInfoForBulkSet(ControlMessagePayload const& currentSensorValues) const noexcept {
                auto allParameterInfoForBulkSet = index_apply<NUMBER_OF_AVAILABLE_PARAMETERS>(
                        [&](auto... Indices) {
                            return std::array<ParameterInfoForBulkSet, NUMBER_OF_AVAILABLE_PARAMETERS>{
                                    std::get<Indices>(internalParameters)
                                            .fetchParameterInfoForBulkSet(currentSensorValues)
                                            ...
                            };
                        });
                return allParameterInfoForBulkSet;
            }

            Params internalParameters;
        };
    }
}

#endif //SENSORGATEWAY_PARAMETERS_HPP
