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

        namespace Details {

            template<typename N, typename ValueType, ValueType v>
            struct NameValuePayload {
                static N const name;
                static ValueType const value = v;
            };
        }

        class StringPayload {

        public:

            explicit StringPayload(std::string const& value) noexcept : value(value) {}

            explicit StringPayload() noexcept :
                    StringPayload(StringPayload::returnDefaultData()) {}

            ~StringPayload() noexcept = default;

            StringPayload(StringPayload const& other) :
                    StringPayload(other.value) {}

            StringPayload(StringPayload&& other) noexcept :
                    StringPayload(std::move(other.value)) {}

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

        private:

            std::string value;
        };

        namespace Defaults {
            StringPayload const DEFAULT_GET_PARAMETER_VALUE_PAYLOAD("");
        }

        inline StringPayload const& StringPayload::returnDefaultData() noexcept {
            return Defaults::DEFAULT_GET_PARAMETER_VALUE_PAYLOAD;
        }
    }
}


#endif //SENSORGATEWAY_SENSORCOMMUNICATIONPAYLOADTYPES_HPP
