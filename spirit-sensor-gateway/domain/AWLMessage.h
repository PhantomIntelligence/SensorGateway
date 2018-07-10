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

#ifndef SPIRITSENSORGATEWAY_MESSAGE_H
#define SPIRITSENSORGATEWAY_MESSAGE_H

#include "spirit-sensor-gateway/common/ConstantValuesDefinition.h"

    namespace AWL {
        using AwlMessageID = int64_t;
        using AwlMessageTimestamp = int64_t;
        using AwlMessageLength = int32_t;
        using DataArray = std::array<unsigned char, MAX_NUMBER_OF_DATA_IN_AWL_MESSAGE> ;
        }

namespace DataFlow {
    class AWLMessage {
    public:
        explicit AWLMessage(AWL::AwlMessageID id, AWL::AwlMessageTimestamp timestamp, AWL::AwlMessageLength length, AWL::DataArray data);

        ~AWLMessage() noexcept = default;

        AWLMessage(AWLMessage const& other);

        AWLMessage(AWLMessage&& other) noexcept;

        AWLMessage& operator=(AWLMessage const& other)& ;

        AWLMessage& operator=(AWLMessage&& other)& noexcept;

        void swap(AWLMessage& current, AWLMessage& other) noexcept;

        bool operator==(AWLMessage const&) const;

        bool operator!=(AWLMessage const&) const;

        AWLMessage static const returnDefaultData() noexcept;

        AWL::AwlMessageID id;
        AWL::AwlMessageTimestamp timestamp;
        AWL::AwlMessageLength length;
        AWL::DataArray data;
    };

}

namespace Defaults {
    using DataFlow::AWLMessage;
    AWL::AwlMessageID const DEFAULT_ID = 0;
    AWL::AwlMessageTimestamp const DEFAULT_TIMESTAMP = 0;
    AWL::AwlMessageLength const DEFAULT_LENGTH = 0;
    AWL::DataArray const DEFAULT_AWL_DATA = AWL::DataArray();
    AWLMessage const DEFAULT_AWL_MESSAGE = AWLMessage(DEFAULT_ID, DEFAULT_TIMESTAMP, DEFAULT_LENGTH, DEFAULT_AWL_DATA);
}

#endif //SPIRITSENSORGATEWAY_MESSAGE_H
