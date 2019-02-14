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

#ifndef SENSORGATEWAY_PARAMETERCONSTANTS_HPP
#define SENSORGATEWAY_PARAMETERCONSTANTS_HPP

#include "Parameters.hpp"

namespace Sensor {
    namespace Parameter {
        namespace Names {
            using InvalidName = StringLiteral<decltype("InvalidName"_ToString)>;

            using PulseWidth = StringLiteral<decltype("PulseWidth"_ToString)>;
            using LaserControl = StringLiteral<decltype("LaserControl"_ToString)>;
            using Accumulation = StringLiteral<decltype("Accumulation"_ToString)>;
            using TIAGain = StringLiteral<decltype("TIA Gain"_ToString)>;
        }
        namespace Types {
            using Boolean = bool;
            using RealNumber = double;
            using UnsignedInteger = uint64_t;
            using SignedInteger = int64_t;
            using Text = std::string;
        }
        namespace Units {
            // TODO: Use UCUM to correctly encode units
            using not_applicable = StringLiteral<decltype("n/a"_ToString)>;

            using step = StringLiteral<decltype("step"_ToString)>;
            using meter = StringLiteral<decltype("m"_ToString)>;
        }
    }

    namespace Gateway {

        namespace Details {
            using NoParameter = Gateway::Parameter<SensorParameterDefinition<
                    GatewayParameterDefinition<Sensor::Parameter::Names::InvalidName,
                            Sensor::Parameter::Types::Boolean,
                            Sensor::Parameter::Units::not_applicable>,
                    0x0,
                    0x0,
                    0x0,
                    0x0,
                    0x0
            >>;
        }
        using NoGatewayParameters = Gateway::Parameters<Details::NoParameter>;

        using PulseWidthDefinition = GatewayParameterDefinition<
                Sensor::Parameter::Names::PulseWidth,
                Sensor::Parameter::Types::UnsignedInteger,
                Sensor::Parameter::Units::step
        >;

        using LaserControlDefinition = GatewayParameterDefinition<
                Sensor::Parameter::Names::LaserControl,
                Sensor::Parameter::Types::Boolean,
                Sensor::Parameter::Units::not_applicable
        >;

        using AccumulationDefinition = GatewayParameterDefinition<
                Sensor::Parameter::Names::Accumulation,
                Sensor::Parameter::Types::UnsignedInteger,
                Sensor::Parameter::Units::step
        >;

        using TIAGainDefinition = GatewayParameterDefinition<
                Sensor::Parameter::Names::TIAGain,
                Sensor::Parameter::Types::Boolean,
                Sensor::Parameter::Units::not_applicable
        >;
    }
}

#endif //SENSORGATEWAY_PARAMETERCONSTANTS_HPP
