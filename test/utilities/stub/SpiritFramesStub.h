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
#ifndef SPIRITSENSORGATEWAY_SPIRITFRAMESSTUB_H
#define SPIRITSENSORGATEWAY_SPIRITFRAMESSTUB_H

#include "spirit-sensor-gateway/domain/Frame.h"

namespace Stub {

    using DataFlow::Frame;
    using DataFlow::FrameID;
    using DataFlow::SystemID;
    using DataFlow::Pixel;
    using DataFlow::PixelID;
    using DataFlow::Track;

    typedef std::pair<PixelID, std::vector<Track>> TracksInPixel;

    Frame const addTrackToFrameAtGivenPixelID(Frame frame, Track track, PixelID pixelID) {
        Frame frameCopy = Frame(frame);
        frameCopy.addTrackToPixelWithID(pixelID, std::move(track));
        return frameCopy;
    }

    Frame createFrame(FrameID frameID, SystemID systemID, std::vector<TracksInPixel> tracksInPixels) {
        Frame frame = Frame(frameID, systemID, {});
        for (auto tracksInPixel : tracksInPixels) {
            for (auto track : tracksInPixel.second) {
                frame.addTrackToPixelWithID(tracksInPixel.first, std::move(track));
            }
        }
        return frame;
    }

    Frame createArbitrarySpiritFrame() {
        return createFrame(42, 16, {TracksInPixel(0, {Track(14275, 0, 139, 0, 109, 0)}),
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

    std::vector<Frame> createSpiritFramesStub() {
        std::vector<Frame> spiritFrames = {
                createFrame(64829, 16, {TracksInPixel(11, {Track(14291, 0, 123, 0, 106, 0)}),
                                        TracksInPixel(12, {Track(14324, 0, 93, 0, 109, 0)}),
                                        TracksInPixel(13, {Track(14302, 0, 115, 0, 109, -1)}),
                                        TracksInPixel(14, {Track(14322, 0, 104, 0, 115, -6)}),
                                        TracksInPixel(15, {Track(14321, 0, 110, 0, 117, 0)})
                            }
                ),

                createFrame(64830, 16, {TracksInPixel(0, {Track(14275, 0, 139, 0, 109, 0)}),
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
        return spiritFrames;
    }
}

#endif // SPIRITSENSORGATEWAY_SPIRITFRAMESSTUB_H
