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

#include <spirit-sensor-gateway/common/ConstantDefinition.h>

struct AWLMessage {
    uint64_t messageID;
    uint64_t messageTimestamp;
    uint64_t messageLength;
    unsigned char messageData[MESSAGE_DATA_LENGTH];
};

#endif //SPIRITSENSORGATEWAY_MESSAGE_H
