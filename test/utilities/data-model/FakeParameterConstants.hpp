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

        static size_t const PARAMETER_CONTROL_MESSAGE_PAYLOAD_LENGTH = 4;
        namespace Details {
            namespace NameLiterals {
                using Impossible = StringLiteral<decltype("A Truth Saying Politician"_ToString)>;
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
                    Sensor::Gateway::GatewayParameterDefinition<N, T, V>, 0x0, 0x0, PARAMETER_CONTROL_MESSAGE_PAYLOAD_LENGTH, 0x0, 0x0> > {
            };

            template<typename N>
            struct NameOnlyParameter : public MaskedParameter<
                    N,
                    Sensor::Parameter::Types::Boolean,
                    Sensor::Parameter::Units::not_applicable
            > {
            };
        }

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
