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

#ifndef SENSORGATEWAY_SENSORCOMMUNICATIONSTRATEGY_H
#define SENSORGATEWAY_SENSORCOMMUNICATIONSTRATEGY_H

#include "sensor-gateway/common/ConstantFunctionsDefinition.h"

namespace SensorCommunication {

    // TODO: add std::enable_if condition to force SFINAE with structures
    template<class T>
    class SensorCommunicationStrategy {

    protected:
        typedef T DATA;

    public:
        typedef typename DATA::Message Message;
        typedef typename DATA::RawData RawData;

        using Messages = std::array<Message, DATA::MAX_NUMBER_OF_BULK_FETCHABLE_MESSAGES>;
        using RawDataCycles = std::array<RawData, DATA::MAX_NUMBER_OF_BULK_FETCHABLE_RAW_DATA_CYCLES>;

        virtual ~SensorCommunicationStrategy() noexcept = default;

        virtual void openConnection() = 0;

        virtual Messages fetchMessages() = 0;

        virtual RawDataCycles fetchRawDataCycles() = 0;

        virtual void closeConnection() = 0;

    };
}

#endif //SENSORGATEWAY_SENSORCOMMUNICATIONSTRATEGY_H
