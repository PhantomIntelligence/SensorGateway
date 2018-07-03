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

#include "Frame.h"

namespace SpiritProtocol {
    Frame::Frame() {
    };

    Frame::~Frame() {
    };

    void Frame::addPixel(Pixel pixel) {
        pixels[pixel.getID()] = pixel;
    }

    Pixel* Frame::fetchPixelByID(PixelID pixelID) {
        return &pixels[pixelID];
    }

    FrameID Frame::getFrameID() const {
        return frameID;
    };

    std::array<Pixel,NUMBER_OF_PIXEL_IN_AWL_16_FRAME>* Frame::getPixels() {
        return &pixels;
    };

    SystemID Frame::getSystemID() const {
        return systemID;
    };

    void Frame::setFrameID(FrameID frameID) {
        this->frameID = frameID;
    }

    void Frame::setSystemID(SystemID systemID) {
        this->systemID = systemID;
    }
}
