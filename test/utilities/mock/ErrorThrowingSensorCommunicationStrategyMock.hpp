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

#ifndef SENSORGATEWAY_ERRORTHROWINGSENSORCOMMUNICATIONSTRATEGYMOCK_HPP
#define SENSORGATEWAY_ERRORTHROWINGSENSORCOMMUNICATIONSTRATEGYMOCK_HPP

#include "test/utilities/data-model/DataModelFixture.h"
#include "sensor-gateway/sensor-communication/SensorCommunicationStrategy.hpp"

namespace Mock {

    template<typename T>
    class ErrorThrowingSensorCommunicationStrategyMock :
            public SensorCommunication::SensorCommunicationStrategy<T> {

    protected:

        using super = SensorCommunication::SensorCommunicationStrategy<T>;

    public:

        ErrorThrowingSensorCommunicationStrategyMock() :
                super(),
                errorToThrow(ErrorHandling::SensorAccessLinkError::returnDefaultData()),
                errorThrown(false),
                throwOnOpen(false),
                throwOnClose(false),
                throwOnFetchMessages(false),
                throwOnFetchRawDataCycles(false) {
        }

        ~ErrorThrowingSensorCommunicationStrategyMock() noexcept final = default;

        ErrorThrowingSensorCommunicationStrategyMock(ErrorThrowingSensorCommunicationStrategyMock const& other) = delete;

        ErrorThrowingSensorCommunicationStrategyMock(ErrorThrowingSensorCommunicationStrategyMock&& other) noexcept = delete;

        ErrorThrowingSensorCommunicationStrategyMock& operator=(ErrorThrowingSensorCommunicationStrategyMock const& other)& = delete;

        ErrorThrowingSensorCommunicationStrategyMock&
        operator=(ErrorThrowingSensorCommunicationStrategyMock&& other)& noexcept = delete;

        ErrorHandling::SensorAccessLinkError expectedErrorRequiringOpenConnection() noexcept {
            return ErrorHandling::SensorAccessLinkError(ORIGIN,
                                                        ErrorHandling::Category::CONNECTION_ERROR,
                                                        ErrorHandling::Severity::WARNING,
                                                        ERROR_CODE,
                                                        MESSAGE);
        }

        ErrorHandling::SensorAccessLinkError expectedErrorRequiringCloseConnection() noexcept {
            return ErrorHandling::SensorAccessLinkError(ORIGIN,
                                                        ErrorHandling::Category::CONNECTION_ERROR,
                                                        ErrorHandling::Severity::EMERGENCY, // Will not require openConnection
                                                        ERROR_CODE,
                                                        MESSAGE);
        }

        void throwOpenConnectionRequiredErrorWhenOpenConnectionIsCalled() noexcept {
            throwOnOpen.store(true);
            errorToThrow = expectedErrorRequiringOpenConnection();

        }

        void throwOpenConnectionRequiredErrorWhenCloseConnectionIsCalled() noexcept {
            throwOnClose.store(true);
            errorToThrow = expectedErrorRequiringOpenConnection();

        }

        void throwOpenConnectionRequiredErrorWhenFetchMessagesIsCalled() noexcept {
            throwOnFetchMessages.store(true);
            errorToThrow = expectedErrorRequiringOpenConnection();

        }

        void throwOpenConnectionRequiredErrorWhenFetchRawDataCyclesIsCalled() noexcept {
            throwOnFetchRawDataCycles.store(true);
            errorToThrow = expectedErrorRequiringOpenConnection();

        }

        void throwCloseConnectionRequiredErrorWhenOpenConnectionIsCalled() noexcept {
            throwOnOpen.store(true);
            errorToThrow = expectedErrorRequiringCloseConnection();
        }

        void throwCloseConnectionRequiredErrorWhenCloseConnectionIsCalled() noexcept {
            throwOnClose.store(true);
            errorToThrow = expectedErrorRequiringCloseConnection();
        }

        void throwCloseConnectionRequiredErrorWhenFetchMessagesIsCalled() noexcept {
            throwOnFetchMessages.store(true);
            errorToThrow = expectedErrorRequiringCloseConnection();
        }

        void throwCloseConnectionRequiredErrorWhenFetchRawDataCyclesIsCalled() noexcept {
            throwOnFetchRawDataCycles.store(true);
            errorToThrow = expectedErrorRequiringCloseConnection();
        }

        ErrorHandling::SensorAccessLinkError expectedErrorWhenOpenConnectionIsCalled() noexcept {
            return ErrorHandling::SensorAccessLinkError(ORIGIN,
                                                        ErrorHandling::Category::COMMUNICATION_ERROR,
                                                        ErrorHandling::Severity::ERROR,
                                                        ERROR_CODE,
                                                        OPEN_ERROR_MESSAGE);
        }

        void throwErrorWhenOpenConnectionIsCalled() noexcept {
            throwOnOpen.store(true);
            errorToThrow = expectedErrorWhenOpenConnectionIsCalled();
        }

        ErrorHandling::SensorAccessLinkError expectedErrorWhenCloseConnectionIsCalled() noexcept {
            return ErrorHandling::SensorAccessLinkError(ORIGIN,
                                                        ErrorHandling::Category::COMMUNICATION_ERROR,
                                                        ErrorHandling::Severity::ERROR,
                                                        ERROR_CODE,
                                                        CLOSE_ERROR_MESSAGE);
        }

        void throwErrorWhenCloseConnectionIsCalled() noexcept {
            throwOnClose.store(true);
            errorToThrow = expectedErrorWhenCloseConnectionIsCalled();
        }


        ErrorHandling::SensorAccessLinkError expectedErrorWhenFetchMessagesIsCalled() noexcept {
            return ErrorHandling::SensorAccessLinkError(ORIGIN,
                                                        ErrorHandling::Category::COMMUNICATION_ERROR,
                                                        ErrorHandling::Severity::ERROR,
                                                        ERROR_CODE,
                                                        MESSAGE_ERROR_MESSAGE);
        }

        void throwErrorWhenFetchMessagesIsCalled() noexcept {
            throwOnFetchMessages.store(true);
            errorToThrow = expectedErrorWhenFetchMessagesIsCalled();
        }

        ErrorHandling::SensorAccessLinkError expectedErrorWhenFetchRawDataCyclesIsCalled() noexcept {
            return ErrorHandling::SensorAccessLinkError(ORIGIN,
                                                        ErrorHandling::Category::COMMUNICATION_ERROR,
                                                        ErrorHandling::Severity::ERROR,
                                                        ERROR_CODE,
                                                        RAW_DATA_ERROR_MESSAGE);
        }

        void throwErrorWhenFetchRawDataCyclesIsCalled() noexcept {
            throwOnFetchRawDataCycles.store(true);
            errorToThrow = expectedErrorWhenFetchRawDataCyclesIsCalled();
        }

        void openConnection() override {
            if (throwOnOpen.load()) {
                throwOnOpen.store(false); // We only throw once to allow the test to pass
                errorThrown.store(true);
                throw errorToThrow;
            }
            openConnectionCalled.store(true);
        }

        void closeConnection() override {
            if (throwOnClose.load()) {
                throwOnClose.store(false); // We only throw once to allow the test to pass
                errorThrown.store(true);
                throw errorToThrow;
            }
            closeConnectionCalled.store(true);
        }

        typename super::Messages fetchMessages() override {
            if(hasCloseConnectionBeenCalled()) {
                // WARNING! This mock implementation of readMessage needs to be slowed down because of the way gtest works. DO NOT REMOVE.
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                std::this_thread::yield();
            }
            if (!errorThrown.load()) {
                if (throwOnFetchMessages.load()) {
                    throwOnFetchMessages.store(false); // We only throw once to allow the test to pass
                    openConnectionCalled.store(false);
                    closeConnectionCalled.store(false);
                    errorThrown.store(true);
                    throw errorToThrow;
                }
            }

            auto message = super::Message::returnDefaultData();
            typename super::Messages messages = {message};
            return messages;
        }

        typename super::RawDataCycles fetchRawDataCycles() override {
            if(hasCloseConnectionBeenCalled()) {
                // WARNING! This mock implementation of readMessage needs to be slowed down because of the way gtest works. DO NOT REMOVE.
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                std::this_thread::yield();
            }
            if (!errorThrown.load()) {
                if (throwOnFetchRawDataCycles.load()) {
                    throwOnFetchRawDataCycles.store(false); // We only throw once to allow the test to pass
                    openConnectionCalled.store(false);
                    closeConnectionCalled.store(false);
                    errorThrown.store(true);
                    throw errorToThrow;
                }
            }
            auto rawData = super::RawData::returnDefaultData();
            typename super::RawDataCycles rawDataCycles = {rawData};
            return rawDataCycles;
        }

        bool hasCloseConnectionBeenCalledAfterThrowingFunction() const noexcept {
            return hasCloseConnectionBeenCalled() && hasErrorBeenThrown();
        }

        bool hasOpenConnectionBeenCalledAfterThrowingFunction() const noexcept {
            return hasOpenConnectionBeenCalled() && hasErrorBeenThrown();
        }

        bool hasOpenConnectionBeenCalled() const {
            return openConnectionCalled.load();
        }

        bool hasCloseConnectionBeenCalled() const {
            return closeConnectionCalled.load();
        }

        void waitUntilOpenConnectionCallIsMadeAfterErrorIsThrown() {
            while (!hasOpenConnectionBeenCalledAfterThrowingFunction()) {
                std::this_thread::yield();
            }
        }

        void waitUntilCloseConnectionCallIsMadeAfterErrorIsThrown() {
            while (!hasCloseConnectionBeenCalledAfterThrowingFunction()) {
                std::this_thread::yield();
            }
        }

        std::string const ORIGIN = "SensorCommunicatorTest ThrowingSensorCommunicationStrategyMock";
        ErrorHandling::ErrorCode const ERROR_CODE = ErrorHandling::GatewayErrorCode::EMPTY_CODE;
        std::string const OPEN_ERROR_MESSAGE = "Help!";
        std::string const CLOSE_ERROR_MESSAGE = "By The Beatles";
        std::string const MESSAGE_ERROR_MESSAGE = "Help! I need somebody!!";
        std::string const RAW_DATA_ERROR_MESSAGE = "Help! Not just anybody!!";
        std::string const MESSAGE = "Help! You know I need someone!!";

    private:

        bool hasErrorBeenThrown() const noexcept {
            return errorThrown.load();
        }

        ErrorHandling::SensorAccessLinkError errorToThrow;

        AtomicFlag errorThrown;
        AtomicFlag openConnectionCalled;
        AtomicFlag closeConnectionCalled;
        AtomicFlag throwOnOpen;
        AtomicFlag throwOnClose;
        AtomicFlag throwOnFetchMessages;
        AtomicFlag throwOnFetchRawDataCycles;

    };
}

#endif //SENSORGATEWAY_ERRORTHROWINGSENSORCOMMUNICATIONSTRATEGYMOCK_HPP
