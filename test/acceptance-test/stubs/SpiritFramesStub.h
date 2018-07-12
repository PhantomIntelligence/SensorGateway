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

using DataFlow::Frame;
using DataFlow::Pixel;
using DataFlow::Track;

std::vector<Frame> spiritFramesStub = {
        Frame(64829, 16, {
                Pixel(11, {Track(14291, 0, 123, 0, 106, 0)}, 1),
                Pixel(12, {Track(14324, 0, 93, 0, 109, 0)}, 1),
                Pixel(13, {Track(14302, 0, 115, 0, 109, -1)}, 1),
                Pixel(14, {Track(14322, 0, 104, 0, 115, -6)}, 1),
                Pixel(15, {Track(14321, 0, 110, 0, 117, 0)}, 1),
        }),
        Frame(64830, 16, {
                Pixel(0, {Track(14275, 0, 139, 0, 109, 0)}, 1),
                Pixel(1, {Track(2, 0, 135, 0, 110, 0)}, 1),
                Pixel(2, {Track(14286, 0, 125, 0, 105, 0)}, 1),
                Pixel(3, {Track(14270, 0, 127, 0, 106, 1)}, 1),
                Pixel(4, {Track(14301, 0, 122, 0, 104, 0)}, 1),
                Pixel(5, {Track(14272, 0, 120, 0, 108, 0)}, 1),
                Pixel(6, {Track(14316, 0, 114, 0, 114, 0)}, 1),
                Pixel(7, {Track(14320, 0, 110, 0, 117, 1)}, 1),
                Pixel(8, {Track(14284, 0, 126, 0, 111, 0)}, 1),
                Pixel(9, {Track(14289, 0, 124, 0, 111, 0)}, 1),
                Pixel(10, {Track(14290, 0, 125, 0, 107, 0)}, 1),
                Pixel(11, {Track(14291, 0, 123, 0, 106, 0)}, 1),
                Pixel(12, {Track(14324, 0, 91, 0, 109, 1)}, 1),
                Pixel(13, {Track(14302, 0, 114, 0, 109, 0)}, 1),
                Pixel(14, {Track(14322, 0, 109, 0, 116, -2)}, 1),
                Pixel(15, {Track(14321, 0, 109, 0, 117, 0)}, 1),
        }),
};