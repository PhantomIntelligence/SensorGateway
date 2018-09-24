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

#ifndef SENSORGATEWAY_SENSORACCESSLINKERROR_H
#define SENSORGATEWAY_SENSORACCESSLINKERROR_H

#include "ErrorConstants.h"

namespace ErrorHandling {

    class SensorAccessLinkError {
    protected:

    public:

        explicit SensorAccessLinkError();

        ~SensorAccessLinkError() noexcept;

        SensorAccessLinkError(SensorAccessLinkError const& other);

        SensorAccessLinkError(SensorAccessLinkError&& other) noexcept;

        SensorAccessLinkError(std::string const& origin);

        SensorAccessLinkError& operator=(SensorAccessLinkError const& other)&;

        SensorAccessLinkError& operator=(SensorAccessLinkError&& other)& noexcept;

        void swap(SensorAccessLinkError& current, SensorAccessLinkError& other) noexcept;

    private:

        std::string const origin;
    };

}

#endif //SENSORGATEWAY_SENSORACCESSLINKERROR_H
