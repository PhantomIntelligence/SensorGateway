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

#ifndef SENSORGATEWAY_GUARDIANTRANSLATIONSTRATEGY_HPP
#define SENSORGATEWAY_GUARDIANTRANSLATIONSTRATEGY_HPP

#include "sensor-gateway/common/data-structure/gateway/GatewayStructures.h"
#include "sensor-gateway/common/data-structure/sensor/GuardianStructures.hpp"
#include "DataTranslationStrategy.hpp"
#include "TranslationErrorFactory.h"

namespace DataTranslation {

    template<class S>
    class GuardianTranslationStrategy final
            : public DataTranslationStrategy<S, GatewayStructuresFor<S>> {

    protected:

        typedef S SensorStructures;
        typedef GatewayStructuresFor<SensorStructures> GatewayStructures;

        using super = DataTranslationStrategy<SensorStructures, GatewayStructures>;

        using SensorMessage = typename super::SensorMessage;
        using SensorRawData = typename super::SensorRawData;
        using SensorControlMessage = typename super::SensorControlMessage;

        using ServerMessage = typename super::ServerMessage;
        using ServerRawData = typename super::ServerRawData;

        using super::MessageSource;
        using super::RawDataSource;
        using super::ResponseControlMessageSource;

    public:

        explicit GuardianTranslationStrategy() = default;

        ~GuardianTranslationStrategy() = default;

        void translateMessage(SensorMessage&& sensorMessage) final {
            switch (sensorMessage.id) {
                case Sensor::AWL::END_OF_FRAME:
                    translateEndOfFrameMessage(std::move(sensorMessage));
                    break;
                case Sensor::AWL::DETECTION_TRACK :
                    translateDetectionTrackMessage(std::move(sensorMessage));
                    break;
                case Sensor::AWL::DETECTION_VELOCITY :
                    translateDetectionVelocityMessage(std::move(sensorMessage));
                    break;
                default:
                    ErrorHandling::throwGuardianTranslationError(
                            "translateMessage", ErrorHandling::GatewayErrorCode::UNRECOGNIZED_GUARDIAN_MESSAGE_ID);
            }
        }

        void translateRawData(SensorRawData&& sensorRawData) final {
            orderRawData(&sensorRawData);

            // Guardian sends little-endian raw data. They should become big-endian for Gateway
            reverseRawDataDefinitionEndianness(&sensorRawData);

            ServerRawData translatedRawData(sensorRawData.content);
            super::RawDataSource::produce(std::move(translatedRawData));
        }


        SensorMessage translateControlMessageToSensorMessageRequest(SensorControlMessage&& sensorControlMessage) final {
            // TODO
            SensorMessage sensorMessage;
            return sensorMessage;
        }

        SensorControlMessage translateSensorMessageToControlMessageResult(SensorMessage&& sensorMessage) final {
            // TODO
            SensorControlMessage sensorControlMessage;
            return sensorControlMessage;
        }

        using super::MessageSource::linkConsumer;
        using super::RawDataSource::linkConsumer;
        using super::ResponseControlMessageSource::linkConsumer;

    private:

        using super::currentOutputMessage;

        void addTrackInPixel(SensorMessage&& sensorMessage, DataFlow::PixelId pixelId) {
            DataFlow::TrackId trackId = castLittleEndianBytesToUInt16InHostEndianness(sensorMessage.data[0],
                                                                                      sensorMessage.data[1]);
            DataFlow::ConfidenceLevel confidenceLevel = sensorMessage.data[5];
            DataFlow::Intensity intensity = castLittleEndianBytesToUInt16InHostEndianness(sensorMessage.data[6],
                                                                                          sensorMessage.data[7]);
            DataFlow::Track track;
            track.id = trackId;
            track.confidenceLevel = confidenceLevel;
            track.intensity = intensity;
            currentOutputMessage.addTrackToPixelWithId(pixelId, std::move(track));
        };

        void translateEndOfFrameMessage(SensorMessage&& sensorMessage) {
            DataFlow::MessageId messageId = castLittleEndianBytesToUInt16InHostEndianness(sensorMessage.data[0],
                                                                                          sensorMessage.data[1]);
            DataFlow::SensorId sensorId = castLittleEndianBytesToUInt16InHostEndianness(sensorMessage.data[2],
                                                                                        sensorMessage.data[3]);
            currentOutputMessage.sensorId = sensorId;
            currentOutputMessage.messageId = messageId;
            super::MessageSource::produce(std::move(currentOutputMessage));
            currentOutputMessage = ServerMessage::returnDefaultData();
        }

        void translateDetectionTrackMessage(SensorMessage&& sensorMessage) {
            DataFlow::PixelId pixelId = castLittleEndianBytesToUInt16InHostEndianness(sensorMessage.data[3],
                                                                                      sensorMessage.data[4]);
            addTrackInPixel(std::move(sensorMessage), pixelId);
        }

        void translateDetectionVelocityMessage(SensorMessage&& sensorMessage) {
            DataFlow::Distance distance = castLittleEndianBytesToUInt16InHostEndianness(sensorMessage.data[2],
                                                                                        sensorMessage.data[3]);
            DataFlow::Speed speed = castLittleEndianBytesToInt16InHostEndianness(sensorMessage.data[4],
                                                                                 sensorMessage.data[5]);
            DataFlow::Acceleration acceleration = castLittleEndianBytesToInt16InHostEndianness(sensorMessage.data[6],
                                                                                               sensorMessage.data[7]);
            DataFlow::TrackId trackId = castLittleEndianBytesToUInt16InHostEndianness(sensorMessage.data[0],
                                                                                      sensorMessage.data[1]);
            auto track = fetchTrack(trackId);
            track->distance = distance;
            track->speed = speed;
            track->acceleration = acceleration;
        }

        void reverseRawDataDefinitionEndianness(SensorRawData* sensorRawData) {
            auto originalContent = typename ServerRawData::Content(sensorRawData->content);
            auto const NUMBER_OF_DATA = SensorRawData::Definitions::SIZE;
            for (auto contentIndex = 0u; contentIndex < NUMBER_OF_DATA; ++contentIndex) {
                sensorRawData->content[contentIndex] = reverseEndiannessOfInt16(originalContent[contentIndex]);
            }
        }

        void orderRawData(SensorRawData* sensorRawData) {

            static constexpr auto const NUMBER_OF_SAMPLES_PER_CHANNEL = SensorRawData::Definitions::NUMBER_OF_SAMPLES_PER_CHANNEL;
            static constexpr auto const NUMBER_OF_CHANNELS = SensorRawData::Definitions::NUMBER_OF_CHANNELS;
            static constexpr std::array<uint32_t, NUMBER_OF_CHANNELS> CHANNEL_POSITIONS = {
                    {8, 0, 9, 1, 10, 2, 11, 3, 12, 4, 13, 5, 14, 6, 15, 7}
            };
            auto unorderedContent = typename ServerRawData::Content(sensorRawData->content);
            for (auto ordinalChannelIndex = 0u; ordinalChannelIndex < NUMBER_OF_CHANNELS; ++ordinalChannelIndex) {
                auto channelPositionIndex = CHANNEL_POSITIONS[ordinalChannelIndex];
                auto originStartPosition =
                        unorderedContent.begin() + channelPositionIndex * NUMBER_OF_SAMPLES_PER_CHANNEL;
                auto destinationStartPosition =
                        sensorRawData->content.begin() + ordinalChannelIndex * NUMBER_OF_SAMPLES_PER_CHANNEL;
                std::copy_n(originStartPosition, NUMBER_OF_SAMPLES_PER_CHANNEL, destinationStartPosition);
            }
        }

        DataFlow::Track* fetchTrack(DataFlow::TrackId const& trackId) {
            auto pixels = currentOutputMessage.getPixels();
            for (auto i = 0; i < Sensor::Guardian::NUMBER_OF_PIXELS; ++i) {
                auto pixel = &pixels->at(i);
                if (pixel->doesTrackExist(trackId)) {
                    return pixel->fetchTrackById(trackId);
                }
            }
            return nullptr;
        }


    };
}
#endif //SENSORGATEWAY_GUARDIANTRANSLATIONSTRATEGY_HPP
