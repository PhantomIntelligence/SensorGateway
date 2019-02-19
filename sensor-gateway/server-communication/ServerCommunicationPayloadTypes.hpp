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

        class GetParameterValuePayload {

        public:

            explicit GetParameterValuePayload(std::string const& parameterName) noexcept : name(parameterName) {}

            explicit GetParameterValuePayload() noexcept :
                    GetParameterValuePayload(GetParameterValuePayload::returnDefaultData()) {}

            ~GetParameterValuePayload() noexcept = default;

            GetParameterValuePayload(GetParameterValuePayload const& other) :
                    GetParameterValuePayload(other.name) {}

            GetParameterValuePayload(GetParameterValuePayload&& other) noexcept :
                    GetParameterValuePayload(std::move(other.name)) {}

            GetParameterValuePayload& operator=(GetParameterValuePayload const& other)& {
                GetParameterValuePayload temporary(other);
                swap(*this, temporary);
                return *this;
            }

            GetParameterValuePayload& operator=(GetParameterValuePayload&& other)& noexcept {
                swap(*this, other);
                return *this;
            }

            void swap(GetParameterValuePayload& current, GetParameterValuePayload& other) noexcept {
                std::swap(current.name, other.name);
            }

            bool operator==(GetParameterValuePayload const& other) const {
                auto sameName = (name == other.name);
                return sameName;
            }

            bool operator!=(GetParameterValuePayload const& other) const {
                return !operator==(other);
            }

            static inline GetParameterValuePayload const& returnDefaultData() noexcept;

            std::string const& getName() const noexcept {
                return name;
            }

        private:

            std::string name;
        };

        namespace Defaults {
            GetParameterValuePayload const DEFAULT_GET_PARAMETER_VALUE_PAYLOAD("");
        }

        inline GetParameterValuePayload const& GetParameterValuePayload::returnDefaultData() noexcept {
            return Defaults::DEFAULT_GET_PARAMETER_VALUE_PAYLOAD;
        }

    }
}


#endif //SENSORGATEWAY_SENSORCOMMUNICATIONPAYLOADTYPES_HPP
