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
                    // WARNING! THIS CASE IS NOT TESTED AUTOMATICALLY
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


        // WARNING! THIS FUNCTION IS NOT TESTED AUTOMATICALLY
        SensorMessage
        translateControlMessageToSensorMessageRequest(SensorControlMessage&& sensorControlMessage) override {
            SensorMessage sensorMessage;
            sensorMessage.id = Sensor::AWL::COMMAND_MESSAGE;
            Byte commandByte = translateControlMessageType(&sensorControlMessage);
            sensorMessage.data[0] = commandByte;

            static constexpr Byte const ALL_PIXELS_MASK = 0xFF;
            static constexpr Byte const NUMBER_OF_FRAMES_FOR_CALIBRATION = 0x64;
            static constexpr Byte const ERASE_CALIBRATION_FILE = 0x01;
            static constexpr Byte const RESERVED = 0x00;

            if (sensorControlMessage.isCalibration()) {
                sensorMessage.data[1] = ALL_PIXELS_MASK;                   // POTENTIALLY CONFIGURABLE
                sensorMessage.data[2] = NUMBER_OF_FRAMES_FOR_CALIBRATION;  // POTENTIALLY CONFIGURABLE
                sensorMessage.data[3] = RESERVED;
                // next 4 bytes --> float value == 0.1 (0x3DCCCCCD)        // POTENTIALLY CONFIGURABLE
                sensorMessage.data[4] = 0x3D;
                sensorMessage.data[5] = 0xCC;
                sensorMessage.data[6] = 0xCC;
                sensorMessage.data[7] = 0xCD;
            } else if (sensorControlMessage.isClearCalibration()) {
                sensorMessage.data[1] = ALL_PIXELS_MASK;        // POTENTIALLY CONFIGURABLE
                sensorMessage.data[2] = ERASE_CALIBRATION_FILE; // POTENTIALLY CONFIGURABLE
                sensorMessage.data[3] = RESERVED;
                sensorMessage.data[4] = RESERVED;
                sensorMessage.data[5] = RESERVED;
                sensorMessage.data[6] = RESERVED;
                sensorMessage.data[7] = RESERVED;
            } else {
                // Assumed -> direct transfer of payload
                auto payload = sensorControlMessage.getPayload().data();

                // Command Type
                sensorMessage.data[1] = payload[1];

                // Address
                uint16_t responseAddress = ((uint16_t*) payload)[1];
                insertAsLittleEndian(responseAddress, &sensorMessage.data.data()[2]);

                // Value
                uint32_t responseValue = ((uint32_t*) payload)[1];
                insertAsLittleEndian(responseValue, &sensorMessage.data.data()[4]);
            }
            sensorMessage.length = std::tuple_size<typename SensorControlMessage::Payload>::value;
            return sensorMessage;
        }

        // WARNING! THIS FUNCTION IS NOT TESTED AUTOMATICALLY
        void translateResponse(SensorMessage&& response) {
            auto controlMessage = translateSensorMessageToControlMessageResult(std::forward<SensorMessage>(response));
            super::ResponseControlMessageSource::produce(std::move(controlMessage));
        }

        // WARNING! THIS FUNCTION IS NOT TESTED AUTOMATICALLY
        SensorControlMessage translateSensorMessageToControlMessageResult(SensorMessage&& sensorMessage) override {
            static constexpr Byte const IRRELEVANT = 0x00;
            static constexpr size_t const size = SensorControlMessage::PAYLOAD_SIZE;

            auto responseData = sensorMessage.data.data();
            Byte responseControlMessageValues[size];

            // Control code
            auto code = translateByteToControlCode(responseData[0]);
            responseControlMessageValues[0] = IRRELEVANT;

            // Command Type
            Byte responseCommandType = responseData[1];
            responseControlMessageValues[1] = responseCommandType;

            // Address
            uint16_t responseAddress = ((uint16_t*) responseData)[1];
            insertAsLittleEndian(responseAddress, &responseControlMessageValues[2]);

            // Value
            uint32_t responseValue = ((uint32_t*) responseData)[1];
            insertAsLittleEndian(responseValue, &responseControlMessageValues[4]);

            using Payload = typename SensorControlMessage::Payload;
            Payload payload{
                    responseControlMessageValues[0],
                    responseControlMessageValues[1],
                    responseControlMessageValues[2],
                    responseControlMessageValues[3],
                    responseControlMessageValues[4],
                    responseControlMessageValues[5],
                    responseControlMessageValues[6],
                    responseControlMessageValues[7]
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

        // WARNING! THIS FUNCTION IS NOT TESTED AUTOMATICALLY
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

        // WARNING! THIS FUNCTION IS NOT TESTED AUTOMATICALLY
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
            DataFlow::MessageId messageId = castLittleEndianBytesToUInt16InHostEndianness(sensorMessage.data[0],
                                                                                          sensorMessage.data[1]);
            DataFlow::SensorId sensorId = castLittleEndianBytesToUInt16InHostEndianness(sensorMessage.data[2],
                                                                                        sensorMessage.data[3]);
            currentOutputMessage.messageId = messageId;
            currentOutputMessage.sensorId = sensorId;
            super::MessageSource::produce(std::move(currentOutputMessage));
            currentOutputMessage = ServerMessage::returnDefaultData();
        }

        void translateDetectionTrackMessage(SensorMessage&& sensorMessage) {
            DataFlow::PixelId pixelId = castLittleEndianBytesToUInt16InHostEndianness(sensorMessage.data[3],
                                                                                      sensorMessage.data[4]);
            addTrackInPixel(std::move(sensorMessage), pixelId);
        }

        void addTrackInPixel(SensorMessage&& sensorMessage, DataFlow::PixelId pixelId) {
            DataFlow::TrackId const trackId = castLittleEndianBytesToUInt16InHostEndianness(sensorMessage.data[0],
                                                                                            sensorMessage.data[1]);
            DataFlow::ConfidenceLevel const confidenceLevel = sensorMessage.data[5];
            DataFlow::Intensity const intensity = castLittleEndianBytesToUInt16InHostEndianness(sensorMessage.data[6],
                                                                                                sensorMessage.data[7]);
            DataFlow::Track track;
            track.id = trackId;
            track.confidenceLevel = confidenceLevel;
            track.intensity = convertIntensityToSNR(intensity);
            currentOutputMessage.addTrackToPixelWithId(pixelId, std::move(track));
        };

        void translateDetectionVelocityMessage(SensorMessage&& sensorMessage) {
            DataFlow::Distance const distance = castLittleEndianBytesToUInt16InHostEndianness(sensorMessage.data[2],
                                                                                              sensorMessage.data[3]);
            DataFlow::Speed const speed = castLittleEndianBytesToInt16InHostEndianness(sensorMessage.data[4],
                                                                                       sensorMessage.data[5]);
            DataFlow::Acceleration const acceleration = castLittleEndianBytesToInt16InHostEndianness(
                    sensorMessage.data[6],
                    sensorMessage.data[7]);
            DataFlow::TrackId const trackId = castLittleEndianBytesToUInt16InHostEndianness(sensorMessage.data[0],
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
