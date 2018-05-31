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

#ifndef SPECTRE_SENSORCONNECTION_HPP
#define SPECTRE_SENSORCONNECTION_HPP

#include "AWLCommunicationStructs.h"

namespace SensorCommunication {
/**
 * @brief Interface that manages the connection to a sensor.
 * @tparam MESSAGE The base-level structure that is used to transmit and received messages (It should be sensor-specific).
 */
    template<class MESSAGE>
    class SensorConnection {
    public:
        SensorConnection() {}

        virtual ~SensorConnection() noexcept {}

//        SensorConnection(SensorConnection const& other) = delete;
//
//        SensorConnection(SensorConnection&& other) noexcept = delete;
//
//        SensorConnection& operator=(SensorConnection other)& = delete;
//
//        SensorConnection& operator=(SensorConnection&& other)& noexcept = delete;

        virtual void openConnection() = 0;

        virtual void closeConnection() = 0;

        virtual void transmitMessage(MESSAGE const& message) = 0;

        virtual auto receiveMessage() -> MESSAGE = 0;

    };
}

#endif //SPECTRE_SENSORCONNECTION_HPP
