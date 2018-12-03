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

#ifndef SENSORGATEWAY_COMMAND_HPP
#define SENSORGATEWAY_COMMAND_HPP

#include "TimeTracking.hpp"

namespace Sensor {
    /**
     * @brief This CommandDefinition struct serves to declare types used by <SensorName>Command classes
     * @tparam T the type of an individual payload data
     * @tparam N the max number of payload data in a command
     */
    template<typename T, std::size_t N>
    struct CommandDefinition {
        static std::size_t const SIZE = N;
        using ValueType = T;
        using Payload = std::array<ValueType, SIZE>;
    };

    namespace CommandPayloadTypes {
        typedef Byte AWL;
        typedef Byte GUARDIAN;
    }
}

namespace DataFlow {

    template<typename CommandDefinition>
    class Command {

    protected:

    public:

        using Definitions = CommandDefinition;
        using Payload = typename Definitions::Payload;

        enum CommandCode {
            NOOP = 0
        };

        explicit Command(CommandCode commandCode, Payload payload) noexcept :
                commandCode(commandCode),
                payload(payload) {}

        explicit Command() noexcept :
                Command(Command::returnDefaultData()) {}

        ~Command() noexcept = default;

        Command(Command const& other) :
                Command(other.commandCode, other.payload) {}

        Command(Command&& other) noexcept :
                Command(std::move(other.commandCode),
                        std::move(other.payload)) {}

        Command& operator=(Command const& other)& {
            Command temporary(other);
            swap(*this, temporary);
            return *this;
        }

        Command& operator=(Command&& other)& noexcept {
            swap(*this, other);
            return *this;
        }

        void swap(Command& current, Command& other) noexcept {
            std::swap(current.commandCode, other.commandCode);
            std::swap(current.payload, other.payload);
        }

        bool operator==(Command const& other) const {
            auto sameCommandCode = (commandCode == other.commandCode);
            auto samePayload = (payload == other.payload);
            auto messagesAreEqual = (sameCommandCode && samePayload);
            return messagesAreEqual;
        }

        bool operator!=(Command const& other) const {
            return !operator==(other);
        }

        static Command const& returnDefaultData() noexcept;

        CommandCode const commandCode;
        Payload const payload;
    };

    namespace Defaults {
        using DataFlow::Command;

        template<typename CommandDefinition>
        typename Command<CommandDefinition>::CommandCode const DEFAULT_COMMAND_CODE = Command<CommandDefinition>::CommandCode::NOOP;

        template<typename CommandDefinition>
        typename CommandDefinition::Payload const DEFAULT_COMMAND_PAYLOAD{};

        template<typename CommandDefinition>
        Command<CommandDefinition> const DEFAULT_COMMAND = Command<CommandDefinition>(
                DEFAULT_COMMAND_CODE<CommandDefinition>,
                DEFAULT_COMMAND_PAYLOAD<CommandDefinition>);
    }

    template<typename CommandDefinition>
    Command<CommandDefinition> const& Command<CommandDefinition>::returnDefaultData() noexcept {
        return Defaults::DEFAULT_COMMAND<CommandDefinition>;
    }

}

#endif //SENSORGATEWAY_COMMAND_HPP
