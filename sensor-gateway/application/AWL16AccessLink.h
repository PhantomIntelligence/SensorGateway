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

#ifndef SENSORGATEWAY_AWL16ACCESSLINK_H
#define SENSORGATEWAY_AWL16ACCESSLINK_H

#include "SensorAccessLink.hpp"
#include "sensor-gateway/sensor-communication/KvaserCanCommunicationStrategy.h"
#include "sensor-gateway/message-translation/AWLTranslationStrategy.h"

namespace SensorGateway {

    class AWL16AccessLink final : public SensorAccessLink<Sensor::AWL::Structures, DataFlow::Frame> {
    protected:
        using super = SensorAccessLink<Sensor::AWL::Structures, DataFlow::Frame>;

        using super::ServerCommunicationStrategy;

    public:

        explicit AWL16AccessLink(ServerCommunicationStrategy* serverCommunicationStrategy)
                : awlTranslationStrategy(),
                  kvaserCanCommunicationStrategy(),
                  super(serverCommunicationStrategy,
                        &awlTranslationStrategy,
                        &kvaserCanCommunicationStrategy) {}

        ~AWL16AccessLink() noexcept = default;

        AWL16AccessLink(AWL16AccessLink const& other) = delete;

        AWL16AccessLink(AWL16AccessLink&& other) noexcept = delete;

        AWL16AccessLink& operator=(AWL16AccessLink const& other)& = delete;

        AWL16AccessLink& operator=(AWL16AccessLink&& other)& noexcept = delete;

    private:

        MessageTranslation::AWLTranslationStrategy awlTranslationStrategy;
        SensorCommunication::KvaserCanCommunicationStrategy kvaserCanCommunicationStrategy;
    };
}

#endif //SENSORGATEWAY_AWL16ACCESSLINK_H
