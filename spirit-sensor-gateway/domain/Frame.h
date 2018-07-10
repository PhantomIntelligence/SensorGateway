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

#ifndef SPIRITSENSORGATEWAY_FRAME_H
#define SPIRITSENSORGATEWAY_FRAME_H

#include "Pixel.h"


namespace DataFlow {

    class Frame {

    public:
            Frame();
            ~Frame();
            void addPixel(Pixel pixel);
            Pixel* fetchPixelByID(PixelID pixelID);
            FrameID getFrameID() const;
            std::array<Pixel, NUMBER_OF_PIXELS_IN_FRAME>* getPixels() ;
            SystemID getSystemID() const;
            void setFrameID(FrameID const& frameID);
            void setSystemID(SystemID const& systemID);
            Frame static const returnDefaultData() noexcept ;
            bool operator==(Frame const& other) const;

        private:
            FrameID frameID;
            std::array<Pixel, NUMBER_OF_PIXELS_IN_FRAME> pixels;
            SystemID systemID;
    };

    namespace Defaults {
        namespace Frame {
            using DataFlow::Pixel;
            FrameID const DEFAULT_FRAME_ID_VALUE = 0;
            SystemID const DEFAULT_SYSTEM_ID_VALUE = 0;
            std::array<Pixel, NUMBER_OF_PIXELS_IN_FRAME> const DEFAULT_PIXELS_ARRAY = std::array<Pixel, NUMBER_OF_PIXELS_IN_FRAME>();
            DataFlow::Frame const DEFAULT_FRAME = DataFlow::Frame();
        }
    }


}

#endif //SPIRITSENSORGATEWAY_FRAME_H
