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


#ifndef SPIRITSENSORGATEWAY_DATASTUBS_H
#define SPIRITSENSORGATEWAY_DATASTUBS_H

#include <vector>
#include "spirit-sensor-gateway/domain/Frame.h"

using DataFlow::Track;
using DataFlow::TrackID;
using DataFlow::ConfidenceLevel;
using DataFlow::Acceleration;
using DataFlow::Distance;
using DataFlow::Speed;
using DataFlow::Intensity;
using DataFlow::Pixel;
using DataFlow::PixelID;
using DataFlow::Frame;
using DataFlow::FrameID;
using DataFlow::SystemID;

namespace Stub {

    static Track createTrack(TrackID trackID, ConfidenceLevel confidenceLevel, Intensity intensity,
                             Acceleration acceleration, Distance distance, Speed speed) {
        Track track = Track(trackID, confidenceLevel, intensity);
        track.setAcceleration(acceleration);
        track.setDistance(distance);
        track.setSpeed(speed);
        return track;
    }

    static Pixel createPixel(PixelID pixelID, std::vector<Track> tracks) {
        Pixel pixel = Pixel(pixelID);
        for (auto track : tracks) {
            pixel.addTrack(track);
        }
        return pixel;
    }

    static Frame createFrame(FrameID frameID, SystemID systemID, std::vector<Pixel> pixels = {}) {
        Frame frame = Frame();
        frame.setFrameID(frameID);
        frame.setSystemID(systemID);
        for (auto pixel : pixels) {
            frame.addPixel(pixel);
        }
        return frame;
    }
}

#endif //SPIRITSENSORGATEWAY_DATASTUBS_H