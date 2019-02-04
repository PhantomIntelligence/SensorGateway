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

#ifndef SENSORGATEWAY_AWLTRANSLATIONSTRATEGY_H
#define SENSORGATEWAY_AWLTRANSLATIONSTRATEGY_H

#include "sensor-gateway/common/data-structure/gateway/GatewayStructures.h"
#include "sensor-gateway/common/data-structure/sensor/AWLStructures.h"
#include "DataTranslationStrategy.hpp"

namespace DataTranslation {

    using AWLStructures = Sensor::AWL::Structures;
    using AWLGatewayStructures = Sensor::Gateway::Structures<
            AWLStructures::AWLMessageDefinition,
            AWLStructures::AWLRawDataDefinition,
            AWLStructures::AWLCommandDefinition
            >;

    class AWLTranslationStrategy final : public DataTranslationStrategy<AWLStructures, AWLGatewayStructures> {

    protected:

        using super = DataTranslationStrategy<AWLStructures, AWLGatewayStructures>;
        using super::SensorMessage;
        using super::SensorRawData;

        using super::MessageSource;
        using super::RawDataSource;

    public:

        explicit AWLTranslationStrategy() = default;

        ~AWLTranslationStrategy() = default;

        void translateMessage(SensorMessage&& sensorMessage) override;

        void translateRawData(SensorRawData&& serverRawData) override;

        static inline DataFlow::Intensity const convertIntensityToSNR(DataFlow::Intensity const& intensity) noexcept {
            DataFlow::Intensity const snr = (intensity / 2.0) - 21; // w+f
            return snr;
        }

    private:

        using super::currentOutputMessage;

        void addTrackInPixel(SensorMessage&& sensorMessage, DataFlow::PixelId pixelId);

        Track* fetchTrack(DataFlow::TrackId const& trackId);

        void translateDetectionTrackMessage(SensorMessage&& sensorMessage);

        void translateDetectionVelocityMessage(SensorMessage&& sensorMessage);

        void translateEndOfFrameMessage(SensorMessage&& sensorMessage);
    };
}
#endif //SENSORGATEWAY_AWLTRANSLATIONSTRATEGY_H
