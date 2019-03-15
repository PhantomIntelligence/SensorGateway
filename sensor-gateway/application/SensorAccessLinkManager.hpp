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

#ifndef SENSORGATEWAY_SENSORACCESSLINKMANAGER_HPP
#define SENSORGATEWAY_SENSORACCESSLINKMANAGER_HPP

#include "SensorAccessLink.hpp"

#include "sensor-gateway/sensor-communication/KvaserCanCommunicationStrategy.hpp"
#include "sensor-gateway/data-translation/AWLTranslationStrategy.hpp"

#include "sensor-gateway/sensor-communication/GuardianUSBCommunicationStrategy.hpp"
#include "sensor-gateway/data-translation/GuardianTranslationStrategy.hpp"

namespace SensorGateway {

    // TODO: find a better way to create type descriptor, will facilitate SensorAccessLinkManager & other Gateway higher order of functionnalities to be implemented
//    template<typename ParameterList, typename SensorConnectionParameters>
//    struct SensorAccessLinkTraits {
//
//        template<typename P>
//        struct CommunicationStructures {
//            typedef CommunicationStructures<P> type;
//        };
//
//        using SensorConnectionParametersType = SensorConnectionParameters;
//    };
//
//    template<typename Traits, typename ParameterList>
//    struct SensorAccessLinkDescriptor {
//        using SensorStructures = typename Traits::template CommunicationStructures<ParameterList>::type;
//        using GatewayStructures = GatewayStructuresFor<SensorStructures>;
//        using AccessLink = Details::SensorAccessLink<SensorStructures, GatewayStructures>;
//
//        using SensorConnectionParameterType = typename Traits::SensorConnectionParameterType;
//    };

    template<typename SensorName,
            typename SensorStructures,
            typename DataTranslationStrategyType,
            typename SensorCommunicationStrategyType,
            typename SensorConnectionParameterDefaults
    >
    struct GenericAccessLink {

        using CommunicationStructures = SensorStructures;
        using Factory = SensorGateway::SensorAccessLinkFactory<CommunicationStructures>;
        using AccessLink = typename Factory::AccessLink;
        using GatewayStructures = typename Factory::GatewayStructures;

        // TODO : Incorporate std::is_same and other compile-time type checking to be REALLY simplify usage
        using DataTranslationStrategy = DataTranslationStrategyType;
        using SensorCommunicationStrategy = SensorCommunicationStrategyType;

        using SensorConnectionParameterType = Bare<typename SensorConnectionParameterDefaults::value_type>;

        using type = GenericAccessLink<
                SensorName,
                SensorStructures,
                DataTranslationStrategyType,
                SensorCommunicationStrategyType,
                SensorConnectionParameterType
        >;

        static auto getName() noexcept {
            return SensorName::toString();
        }

        static SensorConnectionParameterType getDefaultSensorConnectionPointer() noexcept {
            SensorConnectionParameterType value = SensorConnectionParameterDefaults();
            return value;
        }

//        SensorCommunicationStrategy getDefaultSensorConnectionParameterValue() noexcept;

        // TODO : find a way to get this working, will be the key for full abstract SALM
//        template<typename SAL>
//        static constexpr SensorConnectionParameter getDefaultSensorConnectionParameter() noexcept {
//            static_assert(std::is_same<typename SAL::SensorAccessLinkType, type>::value,
//                          "The default sensor connection parameter for this SensorAccessLink is not correctly defined");
//            return SAL::SensorConnectionType();
//        }
    };

    // TODO : find a way to get this working, will be the key for full abstract SALM
//    template<typename SAL>
//    struct ConnectionParameterDefaultValueHolder {
//        typedef typename SAL::type SensorACcessLinkType;
//        typedef typename SAL::SensorConnectionParameterType SensorConnectionType;
//        static SensorConnectionType defaultSensorConnectionValue;
//        constexpr explicit operator SensorConnectionType() const noexcept { return defaultSensorConnectionValue; }
//    };
    template<typename SAL>
    static typename SAL::SensorConnectionParameter getDefaultSensorConnectionParameterFor() noexcept {
        typename SAL::SensorConnectionParameter sensorConnectionParameter;
        return sensorConnectionParameter;
    }

    // AWL16
    using AWL16Name = StringLiteral<decltype("AWL16"_ToString)>;
    using DefaultAWL16ConnectionParameter = std::integral_constant<int, 0>;
//    using DefaultAWL16ConnectionParameter = std::integral_constant<int, 0>;

    template<typename P>
    using AWL16Structures = typename Sensor::AWL::Structures<P>;

    template<typename P>
    using AWL16AccessLink = SensorGateway::GenericAccessLink<
            AWL16Name,
            AWL16Structures<P>,
            DataTranslation::AWLTranslationStrategy<AWL16Structures<P>>,
            SensorCommunication::KvaserCanCommunicationStrategy<AWL16Structures<P>>,
            DefaultAWL16ConnectionParameter
    >;

    // Guardian
    using GuardianName = StringLiteral<decltype("Guardian"_ToString)>;

    using DefaultGuardianConnectionParameterLiterals = SensorCommunication::USBConnectionParametersLiteral<0x058b, 0x0050, 129, 2, 3000>;
    auto defaultGuardianConnectionParameterValues = DefaultGuardianConnectionParameterLiterals::getValue();
    using DefaultGuardianConnectionParameter = std::integral_constant<decltype(defaultGuardianConnectionParameterValues)*, &defaultGuardianConnectionParameterValues>;

    template<typename P>
    using GuardianStructures = typename Sensor::Guardian::Structures<P>;

    template<typename P>
    using GuardianAccessLink = SensorGateway::GenericAccessLink<
            GuardianName,
            GuardianStructures<P>,
            DataTranslation::GuardianTranslationStrategy<GuardianStructures<P>>,
            SensorCommunication::GuardianUSBCommunicationStrategy<GuardianStructures<P>>,
            DefaultGuardianConnectionParameter
    >;

    // EVAL_ADAL6110_10
    using EVAL_ADAL6110_10_Name = StringLiteral<decltype("eval-adal6110-10"_ToString)>;
    using DefaultEVAL_ADAL6110_10_ConnectionParameterLiterals = SensorCommunication::USBConnectionParametersLiteral<0x064b, 0x7823, 129, 1, 3000>;
    auto defaultEVAL_ADAL6110_10_ConnectionParameterValues = DefaultEVAL_ADAL6110_10_ConnectionParameterLiterals::getValue();
    using DefaultEVAL_ADAL6110_10_ConnectionParameter = std::integral_constant<decltype(defaultEVAL_ADAL6110_10_ConnectionParameterValues)*, &defaultEVAL_ADAL6110_10_ConnectionParameterValues>;

    template<typename P>
    using EVAL_ADAL6110_16_Structures = typename Sensor::Guardian::Structures<P>;

    template<typename P>
    using EVAL_ADAL6110_16_AccessLink = SensorGateway::GenericAccessLink<
            EVAL_ADAL6110_10_Name,
            EVAL_ADAL6110_16_Structures<P>,
            DataTranslation::GuardianTranslationStrategy<EVAL_ADAL6110_16_Structures<P>>,
            SensorCommunication::GuardianUSBCommunicationStrategy<EVAL_ADAL6110_16_Structures<P>>,
            DefaultEVAL_ADAL6110_10_ConnectionParameter
    >;

}

#endif //SENSORGATEWAY_SENSORACCESSLINKMANAGER_HPP
