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

#ifndef SENSORGATEWAY_SENSORPARAMETERCONTROLLERTEST_CPP
#define SENSORGATEWAY_SENSORPARAMETERCONTROLLERTEST_CPP

#include <gtest/gtest.h>

#include "test/utilities/mock/Function.hpp"
#include "test/utilities/mock/DevNullDataTranslationStrategyMock.hpp"
#include "test/utilities/assertion/ErrorAssertion.hpp"

#include "sensor-gateway/parameter-control/SensorParameterController.hpp"

namespace SensorParameterControllerTestMock {

    using RealisticDataStructures = Sensor::Test::RealisticImplementation::Structures;
    using DevNullTranslationStrategy = Mock::DevNullDataTranslationStrategyMock<RealisticDataStructures>;

    using DataTranslator = SensorAccessLinkElement::DataTranslator<RealisticDataStructures, RealisticDataStructures>;

    class RequestLoopBackDataTranslatorMock final : public DataTranslator {

    protected:
        using super = DataTranslator;

        using SensorControlMessage = typename super::SensorControlMessage;
//        using SensorResponseMessageSource

    public:

        explicit RequestLoopBackDataTranslatorMock() :
                DataTranslator(&devNullTranslationStrategy) {
        }


    private:

        DevNullTranslationStrategy devNullTranslationStrategy;
    };

    using SimpleServerCommunicationStrategy = ServerCommunication::ServerCommunicationStrategy<Sensor::Test::Simple::Structures>;

    class MockServerCommunicationStrategy final : public SimpleServerCommunicationStrategy {
    protected:
        using super = SimpleServerCommunicationStrategy;

        using Messages = std::list<super::Message>;
        using RawDataCycles = std::list<super::RawData>;

    public:
        MockServerCommunicationStrategy() :
                openConnectionCalled(false),
                closeConnectionCalled(false) {}

        ~MockServerCommunicationStrategy() noexcept override = default;

        void openConnection(std::string const& serverAddress) override {
            openConnectionCalled.store(true);
        }

        bool hasOpenConnectionBeenCalled() const {
            return openConnectionCalled.load();
        }

        typename super::GetParameterValueContents fetchGetParameterValueContents() override {
            typename super::GetParameterValueContents contents;
            return contents;
        }

        void closeConnection() override {
            closeConnectionCalled.store(true);
        }

        bool hasCloseConnectionBeenCalled() const {
            return closeConnectionCalled.load();
        }

        void sendMessage(super::Message&& message) override {
            receivedMessages.push_back(message);
        }

        void sendRawData(super::RawData&& rawData) override {
            receivedRawData.push_back(rawData);
        }

        Messages const& getReceivedMessages() const {
            return receivedMessages;
        }

        RawDataCycles const& getReceivedRawData() const {
            return receivedRawData;
        }

    private:
        Messages receivedMessages;
        RawDataCycles receivedRawData;
        AtomicFlag openConnectionCalled;
        AtomicFlag closeConnectionCalled;
    };
}

#endif //SENSORGATEWAY_SENSORPARAMETERCONTROLLERTEST_CPP


