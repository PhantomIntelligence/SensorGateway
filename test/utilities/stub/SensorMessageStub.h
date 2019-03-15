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

    using AWL16Structures = Sensor::AWL::Structures<>;
    using AWL16GatewayStructures = GatewayStructuresFor<AWL16Structures>;
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
        return createSensorMessage(42, 16, {TracksInPixel(0, {Track(14275, 109, 139, 0, 0, 0)}),
                                        TracksInPixel(1, {Track(42, 110, 135, 0, 0, 0)}),
                                        TracksInPixel(2, {Track(14286, 105, 125, 0, 0, 0)}),
                                        TracksInPixel(3, {Track(14270, 106, 127, 1, 0, 0)}),
                                        TracksInPixel(4, {Track(14301, 104, 122, 0, 0, 0)}),
                                        TracksInPixel(5, {Track(14272, 108, 120, 0, 0, 0)}),
                                        TracksInPixel(6, {Track(14316, 114, 114, 0, 0, 0)}),
                                        TracksInPixel(7, {Track(14321, 117, 112, 1, 0, 0)}),
                                        TracksInPixel(8, {Track(14284, 111, 126, 0, 0, 0)}),
                                        TracksInPixel(9, {Track(14289, 113, 124, 0, 0, 0)}),
                                        TracksInPixel(10, {Track(14290, 107, 125, 0, 0, 0)}),
                                        TracksInPixel(11, {Track(14291, 106, 123, 0, 0, 0)}),
                                        TracksInPixel(12, {Track(14324, 109, 91, 1, 0, 0)}),
                                        TracksInPixel(13, {Track(14302, 109, 114, 0, 0, 0)}),
                                        TracksInPixel(14, {Track(14322, 116, 109, -2, 0, 0)}),
                                        TracksInPixel(15, {Track(14321, 113, 109, 0, 0, 0)})
                });
    }
    SensorMessage createRandomIdSensorMessage() {
        std::default_random_engine randomEngine(std::random_device{}());
        std::uniform_int_distribution<uint16_t> distribution(0, 1000);
        auto randomId = distribution(randomEngine);
        return createSensorMessage(randomId, 16, {TracksInPixel(0, {Track(14275, 109, 139, 0, 0, 0)}),
                                    TracksInPixel(1, {Track(42, 110, 135, 0, 0, 0)}),
                                    TracksInPixel(2, {Track(14286, 105, 125, 0, 0, 0)}),
                                    TracksInPixel(3, {Track(14270, 106, 127, 1, 0, 0)}),
                                    TracksInPixel(4, {Track(14301, 104, 122, 0, 0, 0)}),
                                    TracksInPixel(5, {Track(14272, 108, 120, 0, 0, 0)}),
                                    TracksInPixel(6, {Track(14316, 114, 114, 0, 0, 0)}),
                                    TracksInPixel(7, {Track(14321, 117, 112, 1, 0, 0)}),
                                    TracksInPixel(8, {Track(14284, 111, 126, 0, 0, 0)}),
                                    TracksInPixel(9, {Track(14289, 113, 124, 0, 0, 0)}),
                                    TracksInPixel(10, {Track(14290, 107, 125, 0, 0, 0)}),
                                    TracksInPixel(11, {Track(14291, 106, 123, 0, 0, 0)}),
                                    TracksInPixel(12, {Track(14324, 109, 91, 1, 0, 0)}),
                                    TracksInPixel(13, {Track(14302, 109, 114, 0, 0, 0)}),
                                    TracksInPixel(14, {Track(14322, 116, 109, -2, 0, 0)}),
                                    TracksInPixel(15, {Track(14321, 113, 109, 0, 0, 0)})
        });
    }

    std::vector<SensorMessage> createSensorMessageStub() {
        std::vector<SensorMessage> SensorMessages = {
                createSensorMessage(64829, 16, {TracksInPixel(11, {Track(14291, 106, 123, 0, 0, 0)}),
                                        TracksInPixel(12, {Track(14324, 109, 93, 0, 0, 0)}),
                                        TracksInPixel(13, {Track(14302, 109, 115, -1, 0, 0)}),
                                        TracksInPixel(14, {Track(14322, 115, 104, -6, 0, 0)}),
                                        TracksInPixel(15, {Track(14321, 117, 110, 0, 0, 0)})
                            }
                ),

                createSensorMessage(64830, 16, {TracksInPixel(0, {Track(14275, 109, 139, 0, 0, 0)}),
                                        TracksInPixel(1, {Track(2, 110, 135, 0, 0, 0)}),
                                        TracksInPixel(2, {Track(14286, 105, 125, 0, 0, 0)}),
                                        TracksInPixel(3, {Track(14270, 106, 127, 1, 0, 0)}),
                                        TracksInPixel(4, {Track(14301, 104, 122, 0, 0, 0)}),
                                        TracksInPixel(5, {Track(14272, 108, 120, 0, 0, 0)}),
                                        TracksInPixel(6, {Track(14316, 114, 114, 0, 0, 0)}),
                                        TracksInPixel(7, {Track(14320, 117, 110, 1, 0, 0)}),
                                        TracksInPixel(8, {Track(14284, 111, 126, 0, 0, 0)}),
                                        TracksInPixel(9, {Track(14289, 111, 124, 0, 0, 0)}),
                                        TracksInPixel(10, {Track(14290, 107, 125, 0, 0, 0)}),
                                        TracksInPixel(11, {Track(14291, 106, 123, 0, 0, 0)}),
                                        TracksInPixel(12, {Track(14324, 109, 91, 1, 0, 0)}),
                                        TracksInPixel(13, {Track(14302, 109, 114, 0, 0, 0)}),
                                        TracksInPixel(14, {Track(14322, 116, 109, -2, 0, 0)}),
                                        TracksInPixel(15, {Track(14321, 117, 109, 0, 0, 0)})
                            }
                )
        };
        return SensorMessages;
    }
}

#endif // SENSORGATEWAY_SENSORMESSAGESTUB_H
