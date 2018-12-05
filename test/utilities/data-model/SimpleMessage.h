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

#ifndef SENSORGATEWAY_SIMPLEMESSAGE_H
#define SENSORGATEWAY_SIMPLEMESSAGE_H

#include "sensor-gateway/common/data-structure/TimeTracking.hpp"

namespace DataModel {

    class SimpleMessage {

    protected:


    public:

        static int const NUMBER_OF_SIMPLE_MESSAGE_CONTENT = 2;
        static int const NUMBER_OF_SIMPLE_MESSAGE_TIMESTAMPS = 2;

        // Allows to populate SimpleMessage with a "complex", non-primitive, element
        typedef std::array<std::string, NUMBER_OF_SIMPLE_MESSAGE_CONTENT> Content;
        typedef Metrics::ServiceTimestamps<NUMBER_OF_SIMPLE_MESSAGE_TIMESTAMPS> ServiceTimestamps;

        explicit SimpleMessage(Content simpleDataContent,
                               ServiceTimestamps const& gatewayTimestamps);

        explicit SimpleMessage();

        ~SimpleMessage() noexcept = default;

        SimpleMessage(SimpleMessage const& other) = default;

        SimpleMessage(SimpleMessage&& other) noexcept;

        SimpleMessage& operator=(SimpleMessage const& other)&;

        SimpleMessage& operator=(SimpleMessage&& other)& noexcept;

        void swap(SimpleMessage& current, SimpleMessage& other) noexcept;

        bool operator==(SimpleMessage const& other) const;

        bool operator!=(SimpleMessage const& other) const;

        void inverseContent();

        bool isTheInverseOf(SimpleMessage const& other) const;

        std::string toString() const noexcept;

        void addTimePointForGatewayWithLocation(std::string const& locationName);

        ServiceTimestamps const& getGatewayTimestamps() const noexcept;

        SimpleMessage static const returnDefaultData() noexcept;

    private:

        Content content;
        ServiceTimestamps gatewayTimestamps;
    };

    namespace Defaults {
        using DataModel::SimpleMessage;

        std::string const DEFAULT_FIRST_SIMPLE_MESSAGE_CONTENT = "Some first simple data content";
        std::string const DEFAULT_SECOND_SIMPLE_MESSAGE_CONTENT = "Some second simple data content.";
        SimpleMessage::Content const DEFAULT_SIMPLE_MESSAGE_CONTENT = SimpleMessage::Content(
                {DEFAULT_FIRST_SIMPLE_MESSAGE_CONTENT,
                 DEFAULT_SECOND_SIMPLE_MESSAGE_CONTENT});
        SimpleMessage::ServiceTimestamps const DEFAULT_SERVICE_TIMESTAMPS;
        SimpleMessage const DEFAULT_SIMPLE_MESSAGE = SimpleMessage(DEFAULT_SIMPLE_MESSAGE_CONTENT,
                                                                   DEFAULT_SERVICE_TIMESTAMPS);
    }
}

#endif //SENSORGATEWAY_SIMPLEMESSAGE_H
