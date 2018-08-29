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

#include "sensor-gateway/common/data-structure/spirit/SpiritStructures.h"
#include "sensor-gateway/common/data-structure/sensor/AWLStructures.h"
#include "DataTranslationStrategy.hpp"

namespace DataTranslation {

    using AWLStructures = Sensor::AWL::Structures;
    using AWLSpiritStructures = Sensor::Spirit::Structures<AWLStructures::RawData::RawDataDefinition>;

    class AWLTranslationStrategy final : public DataTranslationStrategy<AWLStructures, AWLSpiritStructures> {
    protected:
        using super = DataTranslationStrategy<AWLStructures, AWLSpiritStructures>;
        using super::SensorMessage;
        using super::SensorRawData;

        using super::MessageSource;
        using super::RawDataSource;
    public:

        explicit AWLTranslationStrategy() = default;

        ~AWLTranslationStrategy() = default;

        void translateMessage(SensorMessage&& sensorMessage) override;

        void translateRawData(SensorRawData&& serverRawData) override;

    private:

        using super::currentOutputMessage;

        void addTrackInPixel(SensorMessage&& sensorMessage, DataFlow::PixelID pixelID);

        Track* fetchTrack(DataFlow::TrackID const& trackID);

        void translateDetectionTrackMessage(SensorMessage&& sensorMessage);

        void translateDetectionVelocityMessage(SensorMessage&& sensorMessage);

        void translateEndOfFrameMessage(SensorMessage&& sensorMessage);
    };
}
#endif //SENSORGATEWAY_AWLTRANSLATIONSTRATEGY_H
