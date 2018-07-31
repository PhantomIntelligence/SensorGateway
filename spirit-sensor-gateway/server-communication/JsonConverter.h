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
#ifndef SPIRITSENSORGATEWAY_JSONPARSER_H
#define SPIRITSENSORGATEWAY_JSONPARSER_H

#include <nlohmann/json.hpp>
#include "spirit-sensor-gateway/domain/Frame.h"


namespace ServerCommunication {
    using Json = nlohmann::json;
    using DataFlow::Frame;

    class JsonConverter final{
    public:
        JsonConverter() = delete;

        ~JsonConverter() = delete;

        JsonConverter(JsonConverter const& other) = delete;

        JsonConverter(JsonConverter&& other) noexcept = delete;

        JsonConverter& operator=(JsonConverter const& other)& = delete;

        JsonConverter& operator=(JsonConverter&& other) noexcept = delete;

        static std::string convertFrameToJsonString(Frame frame);

    private:
        static Json convertPixelToJson(Pixel pixel);

        static Json convertTrackToJson(Track track);

    };
}

#endif //SPIRITSENSORGATEWAY_JSONPARSER_H
