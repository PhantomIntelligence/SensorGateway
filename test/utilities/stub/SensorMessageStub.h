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

#ifndef SENSORGATEWAY_SENSORMESSAGESTUB_H
#define SENSORGATEWAY_SENSORMESSAGESTUB_H

#include "sensor-gateway/common/data-structure/gateway/SensorMessage.hpp"
#include "sensor-gateway/common/data-structure/sensor/AWLStructures.h"
#include "sensor-gateway/common/data-structure/gateway/GatewayStructures.h"

namespace Stub {

    using AWL16Structures = Sensor::AWL::Structures;
    using AWL16GatewayStructures = Sensor::Gateway::Structures<
            AWL16Structures::MessageDefinition,
            AWL16Structures::RawDataDefinition,
            AWL16Structures::ControlMessageDefinition
    >;
    using SensorMessage = AWL16GatewayStructures::Message;
    using DataFlow::MessageId;
    using DataFlow::SensorId;
    using DataFlow::Pixel;
    using DataFlow::PixelId;
    using DataFlow::Track;

    typedef std::pair<PixelId, std::vector<Track>> TracksInPixel;

    SensorMessage const addTrackToSensorMessageAtGivenPixelId(SensorMessage sensorMessage, Track track, PixelId pixelId) {
        SensorMessage sensorMessageCopy = SensorMessage(sensorMessage);
        sensorMessageCopy.addTrackToPixelWithId(pixelId, std::move(track));
        return sensorMessageCopy;
    }

    SensorMessage createSensorMessage(MessageId sensorMessageId, SensorId sensorId, std::vector<TracksInPixel> tracksInPixels) {
        SensorMessage sensorMessage = SensorMessage(sensorMessageId, sensorId, {});
        for (auto tracksInPixel : tracksInPixels) {
            for (auto track : tracksInPixel.second) {
                sensorMessage.addTrackToPixelWithId(tracksInPixel.first, std::move(track));
            }
        }
        return sensorMessage;
    }

    SensorMessage createArbitrarySensorMessage() {
        return createSensorMessage(42, 16, {TracksInPixel(0, {Track(14275, 0, 139, 0, 109, 0)}),
                                        TracksInPixel(1, {Track(42, 0, 135, 0, 110, 0)}),
                                        TracksInPixel(2, {Track(14286, 0, 125, 0, 105, 0)}),
                                        TracksInPixel(3, {Track(14270, 0, 127, 0, 106, 1)}),
                                        TracksInPixel(4, {Track(14301, 0, 122, 0, 104, 0)}),
                                        TracksInPixel(5, {Track(14272, 0, 120, 0, 108, 0)}),
                                        TracksInPixel(6, {Track(14316, 0, 114, 0, 114, 0)}),
                                        TracksInPixel(7, {Track(14321, 0, 112, 0, 117, 1)}),
                                        TracksInPixel(8, {Track(14284, 0, 126, 0, 111, 0)}),
                                        TracksInPixel(9, {Track(14289, 0, 124, 0, 113, 0)}),
                                        TracksInPixel(10, {Track(14290, 0, 125, 0, 107, 0)}),
                                        TracksInPixel(11, {Track(14291, 0, 123, 0, 106, 0)}),
                                        TracksInPixel(12, {Track(14324, 0, 91, 0, 109, 1)}),
                                        TracksInPixel(13, {Track(14302, 0, 114, 0, 109, 0)}),
                                        TracksInPixel(14, {Track(14322, 0, 109, 0, 116, -2)}),
                                        TracksInPixel(15, {Track(14321, 0, 109, 0, 113, 0)})
                });
    }
    SensorMessage createRandomIdSensorMessage() {
        std::default_random_engine randomEngine(std::random_device{}());
        std::uniform_int_distribution<uint16_t> distribution(0, 1000);
        auto randomId = distribution(randomEngine);
        return createSensorMessage(randomId, 16, {TracksInPixel(0, {Track(14275, 0, 139, 0, 109, 0)}),
                                    TracksInPixel(1, {Track(42, 0, 135, 0, 110, 0)}),
                                    TracksInPixel(2, {Track(14286, 0, 125, 0, 105, 0)}),
                                    TracksInPixel(3, {Track(14270, 0, 127, 0, 106, 1)}),
                                    TracksInPixel(4, {Track(14301, 0, 122, 0, 104, 0)}),
                                    TracksInPixel(5, {Track(14272, 0, 120, 0, 108, 0)}),
                                    TracksInPixel(6, {Track(14316, 0, 114, 0, 114, 0)}),
                                    TracksInPixel(7, {Track(14321, 0, 112, 0, 117, 1)}),
                                    TracksInPixel(8, {Track(14284, 0, 126, 0, 111, 0)}),
                                    TracksInPixel(9, {Track(14289, 0, 124, 0, 113, 0)}),
                                    TracksInPixel(10, {Track(14290, 0, 125, 0, 107, 0)}),
                                    TracksInPixel(11, {Track(14291, 0, 123, 0, 106, 0)}),
                                    TracksInPixel(12, {Track(14324, 0, 91, 0, 109, 1)}),
                                    TracksInPixel(13, {Track(14302, 0, 114, 0, 109, 0)}),
                                    TracksInPixel(14, {Track(14322, 0, 109, 0, 116, -2)}),
                                    TracksInPixel(15, {Track(14321, 0, 109, 0, 113, 0)})
        });
    }

    std::vector<SensorMessage> createSensorMessageStub() {
        std::vector<SensorMessage> SensorMessages = {
                createSensorMessage(64829, 16, {TracksInPixel(11, {Track(14291, 0, 123, 0, 106, 0)}),
                                        TracksInPixel(12, {Track(14324, 0, 93, 0, 109, 0)}),
                                        TracksInPixel(13, {Track(14302, 0, 115, 0, 109, -1)}),
                                        TracksInPixel(14, {Track(14322, 0, 104, 0, 115, -6)}),
                                        TracksInPixel(15, {Track(14321, 0, 110, 0, 117, 0)})
                            }
                ),

                createSensorMessage(64830, 16, {TracksInPixel(0, {Track(14275, 0, 139, 0, 109, 0)}),
                                        TracksInPixel(1, {Track(2, 0, 135, 0, 110, 0)}),
                                        TracksInPixel(2, {Track(14286, 0, 125, 0, 105, 0)}),
                                        TracksInPixel(3, {Track(14270, 0, 127, 0, 106, 1)}),
                                        TracksInPixel(4, {Track(14301, 0, 122, 0, 104, 0)}),
                                        TracksInPixel(5, {Track(14272, 0, 120, 0, 108, 0)}),
                                        TracksInPixel(6, {Track(14316, 0, 114, 0, 114, 0)}),
                                        TracksInPixel(7, {Track(14320, 0, 110, 0, 117, 1)}),
                                        TracksInPixel(8, {Track(14284, 0, 126, 0, 111, 0)}),
                                        TracksInPixel(9, {Track(14289, 0, 124, 0, 111, 0)}),
                                        TracksInPixel(10, {Track(14290, 0, 125, 0, 107, 0)}),
                                        TracksInPixel(11, {Track(14291, 0, 123, 0, 106, 0)}),
                                        TracksInPixel(12, {Track(14324, 0, 91, 0, 109, 1)}),
                                        TracksInPixel(13, {Track(14302, 0, 114, 0, 109, 0)}),
                                        TracksInPixel(14, {Track(14322, 0, 109, 0, 116, -2)}),
                                        TracksInPixel(15, {Track(14321, 0, 109, 0, 117, 0)})
                            }
                )
        };
        return SensorMessages;
    }
}

#endif // SENSORGATEWAY_SENSORMESSAGESTUB_H
