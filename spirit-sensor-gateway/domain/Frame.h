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

using DataFlow::Pixel;
using Sensor::AWL::_16::NUMBER_OF_PIXELS_IN_FRAME;

namespace DataFlow {
    using PixelsArray =  std::array<Pixel, NUMBER_OF_PIXELS_IN_FRAME>;
}

namespace DataFlow {

    class Frame {

    public:
        Frame(FrameID frameID, SystemID systemID, PixelsArray pixels);

        Frame();

        ~Frame() = default;

        Frame(Frame const& other);

        Frame(Frame&& other) noexcept;

        Frame& operator = (Frame const& other)& ;

        Frame& operator = (Frame&& other)& noexcept;

        void swap(Frame& current, Frame& other) noexcept;

        bool operator == (Frame const& other) const;

        bool operator != (Frame const& other) const;

        void addPixel(Pixel pixel);

        Pixel* fetchPixelByID(PixelID pixelID);

        FrameID getFrameID() const;

        SystemID getSystemID() const;

        PixelsArray* getPixels();

        void setFrameID(FrameID const& frameID);

        void setSystemID(SystemID const& systemID);

        static Frame const& returnDefaultData() noexcept;


    private:
        FrameID frameID;
        SystemID systemID;
        PixelsArray pixels;
    };
}

namespace Defaults {
    namespace Frame {
        using DataFlow::Frame;
        using DataFlow::FrameID;
        using DataFlow::SystemID;
        using DataFlow::PixelsArray;
        FrameID const DEFAULT_FRAME_ID = 0;
        SystemID const DEFAULT_SYSTEM_ID = 0;
        PixelsArray const DEFAULT_PIXELS_ARRAY = PixelsArray();
        Frame const DEFAULT_FRAME = Frame();
    }
}

#endif //SPIRITSENSORGATEWAY_FRAME_H
