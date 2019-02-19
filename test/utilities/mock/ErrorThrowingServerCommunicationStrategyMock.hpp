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

#ifndef SENSORGATEWAY_ERRORTHROWINGSERVERCOMMUNICATIONSTRATEGYMOCK_HPP
#define SENSORGATEWAY_ERRORTHROWINGSERVERCOMMUNICATIONSTRATEGYMOCK_HPP

#include "test/utilities/data-model/DataModelFixture.h"
#include "sensor-gateway/server-communication/ServerCommunicationStrategy.hpp"

namespace Mock {

    template<typename T>
    class ErrorThrowingServerCommunicationStrategyMock :
            public ServerCommunication::ServerCommunicationStrategy<T> {
    protected:

        using super = ServerCommunication::ServerCommunicationStrategy<T>;

        using GetParameterValueContents = typename super::GetParameterValueContents;

    public:

        ErrorThrowingServerCommunicationStrategyMock() :
                super(),
                errorToThrow(ErrorHandling::SensorAccessLinkError::returnDefaultData()),
                errorThrown(false),
                throwOnOpen(false),
                throwOnClose(false),
                throwOnSendMessage(false),
                throwOnSendRawData(false),
                openConnectionCalled(false),
                closeConnectionCalled(false) {
        }

        ~ErrorThrowingServerCommunicationStrategyMock() noexcept final = default;

        ErrorThrowingServerCommunicationStrategyMock(
                ErrorThrowingServerCommunicationStrategyMock const& other) = delete;

        ErrorThrowingServerCommunicationStrategyMock(
                ErrorThrowingServerCommunicationStrategyMock&& other) noexcept = delete;

        ErrorThrowingServerCommunicationStrategyMock&
        operator=(ErrorThrowingServerCommunicationStrategyMock const& other)& = delete;

        ErrorThrowingServerCommunicationStrategyMock&
        operator=(ErrorThrowingServerCommunicationStrategyMock&& other)& noexcept = delete;

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

        void throwOpenConnectionRequiredErrorWhenSendMessageIsCalled() noexcept {
            throwOnSendMessage.store(true);
            errorToThrow = expectedErrorRequiringOpenConnection();

        }

        void throwOpenConnectionRequiredErrorWhenSendRawDataIsCalled() noexcept {
            throwOnSendRawData.store(true);
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

        void throwCloseConnectionRequiredErrorWhenSendMessageIsCalled() noexcept {
            throwOnSendMessage.store(true);
            errorToThrow = expectedErrorRequiringCloseConnection();
        }

        void throwCloseConnectionRequiredErrorWhenSendRawDataIsCalled() noexcept {
            throwOnSendRawData.store(true);
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

        void throwErrorWhenFetchGetParameterValueContentsIsCalled() noexcept {
            throwOnFetchGetParameterValueContents.store(true);
            errorToThrow = expectedErrorWhenFetchGetParameterValueContentsIsCalled();
        }

        ErrorHandling::SensorAccessLinkError expectedErrorWhenFetchGetParameterValueContentsIsCalled() noexcept {
            return ErrorHandling::SensorAccessLinkError(ORIGIN,
                                                        ErrorHandling::Category::COMMUNICATION_ERROR,
                                                        ErrorHandling::Severity::ERROR,
                                                        ERROR_CODE,
                                                        FETCH_GET_PARAMETER_VALUE_CONTENT_ERROR_MESSAGE);
        }


        void throwErrorWhenCloseConnectionIsCalled() noexcept {
            throwOnClose.store(true);
            errorToThrow = expectedErrorWhenCloseConnectionIsCalled();
        }


        ErrorHandling::SensorAccessLinkError expectedErrorWhenSendMessageIsCalled() noexcept {
            return ErrorHandling::SensorAccessLinkError(ORIGIN,
                                                        ErrorHandling::Category::COMMUNICATION_ERROR,
                                                        ErrorHandling::Severity::ERROR,
                                                        ERROR_CODE,
                                                        MESSAGE_ERROR_MESSAGE);
        }

        void throwErrorWhenSendMessageIsCalled() noexcept {
            throwOnSendMessage.store(true);
            errorToThrow = expectedErrorWhenSendMessageIsCalled();
        }

        ErrorHandling::SensorAccessLinkError expectedErrorWhenSendRawDataIsCalled() noexcept {
            return ErrorHandling::SensorAccessLinkError(ORIGIN,
                                                        ErrorHandling::Category::COMMUNICATION_ERROR,
                                                        ErrorHandling::Severity::ERROR,
                                                        ERROR_CODE,
                                                        RAW_DATA_ERROR_MESSAGE);
        }

        void throwErrorWhenSendRawDataIsCalled() noexcept {
            throwOnSendRawData.store(true);
            errorToThrow = expectedErrorWhenSendRawDataIsCalled();
        }

        GetParameterValueContents fetchGetParameterValueContents() override {
            if (hasCloseConnectionBeenCalled()) {
                // WARNING! This mock implementation of readMessage needs to be slowed down because of the way gtest works. DO NOT REMOVE.
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                yield();
            }
            if (!errorThrown.load()) {
                if (throwOnFetchGetParameterValueContents.load()) {
                    throwOnFetchGetParameterValueContents.store(false); // We only throw once to allow the test to pass
                    openConnectionCalled.store(false);
                    closeConnectionCalled.store(false);
                    errorThrown.store(true);
                    throw errorToThrow;
                }
            }
            GetParameterValueContents getParameterValueContents{{" "}};
            return getParameterValueContents;
        }

        void openConnection(std::string const& serverAddress) override {
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

        void sendMessage(typename super::Message&& message) override {
            if (!errorThrown.load()) {
                if (throwOnSendMessage.load()) {
                    throwOnSendMessage.store(false); // We only throw once to allow the test to pass
                    openConnectionCalled.store(false);
                    closeConnectionCalled.store(false);
                    errorThrown.store(true);
                    throw errorToThrow;
                }
            }
        }

        void sendRawData(typename super::RawData&& rawData) override {
            if (!errorThrown.load()) {
                if (throwOnSendRawData.load()) {
                    throwOnSendRawData.store(false); // We only throw once to allow the test to pass
                    openConnectionCalled.store(false);
                    closeConnectionCalled.store(false);
                    errorThrown.store(true);
                    throw errorToThrow;
                }
            }
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
                yield();
            }
        }

        void waitUntilCloseConnectionCallIsMadeAfterErrorIsThrown() {
            while (!hasCloseConnectionBeenCalledAfterThrowingFunction()) {
                yield();
            }
        }

        std::string const ORIGIN = "ServerCommunicatorTest ErrorThrowingServerCommunicationStrategyMockMock";
        ErrorHandling::ErrorCode const ERROR_CODE = ErrorHandling::GatewayErrorCode::EMPTY_CODE;
        std::string const OPEN_ERROR_MESSAGE = "Dust in the Wind";
        std::string const CLOSE_ERROR_MESSAGE = "By Kansas";
        std::string const FETCH_GET_PARAMETER_VALUE_CONTENT_ERROR_MESSAGE = "  -  -  -  -  -  ";
        std::string const MESSAGE_ERROR_MESSAGE = "I close my eyes, only for a moment, and the moment's gone";
        std::string const RAW_DATA_ERROR_MESSAGE = "All my dreams pass before my eyes, a curiosity";
        std::string const MESSAGE = "Dust in the wind; All they are is dust in the wind.";

    private:

        bool hasErrorBeenThrown() const noexcept {
            return errorThrown.load();
        }

        ErrorHandling::SensorAccessLinkError errorToThrow;

        AtomicFlag openConnectionCalled;
        AtomicFlag closeConnectionCalled;

        AtomicFlag errorThrown;
        AtomicFlag throwOnOpen;
        AtomicFlag throwOnClose;
        AtomicFlag throwOnFetchGetParameterValueContents;
        AtomicFlag throwOnSendMessage;
        AtomicFlag throwOnSendRawData;

    };
}

#endif //SENSORGATEWAY_ERRORTHROWINGSERVERCOMMUNICATIONSTRATEGYMOCK_HPP
