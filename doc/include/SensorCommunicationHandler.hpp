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

#ifndef SPECTRE_SENSORCOMMUNICATIONHANDLER_HPP
#define SPECTRE_SENSORCOMMUNICATIONHANDLER_HPP


class SensorCommunicationHandler {

public:
    virtual SensorCommunicationHandler() = 0;

    virtual ~SensorCommunicationHandler() noexcept = default;

    /**
     * @brief The SensorCommunicationHandler are intended to be used as const instances and referenced ONLY. They shouldn't be move-copied.
     */
    virtual SensorCommunicationHandler(SensorCommunicationHandler&& other) = delete;

    /**
     * @brief The SensorCommunicationHandler are intended to be used as const instances and referenced ONLY. They shouldn't be copied.
     */
    virtual SensorCommunicationHandler(SensorCommunicationHandler const& other) = delete;

    /**
     * @brief The SensorCommunicationHandler are intended to be used as const instances and referenced ONLY. They shouldn't be assigned.
     */
    virtual SensorCommunicationHandler& operator=(SensorCommunicationHandler&& other) = delete;

    /**
     * @brief The SensorCommunicationHandler are intended to be used as const instances and referenced ONLY. They shouldn't be assigned.
     */
    virtual SensorCommunicationHandler& operator=(SensorCommunicationHandler const& other) = delete;

};


#endif //SPECTRE_SENSORCOMMUNICATIONHANDLER_H
