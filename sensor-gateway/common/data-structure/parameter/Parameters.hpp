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

        struct ParameterMetadata {
            std::string name;
            std::string unit;
        };

        struct ParameterInfoForBulkSet {
            bool isOfInterest = false;
            std::string name;
            std::string unit;
            size_t valueOffsetInBits;
            size_t valueLengthInBits;
        };
//        struct ParameterMetadata {
//            std::string name{""};
//            std::string unit{""};
//        };
//
//        struct ParameterInfoForBulkSet {
//            bool isOfInterest = false;
//            std::string const name{""};
//            std::string const unit{""};
//            size_t const valueOffsetInBits = 0;
//            size_t const valueLengthInBits = 0;
//        };

    }
}

namespace {
    using Sensor::Gateway::ParameterMetadata;
}

namespace ParameterValueStatus {

    using ValueIsUnsignedInteger = bool;
    using UnsignedIntegerValue = UnsignedInteger;
    using ValueIsSignedInteger = bool;
    using SignedIntegerValue = SignedInteger;
    using ValueIsRealNumber = bool;
    using RealNumberValue = RealNumber;
    using ValueIsBoolean = bool;
    using BooleanValue = bool;

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

    struct ParameterResponseStatus {
        bool receivedError = false;
        bool receivedResponse = false;
        Sensor::Gateway::ParameterMetadata metadata = {.name = "", .unit=""};
        RequestedValue receivedValue = std::make_tuple(false, 0, false, 0, false, 0.0, false, false);
    };
}

namespace {
    using ParameterValueStatus::RequestedValue;
    using ParameterValueStatus::ParameterResponseStatus;

    using ParameterValueStatus::UnsignedIntegerValue;
    using ParameterValueStatus::SignedIntegerValue;
    using ParameterValueStatus::RealNumberValue;
    using ParameterValueStatus::BooleanValue;

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

    template<typename T>
    static constexpr auto getValue(RequestedValue const& requestedValue) -> T {
        if (isUnsignedInteger(requestedValue)) {
            return getUnsignedInteger(requestedValue);
        }
        if (isSignedInteger(requestedValue)) {
            return getSignedInteger(requestedValue);
        }
        if (isRealNumber(requestedValue)) {
            return getRealNumber(requestedValue);
        }
        if (isBoolean(requestedValue)) {
            return getBoolean(requestedValue);
        }
    }
}

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

        template<typename SensorParameterDefinition>
        class Parameter {

        protected:

            using Definition = SensorParameterDefinition;
            using Unit = typename Definition::Unit;
            using ControlMessageHeader = typename Definition::ControlMessageHeader;
            using ControlMessageData = typename Definition::ControlMessageData;
            using ControlMessagePayload = typename Definition::ControlMessagePayload;

        public:

            using Name = typename Definition::Name;
            using ValueType = typename Definition::ValueType;

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

            template<typename T>
            static constexpr bool isType() noexcept {
                return std::is_same<T, ValueType>::value;
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
                bool ofInterest = isOfInterest(currentSensorValues);
                ParameterInfoForBulkSet info{
                        .isOfInterest = ofInterest,
                        .name = Name::toString(),
                        .unit = Unit::toString(),
                        .valueOffsetInBits = Definition::valueOffsetInBits,
                        .valueLengthInBits = Definition::valueLengthInBits
                };
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

            auto isOfInterest(ControlMessagePayload const& currentSensorValues) const noexcept -> bool {
                Byte type = currentSensorValues[1];
                uint16_t address = ((uint16_t*) currentSensorValues.data())[1];
                bool isOfInterest = type == Definition::commandType &&
                                    address == Definition::internalAddress;
                std::cout
                        << "\n" << std::boolalpha
                        << "    | " << "-------------------------------" << "\n"
                        << "    | " << "-------------------------------" << "\n"
                        << "    | " << "#######> Parameter:   " << Name::toString() << "\n"
                        << "    | " << "Is of interest: " << isOfInterest << "\n"
                        << "    | " << "-------------------------------" << "\n"
                        << "    | " << "Settings type: " << Definition::commandType << "\n"
                        << "    | " << "Received type: " << type << "\n"
                        << "    | " << "Settings address: " << Definition::internalAddress << "\n"
                        << "    | " << "Settings address: " << address << "\n"
                        << "    | " << "-------------------------------" << "\n"
                        << std::endl;
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
                                            BitSetByte(
                                                    std::string(
                                                            std::to_string(bitsetValue[8 * Indices + 0]) +
                                                            std::to_string(bitsetValue[8 * Indices + 1]) +
                                                            std::to_string(bitsetValue[8 * Indices + 2]) +
                                                            std::to_string(bitsetValue[8 * Indices + 3]) +
                                                            std::to_string(bitsetValue[8 * Indices + 4]) +
                                                            std::to_string(bitsetValue[8 * Indices + 5]) +
                                                            std::to_string(bitsetValue[8 * Indices + 6]) +
                                                            std::to_string(bitsetValue[8 * Indices + 7])
                                                    )
                                            )
                                                    .to_ullong()
                                    )
                                            ...
                            };
                        });
                return bytes;
            }

        public:

            using ParameterName = std::string;
            using RequestedValue = ::RequestedValue;
            using ParameterResponseStatus = ::ParameterResponseStatus;

            using RequestedParameterNames = std::unordered_set<ParameterName>;
            using RequestedParameterValues = std::unordered_map<ParameterName, RequestedValue>;
            using ParameterResponseStatuses = std::unordered_map<ParameterName, ParameterResponseStatus>;

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

            void updateStatusForError(SensorControlMessage&& receivedSensorControlMessage,
                                      ParameterResponseStatuses* parameterResponseStatuses) const {
                ControlMessagePayload currentSensorValues = receivedSensorControlMessage.getPayloadCopy();
                std::array<ParameterInfoForBulkSet, NUMBER_OF_AVAILABLE_PARAMETERS> allParametersInfoForBulkSet =
                        fetchAllParameterInfoForBulkSet(currentSensorValues);

                for (auto& info : allParametersInfoForBulkSet) {
                    if (info.isOfInterest) {
                        auto parameterName = info.name;
                        ParameterResponseStatus errorStatus{
                                .receivedError = true,
                                .receivedResponse = false,
                                .metadata = ParameterMetadata{.name = info.name, .unit = info.unit}
                        };
                        (*parameterResponseStatuses)[parameterName] = errorStatus;
                    }
                }
            }

            /**
             * @warning It is assumed that parameterName correspond to an existing name of parameter in this container
             * @brief this function will use the receivedSensorControlMessage in the following way:
             * @param receivedSensorControlMessage Message used to update ParameterStatuses, ASSUMED .isResponse() == true
             * @param requestedParameterGetValues Set of parameter names to return the value of to the Server.
             * @param requestedParameterSetValues Map of parameter names and the desired value to send to the Sensor.
             * @param parameterResponseStatuses Map of Parameter names -> Status to send responses to the server
             * @return A tuple<hasToSendResponseToServer, hasToSendControlMessageToSensor, SensorControlMessage (to send)>
             */
            auto updateStatusForResponse(SensorControlMessage const& receivedSensorControlMessage,
                                         RequestedParameterNames* requestedParameterGetValues,
                                         RequestedParameterValues* requestedParameterSetValues,
                                         ParameterResponseStatuses* parameterResponseStatuses) const
            -> std::tuple<bool, bool, SensorControlMessage> {
                bool hasToSendResponseToServer = false;
                bool hasToSendControlMessageToSensor = false;
                ControlMessagePayload currentSensorValues = receivedSensorControlMessage.getPayloadCopy();

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
                for (auto const& info  : allParametersInfoForBulkSet) {

                    using BitSet = std::bitset<32>; // max size for AWL

                    if (info.isOfInterest) {

                        auto name = info.name;
                        size_t const valueLengthInBits{info.valueLengthInBits};
                        size_t const valueOffsetInBits{info.valueOffsetInBits};
                        // IMPORTANT!! check that a GET value was received for this parameter BEFORE checking if a SET value was received
                        bool hadReceiveARequestToGetValue =
                                requestedParameterGetValues->find(name) != requestedParameterGetValues->end();

                        if (hadReceiveARequestToGetValue) {

                            hasToSendResponseToServer = true;
                            requestedParameterGetValues->erase(name);

                            BitSet valueContainer;
                            for (auto i = 0u; i < valueLengthInBits; ++i) {
                                valueContainer[i] = payloadInBitSet[valueOffsetInBits + i];
                            }
                            auto uncastedValue = valueContainer.to_ullong();

                            bool isUnsignedInteger = isParameterType<UnsignedInteger>(name);
                            UnsignedInteger unsignedIntegerValue = isUnsignedInteger
                                                                   ? static_cast<UnsignedInteger>(uncastedValue)
                                                                   : 0u;
                            bool isSignedInteger = isParameterType<SignedInteger>(name);
                            SignedInteger signedIntegerValue = isSignedInteger
                                                               ? static_cast<SignedInteger>(uncastedValue)
                                                               : 0;
                            bool isRealNumber = isParameterType<RealNumber>(name);
                            RealNumber realNumberValue = isRealNumber
                                                         ? static_cast<RealNumber>(uncastedValue)
                                                         : 0.0;
                            bool isBoolean = isParameterType<Boolean>(name);
                            Boolean booleanValue = isBoolean
                                                   ? static_cast<Boolean>(uncastedValue)
                                                   : false;
                            RequestedValue parameterValue = std::make_tuple(
                                    isUnsignedInteger,
                                    unsignedIntegerValue,
                                    isSignedInteger,
                                    signedIntegerValue,
                                    isRealNumber,
                                    realNumberValue,
                                    isBoolean,
                                    booleanValue
                            );


                            (*parameterResponseStatuses)[name] = ParameterResponseStatus{
                                    .receivedError = false,
                                    .receivedResponse = true,
                                    .metadata = ParameterMetadata{.name = name, .unit = info.unit},
                                    .receivedValue = parameterValue
                            };
                        }

                        // IMPORTANT!! check that a GET value was received for this parameter BEFORE checking if a SET value was received
                        bool hadReceiveARequestToSetValue =
                                requestedParameterSetValues->find(name) != requestedParameterSetValues->end();

                        if (hadReceiveARequestToSetValue) {
                            hasToSendControlMessageToSensor = true;
                            auto requestedValue = requestedParameterSetValues->at(name);
                            requestedParameterSetValues->erase(name);
                            requestedParameterGetValues->insert(name);

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

                            for (auto i = 0u; i < valueLengthInBits; ++i) {
                                payloadInBitSet[valueOffsetInBits + i] = valueContainer[i];
                            }

                        }
                    }
                }
                valuePayload = toByteArray(payloadInBitSet);

                for (auto i = 4u; i < currentSensorValues.size(); ++i) {
                    currentSensorValues[i] = valuePayload[i];
                }

                ControlCode code = ControlCode::SET_VALUE;

                SensorControlMessage controlMessage(code, currentSensorValues);
                return std::make_tuple(hasToSendResponseToServer, hasToSendControlMessageToSensor, controlMessage);
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

            template<typename T>
            constexpr auto isParameterType(ParameterName const& parameterName) const -> bool {
                auto isType = index_apply<NUMBER_OF_AVAILABLE_PARAMETERS>(
                        [&](auto... Indices) {
                            return std::array<bool, NUMBER_OF_AVAILABLE_PARAMETERS>{
                                    getParam<Indices>::template isType<T>()...
                            };
                        });
                auto index = getIndexFor(parameterName);
                return isType[index];
            }

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

            constexpr auto
            fetchAllParameterInfoForBulkSet(ControlMessagePayload const& currentSensorValues) const noexcept {
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

