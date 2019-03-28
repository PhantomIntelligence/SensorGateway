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

#ifndef SENSORGATEWAY_CATCHINGSERVERCOMMUNICATIONSTRATEGYMOCK_HPP
#define SENSORGATEWAY_CATCHINGSERVERCOMMUNICATIONSTRATEGYMOCK_HPP

#include "sensor-gateway/server-communication/ServerCommunicationStrategy.hpp"
#include "test/utilities/mock/Function.hpp"

namespace Mock {

    template<typename T>
    class CatchingServerCommunicationStrategyMock final
            : public ServerCommunication::ServerCommunicationStrategy<T> {
    protected:
        using super = ServerCommunication::ServerCommunicationStrategy<T>;

        using Message = typename super::Message;
        using Messages = std::list<Message>;

        using RawData = typename super::RawData;
        using RawDataCycles = std::list<RawData>;

        using GetParameterValueContent = typename super::GetParameterValueContentBuffer::Content;
        using GetParameterValueContents = typename super::GetParameterValueContentBuffer::Contents;
        using GetParameterValueContentList = typename super::GetParameterValueContentBuffer::ContentList;

        using AllParameterMetadataResponse = typename super::AllParameterMetadataResponse;
        using UnsignedIntegerParameterResponse = typename super::UnsignedIntegerParameterResponse;
        using SignedIntegerParameterResponse = typename super::SignedIntegerParameterResponse;
        using RealNumberParameterResponse = typename super::RealNumberParameterResponse;
        using BooleanParameterResponse = typename super::BooleanParameterResponse;
        using ParameterErrorResponse = typename super::ParameterErrorResponse;
        using SuccessMessageResponse = typename super::SuccessMessageResponse;
        using ErrorMessageResponse = typename super::ErrorMessageResponse;

        using MockFunctionSendAllParameterMetadataResponse = Mock::Function<StringLiteral<decltype("mock->sendParameter<AllParameterMetadata>"_ToString)>, Mock::VoidType, AllParameterMetadataResponse>;
        using MockFunctionSendUnsignedIntegerParameterResponse = Mock::Function<StringLiteral<decltype("mock->sendParameter<UnsignedInteger>"_ToString)>, Mock::VoidType, UnsignedIntegerParameterResponse>;
        using MockFunctionSendSignedIntegerParameterResponse = Mock::Function<StringLiteral<decltype("mock->sendParameter<SignedInteger>"_ToString)>, Mock::VoidType, SignedIntegerParameterResponse>;
        using MockFunctionSendRealNumberParameterResponse = Mock::Function<StringLiteral<decltype("mock->sendParameter<RealNumber>"_ToString)>, Mock::VoidType, RealNumberParameterResponse>;
        using MockFunctionSendBooleanParameterResponse = Mock::Function<StringLiteral<decltype("mock->sendParameter<Boolean>"_ToString)>, Mock::VoidType, BooleanParameterResponse>;
        using MockFunctionSendParameterErrorResponse = Mock::Function<StringLiteral<decltype("mock->sendParameter<ParameterError>"_ToString)>, Mock::VoidType, ParameterErrorResponse>;
        using MockFunctionSendSuccessMessageResponse = Mock::Function<StringLiteral<decltype("mock->sendParameter<SuccessMessage>"_ToString)>, Mock::VoidType, SuccessMessageResponse>;
        using MockFunctionSendErrorMessageResponse = Mock::Function<StringLiteral<decltype("mock->sendParameter<ErrorMessage>"_ToString)>, Mock::VoidType, ErrorMessageResponse>;

    public:
        CatchingServerCommunicationStrategyMock() :
                openConnectionCalled(false),
                sendMessageCalled(false),
                sendRawDataCalled(false),
                closeConnectionCalled(false),
                fetchGetParameterValueContentsCalled(false),
                hasToReturnSpecificGetParameterValueContents(false) {}

        ~CatchingServerCommunicationStrategyMock() noexcept override = default;

        void openConnection(std::string const& serverAddress) override {
            openConnectionCalled.store(true);
        }

        std::tuple<size_t, GetParameterValueContents> fetchGetParameterValueContents() override {
            acknowledgeGetParameterValueContentsHasBeenCalled();

            if (hasToReturnSpecificGetParameterValueContents && !getParameterValueContentsToReturn.empty()) {
                GetParameterValueContents getParameterValueContents;
                auto numberOfValues = getParameterValueContentsToReturn.size();
                for (auto i = 0; i < numberOfValues; ++i) {
                    getParameterValueContents.at(i) = getParameterValueContentsToReturn.front();
                    getParameterValueContentsToReturn.pop_front();
                }
                return std::make_tuple(numberOfValues, getParameterValueContents);
            }

            // WARNING! This mock implementation of fetchGetParameterValueContents needs to be slowed down because the way gtest works. DO NOT REMOVE.
            yield();

            GetParameterValueContent getParameterValueContent;
            GetParameterValueContents getParameterValueContents = {getParameterValueContent};
            return std::make_tuple(0, getParameterValueContents);
        };

        void sendMessage(Message&& message) override {
            sendMessageCalled.store(true);
            receivedMessages.push_back(message);
        }

        void sendRawData(RawData&& rawData) override {
            sendRawDataCalled.store(true);
            receivedRawData.push_back(rawData);
        }

        void sendResponse(AllParameterMetadataResponse&& response) override {
            mockFunctionSendAllParameterMetadataResponse.invokeVoidReturn(
                    std::forward<AllParameterMetadataResponse>(response));
        }

        void sendResponse(UnsignedIntegerParameterResponse&& unsignedIntegerParameterResponse) override {
            mockFunctionSendUnsignedIntegerParameterResponse.invokeVoidReturn(
                    std::forward<UnsignedIntegerParameterResponse>(unsignedIntegerParameterResponse));
        }

        void sendResponse(SignedIntegerParameterResponse&& signedIntegerParameterResponse) override {
            mockFunctionSendSignedIntegerParameterResponse.invokeVoidReturn(
                    std::forward<SignedIntegerParameterResponse>(signedIntegerParameterResponse));
        }

        void sendResponse(RealNumberParameterResponse&& realNumberParameterResponse) override {
            mockFunctionSendRealNumberParameterResponse.invokeVoidReturn(
                    std::forward<RealNumberParameterResponse>(realNumberParameterResponse));
        }

        void sendResponse(BooleanParameterResponse&& booleanParameterResponse) override {
            mockFunctionSendBooleanParameterResponse.invokeVoidReturn(
                    std::forward<BooleanParameterResponse>(booleanParameterResponse));
        }

        void sendResponse(ParameterErrorResponse&& parameterErrorResponse) override {
            mockFunctionSendParameterErrorResponse.invokeVoidReturn(
                    std::forward<ParameterErrorResponse>(parameterErrorResponse));
        }

        void sendResponse(SuccessMessageResponse&& successMessageResponse) override {
            mockFunctionSendSuccessMessageResponse.invokeVoidReturn(
                    std::forward<SuccessMessageResponse>(successMessageResponse));
        }

        void sendResponse(ErrorMessageResponse&& errorMessageResponse) override {
            mockFunctionSendErrorMessageResponse.invokeVoidReturn(
                    std::forward<ErrorMessageResponse>(errorMessageResponse));
        }

        void closeConnection() override {
            closeConnectionCalled.store(true);
        }

        bool hasOpenConnectionBeenCalled() const {
            return openConnectionCalled.load();
        }

        bool hasFetchGetParameterValueContentsBeenCalled() const {
            return fetchGetParameterValueContentsCalled.load();
        }

        bool hasSendMessageBeenCalled() const {
            return sendMessageCalled.load();
        }

        bool hasSendRawDataBeenCalled() const {
            return sendRawDataCalled.load();
        }

        bool hasSendResponseAllParameterMetadataBeenCalledWith(AllParameterMetadataResponse const& value) const {
            return mockFunctionSendAllParameterMetadataResponse.hasBeenInvokedWith(value);
        }

        bool hasSendUnsignedParameterResponseBeenCalledWith(UnsignedIntegerParameterResponse const& value) const {
            return mockFunctionSendUnsignedIntegerParameterResponse.hasBeenInvokedWith(value);
        };

        bool hasSendSignedParameterResponseBeenCalledWith(SignedIntegerParameterResponse const& value) const {
            return mockFunctionSendSignedIntegerParameterResponse.hasBeenInvokedWith(value);
        };

        bool hasSendRealNumberParameterResponseBeenCalledWith(RealNumberParameterResponse const& value) const {
            return mockFunctionSendRealNumberParameterResponse.hasBeenInvokedWith(value);
        };

        bool hasSendBooleanParameterResponseBeenCalledWith(BooleanParameterResponse const& value) const {
            return mockFunctionSendBooleanParameterResponse.hasBeenInvokedWith(value);
        };

        bool hasSendParameterErrorResponseBeenCalledWith(ParameterErrorResponse const& parameterErrorResponse) const {
            return mockFunctionSendParameterErrorResponse.hasBeenInvokedWith(parameterErrorResponse);
        };

        bool hasSendSuccessMessageResponseBeenCalledWith(SuccessMessageResponse const& successMessageResponse) const {
            return mockFunctionSendSuccessMessageResponse.hasBeenInvokedWith(successMessageResponse);
        };

        bool hasSendErrorMessageResponseBeenCalledWith(ErrorMessageResponse const& errorMessageResponse) const {
            return mockFunctionSendErrorMessageResponse.hasBeenInvokedWith(errorMessageResponse);
        };

        bool hasCloseConnectionBeenCalled() const {
            return closeConnectionCalled.load();
        }

        Message getFirstSentMessage() const {
            if (!hasSendMessageBeenCalled()) {
                return Message::returnDefaultData();
            }
            return receivedMessages.front();
        }

        Messages const& getReceivedMessages() const {
            return receivedMessages;
        }

        RawData getFirstSentRawData() const {
            if (!hasSendRawDataBeenCalled()) {
                return RawData::returnDefaultData();
            }
            return receivedRawData.front();
        }

        RawDataCycles const& getReceivedRawData() const {
            return receivedRawData;
        }

        void waitUntilFetchGetParameterValueContentsIsCalled() {
            if (!hasFetchGetParameterValueContentsBeenCalled()) {
                fetchGetParameterValueContentsCalledAcknowledgement.get_future().wait();
            }
        }

    private:

        void acknowledgeGetParameterValueContentsHasBeenCalled() {
            LockGuard sequenceGuard(callSequenceMutex);
            LockGuard guard(fetchGetParameterValueContentsAckMutex);
            if (!hasFetchGetParameterValueContentsBeenCalled()) {
                fetchGetParameterValueContentsCalled.store(true);
                fetchGetParameterValueContentsCalledAcknowledgement.set_value(true);
//                if (!hasFetchRawDataCyclesBeenInvoked()) {
//                    fetchMessagesInvokedBeforeFetchRawDataCycles.store(true);
//                }
            }
        }

        Messages receivedMessages;
        RawDataCycles receivedRawData;
        AtomicFlag openConnectionCalled;
        AtomicFlag sendMessageCalled;
        AtomicFlag sendRawDataCalled;
        AtomicFlag closeConnectionCalled;

        // TODO : Move the following in template (see TODO a few lines below)

        Mutex callSequenceMutex;

        AtomicFlag fetchGetParameterValueContentsCalled;
        Mutex fetchGetParameterValueContentsAckMutex;
        mutable BooleanPromise fetchGetParameterValueContentsCalledAcknowledgement;
        bool hasToReturnSpecificGetParameterValueContents;
        GetParameterValueContentList getParameterValueContentsToReturn;

        // TODO: Create a template class to inherit from when implementing this :
//        AtomicFlag fetchSetParameterValueContentsCalled;
//        Mutex fetchSetParameterValueContentsAckMutex;
//        mutable BooleanPromise fetchSetParameterValueContentsCalledAcknowledgement;
//        bool hasToReturnSpecificSetParameterValueContents;
//        SetParameterValueContentList setParameterValueContentsToReturn;

        MockFunctionSendAllParameterMetadataResponse mockFunctionSendAllParameterMetadataResponse;
        MockFunctionSendUnsignedIntegerParameterResponse mockFunctionSendUnsignedIntegerParameterResponse;
        MockFunctionSendSignedIntegerParameterResponse mockFunctionSendSignedIntegerParameterResponse;
        MockFunctionSendRealNumberParameterResponse mockFunctionSendRealNumberParameterResponse;
        MockFunctionSendBooleanParameterResponse mockFunctionSendBooleanParameterResponse;
        MockFunctionSendParameterErrorResponse mockFunctionSendParameterErrorResponse;
        MockFunctionSendSuccessMessageResponse mockFunctionSendSuccessMessageResponse;
        MockFunctionSendErrorMessageResponse mockFunctionSendErrorMessageResponse;
    };
}

#endif //SENSORGATEWAY_CATCHINGSERVERCOMMUNICATIONSTRATEGYMOCK_HPP
