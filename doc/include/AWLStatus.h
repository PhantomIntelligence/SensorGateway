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

#ifndef SPECTRE_AWLSTATUS_H
#define SPECTRE_AWLSTATUS_H

#include "AWLCommunicationStructs.h"

namespace SensorCommunication {
    namespace AWL {

        /**
         * @brief Contains the status information relative to the AWL Sensor
         */
        class SensorStatus {
        public:
            Boolean statusHasBeenUpdated = false;
            Data32 systemTemperature = 0.0;
            Data32 coreVoltage = 0.0;

            /**
             * @brief Current frame rate.
             * @note May not be reported accurately during playback.
             */
            uData8 frameRate{};

            /** \brief Hardware error flags.
              */
            union {
                uData8 byteData;
                struct {
                    Boolean emitter0 : 1;
                    Boolean emitter1 : 1;
                    Boolean receiver : 1;
                    Boolean dsp      : 1;
                    Boolean memory   : 1;
                } bitFieldData;
            } hardwareError{};

            ReceiverError receiverError{};
            OperationErrors status{};
            FirmwareVersion version{};
            BootChecksumError bootChecksumError{};
            BootSelfTest bootSelfTest{};


            /** \brief Set to true when the sensor is in record mode.
              */
            Boolean inRecordMode = false;

            /** \brief Set to true when the sensor is in playback mode.
              */
            Boolean inPlaybackMode = false;

            /** \brief Current playback file name.
              */
            std::string playbackFileName = "";

            /** \brief Current record file name.
              */
            std::string recordFileName = "";

            /** \brief Communications protocol command error.
              * 0 means no error. Otherwise, the Command type that generated the error code is indicated.
              * \remarks Details on the error can be found in the communications log file.
              */
            uData8 lastCommandError = 0;

            uData16 fpgaRegisterAddressRead = 0;
            uData32 fpgaRegisterValueRead = 0;
            uData16 adcRegisterAddressRead = 0;
            uData32 adcRegisterValueRead = 0;
            uData16 gpioRegisterAddressRead = 0;
            uData32 gpioRegisterValueRead = 0;

            uData16 currentAlgo{};
            uData16 currentAlgoPendingUpdates{};
            Data32 signalToNoiseFloor = 0;

            SensorCommunication::AWL::ChannelMask channelMask{};
            SensorCommunication::AWL::MessageMask messageMask{};
        } ReceiverStatus;

    }
}

#endif //SPECTRE_AWLSTATUS_H
