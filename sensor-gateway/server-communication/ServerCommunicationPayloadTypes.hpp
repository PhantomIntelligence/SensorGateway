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

#ifndef SENSORGATEWAY_SENSORCOMMUNICATIONPAYLOADTYPES_HPP
#define SENSORGATEWAY_SENSORCOMMUNICATIONPAYLOADTYPES_HPP

#include "sensor-gateway/common/data-structure/gateway/SensorMessage.hpp"

namespace ServerCommunication {

    namespace PayloadTypes {


        using UnsignedIntegerParameterPayloadUnit = uint64_t;
        using SignedIntegerParameterPayloadUnit = int64_t;
        using RealNumberParameterPayloadUnit = double_t;
        using BooleanParameterPayloadUnit = bool;

        namespace Details {

            template<typename N, typename ValueType, ValueType v>
            struct NameValuePayload {
                static N const name;
                static ValueType const value = v;
            };
        }

        template<bool success = true>
        class StringPayload {

        public:

            using Type = std::string;
            using ContentType = std::string;

            explicit StringPayload(std::string const& value) noexcept : value(value) {
            }

            explicit StringPayload() noexcept :
                    StringPayload(StringPayload::returnDefaultData()) {
            }

            ~StringPayload() noexcept = default;

            StringPayload(StringPayload const& other) :
                    StringPayload(other.value) {
            }

            StringPayload(StringPayload&& other) noexcept :
                    StringPayload(std::move(other.value)) {
            }

            StringPayload& operator=(StringPayload const& other)& {
                StringPayload temporary(other);
                swap(*this, temporary);
                return *this;
            }

            StringPayload& operator=(StringPayload&& other)& noexcept {
                swap(*this, other);
                return *this;
            }

            void swap(StringPayload& current, StringPayload& other) noexcept {
                std::swap(current.value, other.value);
            }

            bool operator==(StringPayload const& other) const {
                auto sameName = (value == other.value);
                return sameName;
            }

            bool operator!=(StringPayload const& other) const {
                return !operator==(other);
            }

            static inline StringPayload const& returnDefaultData() noexcept;

            std::string const& getValue() const noexcept {
                return value;
            }

            std::string const& getName() const noexcept {
                return value;
            }

            std::string const& toString() const noexcept {
                return value;
            }

            static constexpr bool isSuccess() noexcept {
                return success;
            }

        private:

            std::string value;
        };

        namespace Defaults {
            template<bool success>
            StringPayload<success> const DEFAULT_STRING_PAYLOAD("");
        }

        template<bool success>
        inline StringPayload<success> const& StringPayload<success>::returnDefaultData() noexcept {
            return Defaults::DEFAULT_STRING_PAYLOAD<success>;
        }

        namespace Details {
            template<typename T, typename Content, bool success = true>
            class SimplePayload {

            public:

                using Type = T;
                using ContentType = Content;

                explicit SimplePayload(Type const& value) noexcept : value(value) {
                }

                explicit SimplePayload() noexcept :
                        SimplePayload(SimplePayload::returnDefaultData()) {
                }

                ~SimplePayload() noexcept = default;

                SimplePayload(SimplePayload const& other) :
                        SimplePayload(other.value) {
                }

                SimplePayload(SimplePayload&& other) noexcept :
                        SimplePayload(std::move(other.value)) {
                }

                SimplePayload& operator=(SimplePayload const& other)& {
                    SimplePayload temporary(other);
                    swap(*this, temporary);
                    return *this;
                }

                SimplePayload& operator=(SimplePayload&& other)& noexcept {
                    swap(*this, other);
                    return *this;
                }

                void swap(SimplePayload& current, SimplePayload& other) noexcept {
                    std::swap(current.value, other.value);
                }

                bool operator==(SimplePayload const& other) const {
                    auto sameName = (value == other.value);
                    return sameName;
                }

                bool operator!=(SimplePayload const& other) const {
                    return !operator==(other);
                }

                static inline SimplePayload const& returnDefaultData() noexcept;

                // here we assume that value it not a fundamental type, it is necessarily a std::tuple<string, fundamental, string> (see ParameterType below)
                std::string const& getName() const noexcept {
                    return std::get<0>(value);
                }

                // here we assume that value it not a fundamental type, it is necessarily a std::tuple<string, fundamental, string> (see ParameterType below)
                std::string toString() const noexcept {
                    auto stringedPayload = std::string(
                            getName() + " : " +
                            std::to_string(std::get<1>(value)) + " " +
                            std::get<2>(value)
                    );
                    return stringedPayload;
                }

                Type const& getValue() const noexcept {
                    return value;
                }

                ContentType const& getContentValue() const noexcept {
                    return std::get<ContentType>(value);
                }

                static constexpr bool isSuccess() noexcept {
                    return success;
                }

            private:

                Type value;
            };

            namespace Defaults {
                template<typename T, typename Content, bool success>
                T const DEFAULT_SIMPLE_PAYLOAD_VALUE{};

                template<typename T, typename Content, bool success>
                SimplePayload<T, Content, success> const DEFAULT_SIMPLE_PAYLOAD(
                        DEFAULT_SIMPLE_PAYLOAD_VALUE<T, Content, success>);
            }

            template<typename T, typename Content, bool success>
            inline SimplePayload<T, Content, success> const& SimplePayload<T, Content, success>::returnDefaultData() noexcept {
                return Defaults::DEFAULT_SIMPLE_PAYLOAD<T, Content, success>;
            }
        }

        namespace Details {
            using ParameterMetadataType = std::tuple<std::string, std::string>;

            template<size_t numberOfParameters>
            using AllMetadata = std::array<ParameterMetadataType, numberOfParameters>;

            template<size_t N>
            using AllParameterMetadataPayload = SimplePayload<AllMetadata<N>, AllMetadata<N>>;

            template<typename T>
            using ParameterType = std::tuple<std::string, T, std::string>;

            template<typename T, bool success = true>
            using ParameterPayload = SimplePayload<ParameterType<T>, T, success>;

            using UnsignedIntegerParameterPayload =  ParameterPayload<UnsignedIntegerParameterPayloadUnit>;
            using SignedIntegerParameterPayload =  ParameterPayload<SignedIntegerParameterPayloadUnit>;
            using RealNumberParameterPayload =  ParameterPayload<RealNumberParameterPayloadUnit>;
            using BooleanParameterPayload =  ParameterPayload<BooleanParameterPayloadUnit>;

            using MessagePayload = StringPayload<true>;
            using SuccessPayload = StringPayload<true>;
            using ErrorPayload = StringPayload<false>;

            using ParameterErrorPayload = ParameterPayload<ErrorPayload, false>;
        }

        using Details::UnsignedIntegerParameterPayload;
        using Details::SignedIntegerParameterPayload;
        using Details::RealNumberParameterPayload;
        using Details::BooleanParameterPayload;
        using Details::ParameterErrorPayload;
        using Details::MessagePayload;
        using Details::SuccessPayload;
        using Details::ErrorPayload;

    }

    template<typename SetParameterValueRequest>
    using ParameterPayload = typename PayloadTypes::Details::ParameterPayload<typename SetParameterValueRequest::Payload>;

    template<typename SetParameterValueRequest>
    using ParameterPayloadContent = typename ParameterPayload<SetParameterValueRequest>::Type;

    template<typename SetParameterValueRequest>
    using SetParameterUnit = typename ParameterPayload<SetParameterValueRequest>::ContentType;
}


#endif //SENSORGATEWAY_SENSORCOMMUNICATIONPAYLOADTYPES_HPP
