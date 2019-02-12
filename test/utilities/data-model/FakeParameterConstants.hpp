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

#include "sensor-gateway/common/data-structure/parameter/Parameters.hpp"

namespace Sensor {
    namespace FakeParameter {
        namespace Details {
            namespace NameLiterals {
                using Impossible = StringLiteral<decltype("A Truth Saying Donal Trump"_ToString)>;
            }

            template<typename N, typename T, typename V>
            struct MaskedParameter : public Sensor::Gateway::Parameter<Sensor::Gateway::SensorParameterDefinition<
                    Sensor::Gateway::GatewayParameterDefinition<N, T, V>, 0x0, 0x0, 0x0, 0x0, 0x0> > {
            };
        }

        using Impossible = Details::MaskedParameter<
                Details::NameLiterals::Impossible,
                Sensor::Parameter::Types::Boolean,
                Sensor::Parameter::Units::not_applicable
        >;


    }
}

#endif //SENSORGATEWAY_FAKEPARAMETERCONSTANTS_HPP
