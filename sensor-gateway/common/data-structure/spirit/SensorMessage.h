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

#ifndef SENSORGATEWAY_FRAME_H
#define SENSORGATEWAY_FRAME_H

#include "Pixel.h"

using DataFlow::Pixel;
using Sensor::AWL::_16::NUMBER_OF_PIXELS_IN_FRAME;

namespace DataFlow {
    using PixelsArray =  std::array<Pixel, NUMBER_OF_PIXELS_IN_FRAME>;
}

namespace DataFlow {

    class SensorMessage {

    public:

        explicit SensorMessage(MessageId messageId, SensorId sensorId, PixelsArray pixels);

        SensorMessage();

        ~SensorMessage() = default;

        SensorMessage(SensorMessage const& other);

        SensorMessage(SensorMessage&& other) noexcept;

        SensorMessage& operator=(SensorMessage const& other)&;

        SensorMessage& operator=(SensorMessage&& other)& noexcept;

        static void swap(SensorMessage& current, SensorMessage& other) noexcept;

        bool operator==(SensorMessage const& other) const;

        bool operator!=(SensorMessage const& other) const;

        static SensorMessage const& returnDefaultData() noexcept;

        void addTrackToPixelWithId(PixelId const& pixelId, Track&& trackToAdd);

        std::string const getSensorIdentifier() const noexcept;

        PixelsArray* getPixels();

        MessageId messageId;
        SensorId sensorId;

    private:

        PixelsArray pixels;

        void updatePixelId(PixelId const& pixelId);
    };
}

namespace Defaults {
    namespace SensorMessage {
        using DataFlow::SensorMessage;
        using DataFlow::MessageId;
        using DataFlow::SensorId;
        using DataFlow::PixelsArray;

        /**
         * @warning Carelessly modifying these values WILL cause a huge performance drop.
         * If a value is modified here, be sure it homologous value in the communication protocol schema file.
         * @see https://github.com/PhantomIntelligence/GatewayProtocol.git
         */
        MessageId const DEFAULT_MESSAGE_ID = 0;
        SensorId const DEFAULT_SENSOR_ID = -1;
        PixelsArray const DEFAULT_PIXELS_ARRAY = PixelsArray();
        SensorMessage const DEFAULT_SENSOR_MESSAGE = SensorMessage(DEFAULT_MESSAGE_ID, DEFAULT_SENSOR_ID, DEFAULT_PIXELS_ARRAY);
    }
}

#endif //SENSORGATEWAY_FRAME_H
