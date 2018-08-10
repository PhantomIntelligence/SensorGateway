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

#ifndef SENSORGATEWAY_GUARDIANMESSAGE_H
#define SENSORGATEWAY_GUARDIANMESSAGE_H

#include "sensor-gateway/common/ConstantValuesDefinition.h"

namespace DataFlow {

    class GuardianMessage {
    protected:

    public:

        explicit GuardianMessage();

        ~GuardianMessage() noexcept;

        GuardianMessage(GuardianMessage const& other);

        GuardianMessage(GuardianMessage&& other) noexcept;

        GuardianMessage& operator=(GuardianMessage const& other)&;

        GuardianMessage& operator=(GuardianMessage&& other)& noexcept;

        void swap(GuardianMessage& current, GuardianMessage& other) noexcept;

        bool operator==(GuardianMessage const&) const;

        bool operator!=(GuardianMessage const&) const;

        static GuardianMessage const& returnDefaultData() noexcept;
    };

    namespace Defaults {
        using DataFlow::AWLMessage;
    }

}

#endif //SENSORGATEWAY_GUARDIANMESSAGE_H
