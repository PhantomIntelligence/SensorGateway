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

#ifndef SENSORGATEWAY_ABSTRACTSTRUCTURES_H
#define SENSORGATEWAY_ABSTRACTSTRUCTURES_H

namespace Sensor {
    namespace Communication {
        class AbstractStructures {
        public:

            explicit AbstractStructures() = delete;

            ~AbstractStructures() noexcept = delete;

            AbstractStructures(AbstractStructures const& other) = delete;

            AbstractStructures(AbstractStructures&& other) noexcept = delete;

            AbstractStructures& operator=(AbstractStructures const& other)& = delete;

            AbstractStructures& operator=(AbstractStructures&& other)& noexcept = delete;

            void swap(AbstractStructures& current, AbstractStructures& other) noexcept = delete;
        };
    }
}

#endif //SENSORGATEWAY_ABSTRACTSTRUCTURES_H
