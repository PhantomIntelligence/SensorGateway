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

#ifndef SENSORGATEWAY_SENSORMESSAGE_HPP
#define SENSORGATEWAY_SENSORMESSAGE_HPP

#include "Pixel.h"

namespace DataFlow {

    template<typename SensorMessageDefinition>
    class SensorMessage : public SensorMessageDefinition::TimeTracking {

    protected:

        using superTimeTracking = typename SensorMessageDefinition::TimeTracking;

    public:

        using Pixels = typename SensorMessageDefinition::template Pixels<Pixel>::type;

        explicit SensorMessage(MessageId messageId, SensorId sensorId, Pixels pixels) :
                messageId(messageId), sensorId(sensorId), pixels(std::move(pixels)) {};

        SensorMessage() : SensorMessage(returnDefaultData()) {};

        ~SensorMessage() = default;

        SensorMessage(SensorMessage const& other) :
                SensorMessage(other.messageId, other.sensorId, other.pixels) {};

        SensorMessage(SensorMessage&& other) noexcept;

        SensorMessage& operator=(SensorMessage const& other)& {
            SensorMessage temporary(other);
            swap(*this, temporary);
            return *this;
        };

        SensorMessage& operator=(SensorMessage&& other)& noexcept {
            swap(*this, other);
            return *this;
        };

        static void swap(SensorMessage& current, SensorMessage& other) noexcept;

        bool operator==(SensorMessage const& other) const {
            auto sameSensorMessageId = (messageId == other.messageId);
            auto sameSensorId = (sensorId == other.sensorId);
            auto samePixels = (pixels == other.pixels);
            auto sensorMessagesAreEqual = (sameSensorMessageId && sameSensorId && samePixels);
            return sensorMessagesAreEqual;
        }

        bool operator!=(SensorMessage const& other) const {
            return !(operator==(other));
        }

        static SensorMessage const& returnDefaultData() noexcept;

        void addTrackToPixelWithId(PixelId const& pixelId, Track&& trackToAdd) {
            updatePixelId(pixelId);
            pixels[pixelId].addTrack(std::forward<Track>(trackToAdd));
        }

        std::string const getSensorIdentifier() const noexcept {
            std::ostringstream sensorIdentifierStream;
            sensorIdentifierStream << sensorId;
            std::string sensorIdentifier = sensorIdentifierStream.str();
            return sensorIdentifier;
        }

        Pixels* getPixels() {
            return &pixels;
        }

        using superTimeTracking::addTimePointForSensorWithLocation;
        using superTimeTracking::addTimePointForGatewayWithLocation;
        using superTimeTracking::getSensorTimestamps;
        using superTimeTracking::getGatewayTimestamps;

        MessageId messageId;
        SensorId sensorId;

    private:

        Pixels pixels;

        void updatePixelId(PixelId const& pixelId) {
            pixels[pixelId].id = pixelId;
        }
    };

    namespace Defaults {

        using DataFlow::SensorMessage;
        using DataFlow::MessageId;
        using DataFlow::SensorId;

        /**
         * @warning Carelessly modifying these values WILL cause a huge performance drop.
         * If a value is modified here, be sure its homologous value in the communication protocol schema file is too.
         * @see https://github.com/PhantomIntelligence/GatewayProtocol.git
         */
        MessageId const DEFAULT_MESSAGE_ID = 0;
        SensorId const DEFAULT_SENSOR_ID = -1;

        template<typename SensorMessageDefinition>
        typename SensorMessage<SensorMessageDefinition>::Pixels const DEFAULT_PIXELS_ARRAY;

        template<typename SensorMessageDefinition>
        SensorMessage<SensorMessageDefinition> const DEFAULT_SENSOR_MESSAGE = SensorMessage<SensorMessageDefinition>(
                DEFAULT_MESSAGE_ID, DEFAULT_SENSOR_ID, DEFAULT_PIXELS_ARRAY<SensorMessageDefinition>);
    }

    template<typename SensorMessageDefinition>
    SensorMessage<SensorMessageDefinition>::SensorMessage(SensorMessage<SensorMessageDefinition>&& other) noexcept:
            messageId(other.messageId),
            sensorId(other.sensorId),
            pixels(other.pixels) {
        other.messageId = Defaults::DEFAULT_MESSAGE_ID;
        other.sensorId = Defaults::DEFAULT_SENSOR_ID;
        other.pixels = Defaults::DEFAULT_PIXELS_ARRAY<SensorMessageDefinition>;
    };


    template<typename SensorMessageDefinition>
    SensorMessage<SensorMessageDefinition> const& SensorMessage<SensorMessageDefinition>::returnDefaultData() noexcept {
        return Defaults::DEFAULT_SENSOR_MESSAGE<SensorMessageDefinition>;
    }

    template<typename SensorMessageDefinition>
    void SensorMessage<SensorMessageDefinition>::swap(SensorMessage<SensorMessageDefinition>& current,
                                                      SensorMessage<SensorMessageDefinition>& other) noexcept {
        std::swap(current.messageId, other.messageId);
        std::swap(current.sensorId, other.sensorId);
        std::swap(current.pixels, other.pixels);
    };
}

#endif //SENSORGATEWAY_SENSORMESSAGE_HPP
