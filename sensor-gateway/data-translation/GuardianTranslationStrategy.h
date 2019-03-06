/**
	Copyright 2014-2018 Phantom Intelligence Inc.
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

#ifndef SENSORGATEWAY_GUARDIANTRANSLATIONSTRATEGY_H
#define SENSORGATEWAY_GUARDIANTRANSLATIONSTRATEGY_H

#include "sensor-gateway/common/data-structure/gateway/GatewayStructures.h"
#include "sensor-gateway/common/data-structure/sensor/GuardianStructures.h"
#include "DataTranslationStrategy.hpp"

namespace DataTranslation {

    using GuardianStructures = Sensor::Guardian::Structures;
    using GuardianGatewayStructures = Sensor::Gateway::Structures<
            GuardianStructures::MessageDefinition,
            GuardianStructures::RawDataDefinition,
            GuardianStructures::ControlMessageDefinition
    >;

    class GuardianTranslationStrategy final
            : public DataTranslationStrategy<GuardianStructures, GuardianGatewayStructures> {

    protected:

        using super = DataTranslationStrategy<GuardianStructures, GuardianGatewayStructures>;

        using SensorStructures = GuardianStructures;
        using super::SensorMessage;
        using super::SensorRawData;
        using super::ParameterControlMessage;

        using super::ServerRawData;

        using super::MessageSource;
        using super::RawDataSource;

    public:

        explicit GuardianTranslationStrategy() = default;

        ~GuardianTranslationStrategy() = default;

        void translateMessage(SensorMessage&& sensorMessage) override;

        void translateRawData(SensorRawData&& sensorRawData) override;

        SensorMessage
        translateControlMessageToSensorMessageRequest(ParameterControlMessage&& parameterControlMessage) override;

        ParameterControlMessage translateSensorMessageToControlMessageResult(SensorMessage&& sensorMessage) override;

    private:

        using super::currentOutputMessage;

        void addTrackInPixel(SensorMessage&& sensorMessage, DataFlow::PixelId pixelId);

        Track* fetchTrack(DataFlow::TrackId const& trackId);

        void translateDetectionTrackMessage(SensorMessage&& sensorMessage);

        void translateDetectionVelocityMessage(SensorMessage&& sensorMessage);

        void translateEndOfFrameMessage(SensorMessage&& sensorMessage);

        static void reverseRawDataDefinitionEndianness(SensorRawData* sensorRawData);

        static void orderRawData(SensorRawData* rawData);

    };
}
#endif //SENSORGATEWAY_GUARDIANTRANSLATIONSTRATEGY_H
