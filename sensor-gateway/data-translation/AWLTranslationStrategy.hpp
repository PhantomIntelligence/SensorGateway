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
        using ControlCode = typename SensorControlMessage::ControlMessageCode;

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
                case Sensor::AWL::COMMAND_MESSAGE :
                    translateResponse(std::move(sensorMessage));
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
            SensorMessage sensorMessage;
            sensorMessage.id = Sensor::AWL::COMMAND_MESSAGE;
            Byte commandByte = translateControlMessageType(&sensorControlMessage);
            sensorMessage.data[0] = commandByte;
            bool calibration = false;

            static constexpr Byte const ALL_PIXELS_MASK = 0xFF;
            static constexpr Byte const NUMBER_OF_FRAMES_FOR_CALIBRATION = 0x64;
            static constexpr Byte const ERASE_CALIBRATION_FILE = 0x01;
            static constexpr Byte const RESERVED = 0x00;

            if (sensorControlMessage.isCalibration()) {
                sensorMessage.data[1] = ALL_PIXELS_MASK;
                sensorMessage.data[2] = NUMBER_OF_FRAMES_FOR_CALIBRATION;
                sensorMessage.data[3] = RESERVED;

                // next 4 bytes --> float value == 0.1 (0x3DCCCCCD)
                sensorMessage.data[4] = 0x3D;
                sensorMessage.data[5] = 0xCC;
                sensorMessage.data[6] = 0xCC;
                sensorMessage.data[7] = 0xCD;
            } else if (sensorControlMessage.isClearCalibration()) {
                sensorMessage.data[1] = ALL_PIXELS_MASK;
                sensorMessage.data[2] = ERASE_CALIBRATION_FILE;
                sensorMessage.data[3] = RESERVED;
                sensorMessage.data[4] = RESERVED;
                sensorMessage.data[5] = RESERVED;
                sensorMessage.data[6] = RESERVED;
                sensorMessage.data[7] = RESERVED;
            } else {
                auto payload = sensorControlMessage.getPayload();
                // Command Type
                sensorMessage.data[1] = payload[1];
                // Internal address
                sensorMessage.data[2] = payload[3];
                sensorMessage.data[3] = payload[2];
                // Value
                sensorMessage.data[4] = payload[7];
                sensorMessage.data[5] = payload[6];
                sensorMessage.data[6] = payload[5];
                sensorMessage.data[7] = payload[4];
            }
            sensorMessage.length = std::tuple_size<typename SensorControlMessage::Payload>::value;
            return sensorMessage;
        }

        void translateResponse(SensorMessage&& response) {
            auto controlMessage = translateSensorMessageToControlMessageResult(std::forward<SensorMessage>(response));
            super::ResponseControlMessageSource::produce(std::move(controlMessage));
        }

        SensorControlMessage translateSensorMessageToControlMessageResult(SensorMessage&& sensorMessage) override {

            using Payload = typename SensorControlMessage::Payload;

            auto responseData = sensorMessage.data;

            auto code = translateByteToControlCode(responseData[0]);

            static constexpr Byte const IRRELEVANT = 0x00;

            Payload payload{
                    // Has already been used for Control Code
                    IRRELEVANT,
                    // Command Type
                    responseData[1],
                    // Address
                    responseData[3],
                    responseData[2],
                    // Value
                    responseData[7],
                    responseData[6],
                    responseData[5],
                    responseData[4]
            };

            SensorControlMessage sensorControlMessage(code, payload);
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

        static constexpr Byte const SET_PARAMETER = 0xC0;
        static constexpr Byte const QUERY_PARAMETER = 0xC1;
        static constexpr Byte const RESPONSE_PARAMETER = 0xC2;
        static constexpr Byte const ERROR_PARAMETER = 0xC3;

        static constexpr Byte const RECORD_CALIBRATION = 0xDA;
        static constexpr Byte const CLEAR_CALIBRATION = 0xDB;

        Byte translateControlMessageType(SensorControlMessage* controlMessage) {
            Byte byte;
            if (controlMessage->isSet()) {
                byte = SET_PARAMETER;
            } else if (controlMessage->isGet()) {
                byte = QUERY_PARAMETER;
            } else if (controlMessage->isCalibration()) {
                byte = RECORD_CALIBRATION;
            } else if (controlMessage->isClearCalibration()) {
                byte = CLEAR_CALIBRATION;
            } else {
                byte = ERROR_PARAMETER;
            }
            return byte;
        }

        ControlCode translateByteToControlCode(Byte byte) {
            ControlCode code;
            if (byte == RESPONSE_PARAMETER) {
                code = ControlCode::RESPONSE;
            } else if (byte == ERROR_PARAMETER) {
                code = ControlCode::ERROR;
            } else {
                code = ControlCode::NOOP;
            }
            return code;
        }

        using super::currentOutputMessage;

        void translateEndOfFrameMessage(SensorMessage&& sensorMessage) {
            DataFlow::MessageId messageId = flipEndiannessAndCastAsUInt16(sensorMessage.data[0],
                                                                          sensorMessage.data[1]);
            DataFlow::SensorId sensorId = flipEndiannessAndCastAsUInt16(sensorMessage.data[2],
                                                                        sensorMessage.data[3]);
            currentOutputMessage.messageId = messageId;
            currentOutputMessage.sensorId = sensorId;
            super::MessageSource::produce(std::move(currentOutputMessage));
            currentOutputMessage = ServerMessage::returnDefaultData();
        }

        void translateDetectionTrackMessage(SensorMessage&& sensorMessage) {
            DataFlow::PixelId pixelId = flipEndiannessAndCastAsUInt16(sensorMessage.data[3],
                                                                      sensorMessage.data[4]);
            addTrackInPixel(std::move(sensorMessage), pixelId);
        }

        void addTrackInPixel(SensorMessage&& sensorMessage, DataFlow::PixelId pixelId) {
            DataFlow::TrackId const trackId = flipEndiannessAndCastAsUInt16(sensorMessage.data[0],
                                                                            sensorMessage.data[1]);
            DataFlow::ConfidenceLevel const confidenceLevel = sensorMessage.data[5];
            DataFlow::Intensity const intensity = flipEndiannessAndCastAsUInt16(sensorMessage.data[6],
                                                                                sensorMessage.data[7]);
            DataFlow::Track track;
            track.id = trackId;
            track.confidenceLevel = confidenceLevel;
            track.intensity = convertIntensityToSNR(intensity);
            currentOutputMessage.addTrackToPixelWithId(pixelId, std::move(track));
        };

        void translateDetectionVelocityMessage(SensorMessage&& sensorMessage) {
            DataFlow::Distance const distance = flipEndiannessAndCastAsUInt16(sensorMessage.data[2],
                                                                              sensorMessage.data[3]);
            DataFlow::Speed const speed = flipEndiannessAndCastAsInt16(sensorMessage.data[4],
                                                                       sensorMessage.data[5]);
            DataFlow::Acceleration const acceleration = flipEndiannessAndCastAsInt16(sensorMessage.data[6],
                                                                                     sensorMessage.data[7]);
            DataFlow::TrackId const trackId = flipEndiannessAndCastAsUInt16(sensorMessage.data[0],
                                                                            sensorMessage.data[1]);
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
