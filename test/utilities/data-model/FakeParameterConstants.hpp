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

#ifndef SENSORGATEWAY_FAKEPARAMETERCONSTANTS_HPP
#define SENSORGATEWAY_FAKEPARAMETERCONSTANTS_HPP

#include "sensor-gateway/common/data-structure/parameter/ParameterConstants.hpp"

namespace Sensor {
    namespace FakeParameter {

        static size_t const PARAMETER_CONTROL_MESSAGE_PAYLOAD_LENGTH_IN_BITS = 32;
        namespace Details {
            namespace NameLiterals {
                using Impossible = StringLiteral<decltype("A Truth Saying Politician"_ToString)>;
                using UnsignedInteger = StringLiteral<decltype("UnsignedInteger"_ToString)>;
                using SignedInteger = StringLiteral<decltype("SignedInteger"_ToString)>;
                using RealNumber = StringLiteral<decltype("RealNumber"_ToString)>;
                using Boolean = StringLiteral<decltype("Boolean"_ToString)>;
                using Apple = StringLiteral<decltype("Apple"_ToString)>;
                using Pear = StringLiteral<decltype("Pear"_ToString)>;
                using Banana = StringLiteral<decltype("Banana"_ToString)>;
                using Avocado = StringLiteral<decltype("Avocado"_ToString)>;
                using Grape = StringLiteral<decltype("Grape"_ToString)>;
                using Orange = StringLiteral<decltype("Orange"_ToString)>;
                using Egg = StringLiteral<decltype("Egg"_ToString)>;
                using Carrot = StringLiteral<decltype("Carrot"_ToString)>;
            }

            template<typename N, typename T, typename V>
            struct MaskedParameter : public Sensor::Gateway::Parameter<Sensor::Gateway::SensorParameterDefinition<
                   Sensor::Gateway::GatewayParameterDefinition<N, T, V>, 0x0, 0x0, PARAMETER_CONTROL_MESSAGE_PAYLOAD_LENGTH_IN_BITS, 0x0, 0x0> > {
            };

            template<typename N>
            struct NameOnlyParameter : public MaskedParameter<
                    N,
                    Sensor::Parameter::Types::Boolean,
                    Sensor::Parameter::Units::not_applicable
            > {
            };
        }
        using MaskedUnsignedIntegerParameter = Details::MaskedParameter<
                Details::NameLiterals::UnsignedInteger,
                Sensor::Parameter::Types::UnsignedInteger,
                Sensor::Parameter::Units::not_applicable
        >;
        using MaskedSignedIntegerParameter = Details::MaskedParameter<
                Details::NameLiterals::SignedInteger,
                Sensor::Parameter::Types::SignedInteger,
                Sensor::Parameter::Units::not_applicable
        >;
        using MaskedRealNumberParameter = Details::MaskedParameter<
                Details::NameLiterals::RealNumber,
                Sensor::Parameter::Types::RealNumber,
                Sensor::Parameter::Units::not_applicable
        >;
        using MaskedBooleanParameter = Details::MaskedParameter<
                Details::NameLiterals::Boolean,
                Sensor::Parameter::Types::Boolean,
                Sensor::Parameter::Units::not_applicable
        >;

        using Impossible = Details::NameOnlyParameter<Details::NameLiterals::Impossible>;

        using Apple = Details::NameOnlyParameter<Details::NameLiterals::Apple>;
        using Pear = Details::NameOnlyParameter<Details::NameLiterals::Pear>;
        using Banana = Details::NameOnlyParameter<Details::NameLiterals::Banana>;
        using Avocado = Details::NameOnlyParameter<Details::NameLiterals::Avocado>;
        using Grape = Details::NameOnlyParameter<Details::NameLiterals::Grape>;
        using Orange = Details::NameOnlyParameter<Details::NameLiterals::Orange>;
        using FruitBasketParameters = Sensor::Gateway::Parameters<
                Apple,
                Pear,
                Banana,
                Avocado,
                Grape,
                Orange
        >;

        using Egg = Details::NameOnlyParameter<Details::NameLiterals::Egg>;
        using Carrot = Details::NameOnlyParameter<Details::NameLiterals::Carrot>;

        // IMPORTANT! : Keep this list up to date with declaration of FakeParameters. It MUST NOT be used in any Sensor::Communication::DataStructures declaration.
        using AllFakeParameters = Sensor::Gateway::Parameters<
                Impossible,
                MaskedUnsignedIntegerParameter,
                MaskedSignedIntegerParameter,
                MaskedRealNumberParameter,
                MaskedBooleanParameter,
                Apple,
                Pear,
                Banana,
                Avocado,
                Grape,
                Orange,
                Egg,
                Carrot
        >;
    }
}

#endif //SENSORGATEWAY_FAKEPARAMETERCONSTANTS_HPP
