/**
	Copyright 2014-2016 Phantom Intelligence Inc.

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


#ifndef SPECTRE_AWLCOMMUNICATIONSTRUCTS_H
#define SPECTRE_AWLCOMMUNICATIONSTRUCTS_H

//#include "CoordinateSystem.h"
#include "../TypeDefinition.h"

namespace SensorCommunication {
    namespace AWL {
        namespace InnerStruct {
            /**
             * @brief Describes the AWL message groups of which the reception can be toggled on/off.
             * @note This is mainly used for operation customisation or to bandwidth and communications preservation
             */
            struct MessageGroups {
                Boolean unused_0      : true;
                Boolean obstacle      : true;
                Boolean distance_1_4  : true;
                Boolean distance_5_8  : true;
                Boolean intensity_1_4 : true;
                Boolean intensity_5_8 : true;
                Boolean unused_6      : true;
                Boolean unused_7      : true;
            };

            /**
             * @brief Describes the AWL receiver channel
             */
            struct Channels {
                Boolean channel0 : true;
                Boolean channel1 : true;
                Boolean channel2 : true;
                Boolean channel3 : true;
                Boolean channel4 : true;
                Boolean channel5 : true;
                Boolean channel6 : true;
                Boolean monitor  : true;
            };

            struct OperationErrorOrigin {
                Boolean selfTest           : true;
                Boolean shutdown           : true;
                Boolean sensorBlocked      : true;
                Boolean reducedPerformance : true;
                Boolean saturation         : true;
            };

            struct ChecksumState {
                Boolean mainChecksum : true;
                Boolean auxChecksum  : true;
            };

            struct SelfTestState {
                Boolean emitter0    : true;
                Boolean emitter1    : true;
                Boolean receiver    : true;
                Boolean dsp         : true;
                Boolean memory      : true;
                Boolean checksum    : true;
            };

        }

        struct MessageMask {
            uData8 byteData;
            InnerStruct::MessageGroups bitFieldData;
        };

        struct ChannelMask {
            uData8 byteData;
            /** @note the monitor value is unused here */
            InnerStruct::Channels bitFieldData;
        };

        /**
         * @brief Indicates an error condition on a specific detector channel.
         */
        struct ReceiverError {
            uData8 byteData;
            InnerStruct::Channels bitFieldData;
        };

        struct OperationErrors {
            uData8 byteData;
            InnerStruct::OperationErrorOrigin bitFieldData;
        };

        struct FirmwareVersion {
            uData8 major;
            uData8 minor;
        };

        struct BootChecksumError {
            uData8 byteData;
            InnerStruct::ChecksumState bitFieldData;
        };

        /** \brief Boot sequence hardware self-tests.
          */
        struct BootSelfTest {
            uData8 byteData;
            InnerStruct::SelfTestState bitFieldData;
        };

        namespace CAN {

// CAN Frame
//            typedef struct {
//                unsigned long id;        // Message id
//                unsigned long timestamp; // timestamp in milliseconds
//                unsigned char flags;     // [extended_id|1][RTR:1][reserver:6]
//                unsigned char len;       // Frame size (0.8)
//                unsigned char data[8]; // Databytes 0..7
//            } AWLCANMessage;

            const auto NUMBER_OF_DATA_IN_MESSAGE = 0;

            struct Message {
                uData64 id;
                uData64 timestamp; // in milliseconds
                uData8 flags;
                uData8 length;
                unsignedChar data[NUMBER_OF_DATA_IN_MESSAGE];
            };
        }
    }

}

#endif //SPECTRE_AWLCOMMUNICATIONSTRUCTS_H
