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
#ifndef SPIRITSENSORGATEWAY_JSONPARSER_CPP
#define SPIRITSENSORGATEWAY_JSONPARSER_CPP

#include "JsonConverter.h"

using ServerCommunication::JsonConverter;

using Json = nlohmann::json;


Json JsonConverter::convertTrackToJson(Track track) {
    Json jsonTrack;
    jsonTrack["trackID"] = track.ID;
    jsonTrack["intensity"] = track.intensity;
    jsonTrack["distance"] = track.distance;
    jsonTrack["confidenceLevel"] = track.confidenceLevel;
    jsonTrack["speed"] = track.speed;
    jsonTrack["acceleration"] = track.acceleration;
    return jsonTrack;
}

Json JsonConverter::convertPixelToJson(Pixel pixel) {
    Json jsonPixel;
    jsonPixel["pixelID"] = pixel.ID;
    jsonPixel["angleStart"] = pixel.angleStart;
    jsonPixel["angleEnd"] = pixel.angleEnd;
    jsonPixel["numberOfTracksInPixel"] = pixel.getCurrentNumberOfTracksInPixel();
    Json pixelTracks;
    for (Track track:*pixel.getTracks()) {
        pixelTracks.push_back(convertTrackToJson(track));
    }
    jsonPixel["tracks"] = pixelTracks;
    return jsonPixel;
}

std::string JsonConverter::convertFrameToJsonString(Frame frame) {
    Json jsonFrame;
    jsonFrame["frameID"] = frame.frameID;
    jsonFrame["systemID"] = frame.systemID;
    Json framePixels;
    for (Pixel pixel: *frame.getPixels()) {
        framePixels.push_back(convertPixelToJson(pixel));
    }
    jsonFrame["pixels"] = framePixels;
    return jsonFrame.dump();
}

#endif //SPIRITSENSORGATEWAY_JSONPARSER_CPP