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
#ifndef SENSORGATEWAY_AWLTRANSLATIONSTRATEGY_HPP
#define SENSORGATEWAY_AWLTRANSLATIONSTRATEGY_HPP

#include "sensor-gateway/common/data-structure/gateway/GatewayStructures.h"
#include "sensor-gateway/common/data-structure/sensor/AWLStructures.h"
#include "DataTranslationStrategy.hpp"
#include "TranslationErrorFactory.h"

namespace DataTranslation {

    template<class S>
    class AWLTranslationStrategy final
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

        explicit AWLTranslationStrategy() = default;

        ~AWLTranslationStrategy() = default;

        AWLTranslationStrategy(AWLTranslationStrategy const& other) = delete;

        AWLTranslationStrategy(AWLTranslationStrategy&& other) noexcept = delete;

        AWLTranslationStrategy& operator=(AWLTranslationStrategy const& other)& = delete;

        AWLTranslationStrategy& operator=(AWLTranslationStrategy&& other)& noexcept = delete;

        void translateMessage(SensorMessage&& sensorMessage) override {
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
                    ErrorHandling::throwAWLTranslationError("translateMessage",
                                                            ErrorHandling::GatewayErrorCode::UNRECOGNIZED_AWL_MESSAGE_ID);
            }
        }

        void translateRawData(SensorRawData&& serverRawData) override {
            // TODO
        }

        SensorMessage translateControlMessageToSensorMessageRequest(SensorControlMessage&& sensorControlMessage) override {
            // TODO
            SensorMessage sensorMessage;
            return sensorMessage;
        }

        SensorControlMessage translateSensorMessageToControlMessageResult(SensorMessage&& sensorMessage) override {
            // TODO
            SensorControlMessage sensorControlMessage;
            return sensorControlMessage;
        }

        static inline DataFlow::Intensity const convertIntensityToSNR(DataFlow::Intensity const& intensity) noexcept {
            DataFlow::Intensity const snr = (intensity / 2.0) - 21; // w+f
            return snr;
        }

        using super::MessageSource::linkConsumer;
        using super::RawDataSource::linkConsumer;
        using super::ResponseControlMessageSource::linkConsumer;

    private:

        using super::currentOutputMessage;

        void translateEndOfFrameMessage(SensorMessage&& sensorMessage) {
            DataFlow::MessageId messageId = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[0], sensorMessage.data[1]);
            DataFlow::SensorId sensorId = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[2], sensorMessage.data[3]);
            currentOutputMessage.messageId = messageId;
            currentOutputMessage.sensorId = sensorId;
            super::MessageSource::produce(std::move(currentOutputMessage));
            currentOutputMessage = ServerMessage::returnDefaultData();
        }

        void translateDetectionTrackMessage(SensorMessage&& sensorMessage) {
            DataFlow::PixelId pixelId = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[3], sensorMessage.data[4]);
            addTrackInPixel(std::move(sensorMessage), pixelId);
        }

        void addTrackInPixel(SensorMessage&& sensorMessage, DataFlow::PixelId pixelId) {
            DataFlow::TrackId const trackId = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[0], sensorMessage.data[1]);
            DataFlow::ConfidenceLevel const confidenceLevel = sensorMessage.data[5];
            DataFlow::Intensity const intensity = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[6],
                                                                           sensorMessage.data[7]);
            DataFlow::Track track;
            track.id = trackId;
            track.confidenceLevel = confidenceLevel;
            track.intensity = convertIntensityToSNR(intensity);
            currentOutputMessage.addTrackToPixelWithId(pixelId, std::move(track));
        };

        void translateDetectionVelocityMessage(SensorMessage&& sensorMessage) {
            DataFlow::Distance const distance = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[2], sensorMessage.data[3]);
            DataFlow::Speed const speed = convertTwoBytesToSignedBigEndian(sensorMessage.data[4], sensorMessage.data[5]);
            DataFlow::Acceleration const acceleration = convertTwoBytesToSignedBigEndian(sensorMessage.data[6],
                                                                               sensorMessage.data[7]);
            DataFlow::TrackId const trackId = convertTwoBytesToUnsignedBigEndian(sensorMessage.data[0], sensorMessage.data[1]);
            auto track = fetchTrack(trackId);
            track->distance = distance / ConversionUnits::NUMBER_OF_CENTIMETERS_IN_A_METER;
            track->speed = speed / ConversionUnits::NUMBER_OF_CENTIMETERS_IN_A_METER;
            track->acceleration = acceleration / ConversionUnits::NUMBER_OF_CENTIMETERS_IN_A_METER;
        }


        DataFlow::Track* fetchTrack(DataFlow::TrackId const& trackId) {
            auto pixels = currentOutputMessage.getPixels();
            for (auto i = 0u; i < Sensor::AWL::_16::NUMBER_OF_PIXELS; ++i) {
                auto pixel = &pixels->at(i);
                if (pixel->doesTrackExist(trackId)) {
                    return pixel->fetchTrackById(trackId);
                }
            }
            return nullptr;
        }
    };
}
#endif //SENSORGATEWAY_AWLTRANSLATIONSTRATEGY_HPP
