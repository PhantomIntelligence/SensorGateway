/**
	Copyright 2014-2019 Phantom Intelligence Inc.

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

#ifndef SENSORGATEWAY_SIMPLE_CONTROLMESSAGE_H
#define SENSORGATEWAY_SIMPLE_CONTROLMESSAGE_H

#include "SimpleRawData.h"

namespace DataModel {

    class SimpleControlMessage {

    protected:


    public:

        static int const NUMBER_OF_SIMPLE_CONTROL_MESSAGE_CONTENT = 2;
        static int const NUMBER_OF_SIMPLE_CONTROL_MESSAGE_TIMESTAMPS = 2;

        // Allows to populate SimpleControlMessage with a "complex", non-primitive, element
        typedef std::array<std::string, NUMBER_OF_SIMPLE_CONTROL_MESSAGE_CONTENT> Content;
        typedef Metrics::ServiceTimestamps<NUMBER_OF_SIMPLE_CONTROL_MESSAGE_TIMESTAMPS> ServiceTimestamps;

        explicit SimpleControlMessage(Content simpleDataContent,
                               ServiceTimestamps const& gatewayTimestamps);

        explicit SimpleControlMessage();

        ~SimpleControlMessage() noexcept = default;

        SimpleControlMessage(SimpleControlMessage const& other) = default;

        SimpleControlMessage(SimpleControlMessage&& other) noexcept;

        SimpleControlMessage& operator=(SimpleControlMessage const& other)&;

        SimpleControlMessage& operator=(SimpleControlMessage&& other)& noexcept;

        void swap(SimpleControlMessage& current, SimpleControlMessage& other) noexcept;

        bool operator==(SimpleControlMessage const& other) const;

        bool operator!=(SimpleControlMessage const& other) const;

        void inverseContent();

        bool isTheInverseOf(SimpleControlMessage const& other) const;

        std::string toString() const noexcept;

        void addTimePointForGatewayWithLocation(std::string const& locationName);

        ServiceTimestamps const& getGatewayTimestamps() const noexcept;

        SimpleControlMessage static const returnDefaultData() noexcept;

    private:

        Content content;
        ServiceTimestamps gatewayTimestamps;
    };

    namespace Defaults {
        using DataModel::SimpleControlMessage;

        std::string const DEFAULT_SIMPLE_CONTROL_MESSAGE_NAME = "Execute Order";
        std::string const DEFAULT_SIMPLE_CONTROL_MESSAGE_VALUE = "66";
        SimpleControlMessage::Content const DEFAULT_SIMPLE_CONTROL_MESSAGE_CONTENT = SimpleControlMessage::Content(
                {DEFAULT_SIMPLE_CONTROL_MESSAGE_NAME,
                 DEFAULT_SIMPLE_CONTROL_MESSAGE_VALUE});
        SimpleControlMessage const DEFAULT_SIMPLE_CONTROL_MESSAGE = SimpleControlMessage(DEFAULT_SIMPLE_CONTROL_MESSAGE_CONTENT,
                                                                   DEFAULT_SERVICE_TIMESTAMPS);
    }

}

#endif //SENSORGATEWAY_SIMPLE_CONTROLMESSAGE_H
