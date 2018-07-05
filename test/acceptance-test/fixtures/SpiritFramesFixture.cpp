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

#include "spirit-sensor-gateway/domain/Frame.h"

using namespace SpiritProtocol; 

Track createTrack(TrackID trackID, ConfidenceLevel confidenceLevel, Intensity intensity, Acceleration acceleration,
                  Distance distance, Speed speed){
    Track track = Track(trackID, confidenceLevel, intensity);
    track.setAcceleration(acceleration);
    track.setDistance(distance);
    track.setSpeed(speed);
    return track;
}

Pixel createPixel(PixelID pixelID, std::vector<Track> tracks){
    Pixel pixel = Pixel(pixelID);
    for(auto track : tracks){
        pixel.addTrack(track);
    }
    return pixel;
}

Frame createFrame(FrameID frameID, SystemID systemID, std::vector<Pixel> pixels){
    Frame frame = Frame();
    frame.setFrameID(frameID);
    frame.setSystemID(systemID);
    for(auto pixel : pixels){
        frame.addPixel(pixel);
    }
    return frame;
}


std::vector<Frame> spiritFramesFixture = {
        createFrame(64829, 16, {createPixel(11, {createTrack(14291, 0, 123, 0, 106, 0)}),
                                createPixel(12, {createTrack(14324, 0, 93, 0, 109, 0)}),
                                createPixel(13, {createTrack(14302, 0, 115, 0, 109, 0xFFFF)}),
                                createPixel(14, {createTrack(14322, 0, 104, 0, 115, 0xFFFA)}),
                                createPixel(15, {createTrack(14321, 0, 110, 0, 117, 0)}),
        }),
        createFrame(64830, 16, {createPixel(0, {createTrack(14275, 0, 139, 0, 109, 0)}),
                                createPixel(1, {createTrack(2, 0, 135, 0, 110, 0)}),
                                createPixel(2, {createTrack(14286, 0, 125, 0, 105, 0)}),
                                createPixel(3, {createTrack(14270, 0, 127, 0, 106, 1)}),
                                createPixel(4, {createTrack(14301, 0, 122, 0, 104, 0)}),
                                createPixel(5, {createTrack(14272, 0, 120, 0, 108, 0)}),
                                createPixel(6, {createTrack(14316, 0, 114, 0, 114, 0)}),
                                createPixel(7, {createTrack(14320, 0, 110, 0, 117, 1)}),
                                createPixel(8, {createTrack(14284, 0, 126, 0, 111, 0)}),
                                createPixel(9, {createTrack(14289, 0, 124, 0, 111, 0)}),
                                createPixel(10, {createTrack(14290, 0, 125, 0, 107, 0)}),
                                createPixel(11, {createTrack(14291, 0, 123, 0, 106, 0)}),
                                createPixel(12, {createTrack(14324, 0, 91, 0, 109, 1)}),
                                createPixel(13, {createTrack(14302, 0, 114, 0, 109, 0)}),
                                createPixel(14, {createTrack(14322, 0, 109, 0, 116, 0xFFFE)}),
                                createPixel(15, {createTrack(14321, 0, 109, 0, 117, 0)}),
        }),
};