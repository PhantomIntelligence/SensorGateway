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

        using GetParameterValueContents = typename super::GetParameterValueContentBuffer::Contents;

    public:

        ErrorThrowingServerCommunicationStrategyMock() :
                super(),
                errorToThrow(ErrorHandling::SensorAccessLinkError::returnDefaultData()),
                errorThrown(false),
                throwOnOpen(false),
                throwOnClose(false),
                throwOnSendMessage(false),
                throwOnSendRawData(false),
                throwOnSendAllParameterMetadataResponse(false),
                throwOnSendUnsignedIntegerParameterValueResponse(false),
                throwOnSendSignedIntegerParameterValueResponse(false),
                throwOnSendRealNumberParameterValueResponse(false),
                throwOnSendBooleanParameterValueResponse(false),
                throwOnSendParameterErrorResponse(false),
                throwOnSendSuccessMessageResponse(false),
                throwOnSendErrorMessageResponse(false),
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

        void throwOpenConnectionRequiredErrorWhenSendAllParameterMetadataResponseIsCalled() noexcept {
            throwOnSendAllParameterMetadataResponse.store(true);
            errorToThrow = expectedErrorRequiringOpenConnection();
        }

        void throwOpenConnectionRequiredErrorWhenSendUnsignedIntegerParameterValueResponseIsCalled() noexcept {
            throwOnSendUnsignedIntegerParameterValueResponse.store(true);
            errorToThrow = expectedErrorRequiringOpenConnection();
        }

        void throwOpenConnectionRequiredErrorWhenSendSignedIntegerParameterValueResponseIsCalled() noexcept {
            throwOnSendSignedIntegerParameterValueResponse.store(true);
            errorToThrow = expectedErrorRequiringOpenConnection();
        }

        void throwOpenConnectionRequiredErrorWhenSendRealNumberParameterValueResponseIsCalled() noexcept {
            throwOnSendRealNumberParameterValueResponse.store(true);
            errorToThrow = expectedErrorRequiringOpenConnection();
        }

        void throwOpenConnectionRequiredErrorWhenSendBooleanParameterValueResponseIsCalled() noexcept {
            throwOnSendBooleanParameterValueResponse.store(true);
            errorToThrow = expectedErrorRequiringOpenConnection();
        }

        void throwOpenConnectionRequiredErrorWhenSendParameterErrorResponseIsCalled() noexcept {
            throwOnSendParameterErrorResponse.store(true);
            errorToThrow = expectedErrorRequiringOpenConnection();
        }

        void throwOpenConnectionRequiredErrorWhenSendSuccessMessageResponseIsCalled() noexcept {
            throwOnSendSuccessMessageResponse.store(true);
            errorToThrow = expectedErrorRequiringOpenConnection();
        }

        void throwOpenConnectionRequiredErrorWhenSendErrorMessageResponseIsCalled() noexcept {
            throwOnSendErrorMessageResponse.store(true);
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

        void throwCloseConnectionRequiredErrorWhenSendAllParameterMetadataResponseIsCalled() noexcept {
            throwOnSendAllParameterMetadataResponse.store(true);
            errorToThrow = expectedErrorRequiringCloseConnection();
        }

        void throwCloseConnectionRequiredErrorWhenSendUnsignedIntegerParameterValueResponseIsCalled() noexcept {
            throwOnSendUnsignedIntegerParameterValueResponse.store(true);
            errorToThrow = expectedErrorRequiringCloseConnection();
        }

        void throwCloseConnectionRequiredErrorWhenSendSignedIntegerParameterValueResponseIsCalled() noexcept {
            throwOnSendSignedIntegerParameterValueResponse.store(true);
            errorToThrow = expectedErrorRequiringCloseConnection();
        }

        void throwCloseConnectionRequiredErrorWhenSendRealNumberParameterValueResponseIsCalled() noexcept {
            throwOnSendRealNumberParameterValueResponse.store(true);
            errorToThrow = expectedErrorRequiringCloseConnection();
        }

        void throwCloseConnectionRequiredErrorWhenSendBooleanParameterValueResponseIsCalled() noexcept {
            throwOnSendBooleanParameterValueResponse.store(true);
            errorToThrow = expectedErrorRequiringCloseConnection();
        }

        void throwCloseConnectionRequiredErrorWhenSendParameterErrorResponseIsCalled() noexcept {
            throwOnSendParameterErrorResponse.store(true);
            errorToThrow = expectedErrorRequiringCloseConnection();
        }

        void throwCloseConnectionRequiredErrorWhenSendSuccessMessageResponseIsCalled() noexcept {
            throwOnSendSuccessMessageResponse.store(true);
            errorToThrow = expectedErrorRequiringCloseConnection();
        }

        void throwCloseConnectionRequiredErrorWhenSendErrorMessageResponseIsCalled() noexcept {
            throwOnSendErrorMessageResponse.store(true);
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

        ErrorHandling::SensorAccessLinkError
        expectedErrorWhenSendResponseIsCalled(std::string const& errorMessage) noexcept {
            return ErrorHandling::SensorAccessLinkError(ORIGIN,
                                                        ErrorHandling::Category::COMMUNICATION_ERROR,
                                                        ErrorHandling::Severity::ERROR,
                                                        ERROR_CODE,
                                                        errorMessage);
        }

        ErrorHandling::SensorAccessLinkError expectedErrorWhenSendResponseIsCalled() noexcept {
            return expectedErrorWhenSendResponseIsCalled(SEND_RESPONSE);
        }

        void throwErrorWhenSendParameterErrorResponseIsCalled() noexcept {
            throwOnSendParameterErrorResponse.store(true);
            errorToThrow = expectedErrorWhenSendResponseIsCalled();
        }

        void throwErrorWhenSendErrorMessageResponseIsCalled() noexcept {
            throwOnSendErrorMessageResponse.store(true);
            errorToThrow = expectedErrorWhenSendResponseIsCalled();
        }

        std::tuple<size_t, GetParameterValueContents> fetchGetParameterValueContents() override {
            if (hasCloseConnectionBeenCalled()) {
                // WARNING! This mock implementation of readMessage needs to be slowed down because of the way gtest works. DO NOT REMOVE.
                std::this_thread::sleep_for(std::chrono::milliseconds(5));
                yield();
            }

            handleThrowConfirmation(&throwOnFetchGetParameterValueContents);
            GetParameterValueContents getParameterValueContents{{" "}};
            return std::make_tuple(1, getParameterValueContents);
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
            handleThrowConfirmation(&throwOnSendMessage);
        }

        void sendRawData(typename super::RawData&& rawData) override {
            handleThrowConfirmation(&throwOnSendRawData);
        }

        void sendResponse(typename super::AllParameterMetadataResponse&& response) override {
            handleThrowConfirmation(&throwOnSendAllParameterMetadataResponse);
        }

        void sendResponse(typename super::UnsignedIntegerParameterResponse&& response) override {
            handleThrowConfirmation(&throwOnSendUnsignedIntegerParameterValueResponse);
        }

        void sendResponse(typename super::SignedIntegerParameterResponse&& response) override {
            handleThrowConfirmation(&throwOnSendSignedIntegerParameterValueResponse);
        }

        void sendResponse(typename super::RealNumberParameterResponse&& response) override {
            handleThrowConfirmation(&throwOnSendRealNumberParameterValueResponse);
        }

        void sendResponse(typename super::BooleanParameterResponse&& response) override {
            handleThrowConfirmation(&throwOnSendBooleanParameterValueResponse);
        }

        void sendResponse(typename super::ParameterErrorResponse&& parameterErrorResponse) override {
            handleThrowConfirmation(&throwOnSendParameterErrorResponse);
        }

        void sendResponse(typename super::SuccessMessageResponse&& successMessageResponse) override {
            handleThrowConfirmation(&throwOnSendSuccessMessageResponse);
        }

        void sendResponse(typename super::ErrorMessageResponse&& errorMessageResponse) override {
            handleThrowConfirmation(&throwOnSendErrorMessageResponse);
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
        std::string const MESSAGE = "Dust in the wind;";
        std::string const SEND_RESPONSE = "All they are is dust in the wind.";

    private:

        void handleThrowConfirmation(AtomicFlag* throwOnCallFlag) {
            if (!errorThrown.load()) {
                if (throwOnCallFlag->load()) {
                    throwOnCallFlag->store(false); // We only throw once to allow the test to pass
                    openConnectionCalled.store(false);
                    closeConnectionCalled.store(false);
                    errorThrown.store(true);
                    throw errorToThrow;
                }
            }
        }

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
        AtomicFlag throwOnSendAllParameterMetadataResponse;
        AtomicFlag throwOnSendUnsignedIntegerParameterValueResponse;
        AtomicFlag throwOnSendSignedIntegerParameterValueResponse;
        AtomicFlag throwOnSendRealNumberParameterValueResponse;
        AtomicFlag throwOnSendBooleanParameterValueResponse;
        AtomicFlag throwOnSendParameterErrorResponse;
        AtomicFlag throwOnSendSuccessMessageResponse;
        AtomicFlag throwOnSendErrorMessageResponse;

    };
}

#endif //SENSORGATEWAY_ERRORTHROWINGSERVERCOMMUNICATIONSTRATEGYMOCK_HPP
