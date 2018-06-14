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
    uint64_t messageFlags;
    uint64_t messageLength;
    unsigned char messageData[MESSAGE_DATA_LENGTH];
};

struct AWLFrameDoneMessage:AWLMessage {
    const int FRAME_ID_DATA_INDEX = 00;
    const int SYSTEM_ID_DATA_INDEX = 02;
    const int SYSTEM_ERROR_FLAG_DATA_INDEX= 04;
    uint16_t frameID;
    uint16_t systemID;
    uint32_t systemErrorFlag;

};
struct AWLDetectionTrackMessage:AWLMessage  {
    const int TRACKING_ID_DATA_INDEX = 00;
    uint16_t trackingId;
    uint8_t obstacleSensor;
    uint16_t obstaclePixel;
    uint8_t trackProbability;
    uint16_t objectIntensity;
};
struct AWLDetectionVelocityMessage:AWLMessage  {
    const int TRACKING_ID_DATA_INDEX = 00;
    const int TRACK_DISTANCE_DATA_INDEX = 02;
    const int TRACK_SPEED_DATA_INDEX = 04;
    const int TRACK_ACCELERATION_DATA_INDEX = 06;
    uint16_t trackingId;
    uint16_t trackDistance;
    int16_t trackSpeed;
    int16_t trackAcceleration;


};

#endif //SPIRITSENSORGATEWAY_MESSAGE_H
