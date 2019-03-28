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

#ifndef SENSORGATEWAY_SERVERCOMMUNICATIONPROTOCOLSTRATEGY_HPP
#define SENSORGATEWAY_SERVERCOMMUNICATIONPROTOCOLSTRATEGY_HPP

#include "ServerRequest.hpp"
#include "ServerResponse.hpp"

namespace ServerCommunication {

    namespace Details {


        template<typename ContentType, size_t maxNumberOfConcurrentlyHandledRequests, size_t maxBulkReceptionSize>
        class RequestContentBuffer {

        protected:

        public:

            using Content = ContentType;
            static constexpr size_t const size = maxNumberOfConcurrentlyHandledRequests;
            static constexpr size_t const bulkSize = maxBulkReceptionSize;
            using Contents = std::array<Content, size>;
            using BulkContents = std::array<Content, bulkSize>;
            using ContentList = std::list<Content>;

            explicit RequestContentBuffer() {
            };

            ~RequestContentBuffer() noexcept = default;

            auto fetchReceivedContent() -> std::tuple<size_t, Contents> {
                LockGuard guard(contentMutex); // ensures that requests are treated at most one bulk at the time

                Contents contentsToReturn;

                size_t contentCount = 0u;
                while (hasReceivedNewUntransmittedContent() && contentCount < contentsToReturn.size()) {
                    auto getParameterValueContent = untransmittedReceivedContent.front();
                    contentsToReturn[contentCount] = getParameterValueContent;
                    untransmittedReceivedContent.pop_front();
                    ++contentCount;
                }

                return std::make_tuple(contentCount, contentsToReturn);
            }

            void receiveNewContentInBulk(BulkContents&& newContents) {
                LockGuard guard(contentMutex);

                auto counter = 0u;
                while (counter != newContents.max_size()) {
                    untransmittedReceivedContent.emplace_back(std::move[newContents++]);
                }
            }

            void receiveNewContent(Content&& newContent) {
                LockGuard guard(contentMutex);
                untransmittedReceivedContent.emplace_back(std::forward<Content>(newContent));
            }

        private:

            bool hasReceivedNewUntransmittedContent() const noexcept {
                return !untransmittedReceivedContent.empty();
            }

            mutable Mutex contentMutex;
            ContentList untransmittedReceivedContent;
        };
    }

    template<class T>
    class ServerCommunicationStrategy {

    protected:

        static constexpr size_t const MAX_NUMBER_OF_CONCURRENT_REQUEST_OF_ONE_KIND = T::MAX_NUMBER_OF_CONCURRENT_REQUEST_OF_ONE_KIND;
        static constexpr size_t const MAX_NUMBER_OF_PARAMETER_FOR_BULK_OPERATIONS = T::Parameters::NUMBER_OF_AVAILABLE_PARAMETERS;

        template<typename Content>
        using RequestContentBuffer =Details::RequestContentBuffer<
                Content,
                MAX_NUMBER_OF_CONCURRENT_REQUEST_OF_ONE_KIND,
                MAX_NUMBER_OF_PARAMETER_FOR_BULK_OPERATIONS
        >;

    public:

        using Message = typename T::Message;
        using RawData = typename T::RawData;

        using ParameterName = std::string;

        using GetParameterValueContent = ParameterName;
        using SetUnsignedIntegerParameterValueContent = std::pair<ParameterName, UnsignedInteger>;
        using SetSignedIntegerParameterValueContent = std::pair<ParameterName, SignedInteger>;
        using SetRealNumberParameterValueContent = std::pair<ParameterName, RealNumber>;
        using SetBooleanParameterValueContent = std::pair<ParameterName, bool>;

        using AllParameterNamesList = std::array<ParameterName, MAX_NUMBER_OF_PARAMETER_FOR_BULK_OPERATIONS>;

        using GetParameterValueContentBuffer = RequestContentBuffer<ParameterName>;
        using SetUnsignedIntegerParameterValueContentBuffer = RequestContentBuffer<SetUnsignedIntegerParameterValueContent>;
        using SetSignedIntegerParameterValueContentBuffer = RequestContentBuffer<SetSignedIntegerParameterValueContent>;
        using SetRealNumberParameterValueContentBuffer = RequestContentBuffer<SetRealNumberParameterValueContent>;
        using SetBooleanParameterValueContentBuffer = RequestContentBuffer<SetBooleanParameterValueContent>;

        using AllParameterMetadataResponse = ResponseType::AllParameterMetadataResponse<T::Parameters::NUMBER_OF_AVAILABLE_PARAMETERS>;
        using UnsignedIntegerParameterResponse = ResponseType::UnsignedIntegerParameterResponse;
        using SignedIntegerParameterResponse = ResponseType::SignedIntegerParameterResponse;
        using RealNumberParameterResponse = ResponseType::RealNumberParameterResponse;
        using BooleanParameterResponse = ResponseType::BooleanParameterResponse;
        using ParameterErrorResponse = ResponseType::ParameterErrorResponse;
        using SuccessMessageResponse = ResponseType::SuccessMessageResponse;
        using ErrorMessageResponse = ResponseType::ErrorMessageResponse;

        ServerCommunicationStrategy() noexcept :
                receivedGetAllParameterNamesRequest(false),
                receivedCalibrationRequest(false),
                receivedClearCalibrationRequest(false) {
        }

        virtual ~ServerCommunicationStrategy() noexcept = default;

        virtual void openConnection(std::string const& serverAddress) = 0;

        bool hasReceivedGetAllParameterNamesRequest() noexcept {
            auto receivedRequest = checkIfRequestHasBeenReceived(&receivedGetAllParameterNamesRequest);
            return receivedRequest;
        }

        // TODO: test this function
        virtual auto fetchGetParameterValueContents()
        -> decltype(std::declval<GetParameterValueContentBuffer>().fetchReceivedContent()) {
            return getParameterValueContentBuffer.fetchReceivedContent();
        }

        // TODO: test this function
        virtual auto fetchSetUnsignedIntegerParameterValueContents()
        -> decltype(std::declval<SetUnsignedIntegerParameterValueContentBuffer>().fetchReceivedContent()) {
            return setUnsignedIntegerParameterValueContentBuffer.fetchReceivedContent();
        }

        // TODO: test this function
        virtual auto fetchSetSignedIntegerParameterValueContents()
        -> decltype(std::declval<SetSignedIntegerParameterValueContentBuffer>().fetchReceivedContent()) {
            return setSignedIntegerParameterValueContentBuffer.fetchReceivedContent();
        }

        // TODO: test this function
        virtual auto fetchSetRealNumberParameterValueContents()
        -> decltype(std::declval<SetRealNumberParameterValueContentBuffer>().fetchReceivedContent()) {
            return setRealNumberParameterValueContentBuffer.fetchReceivedContent();
        }

        // TODO: test this function
        virtual auto fetchSetBooleanParameterValueContents()
        -> decltype(std::declval<SetBooleanParameterValueContentBuffer>().fetchReceivedContent()) {
            return setBooleanParameterValueContentBuffer.fetchReceivedContent();
        }

        bool hasReceivedCalibrationRequest() noexcept {
            return checkIfRequestHasBeenReceived(&receivedCalibrationRequest);
        }

        bool hasReceivedClearCalibrationRequest() noexcept {
            return checkIfRequestHasBeenReceived(&receivedClearCalibrationRequest);
        }

        virtual void sendMessage(Message&& message) = 0;

        virtual void sendRawData(RawData&& rawData) = 0;

        virtual void sendResponse(AllParameterMetadataResponse&& allParameterMetadataResponse) = 0;

        virtual void sendResponse(UnsignedIntegerParameterResponse&& unsignedIntegerParameterResponse) = 0;

        virtual void sendResponse(SignedIntegerParameterResponse&& signedIntegerParameterResponse) = 0;

        virtual void sendResponse(RealNumberParameterResponse&& realNumberParameterResponse) = 0;

        virtual void sendResponse(BooleanParameterResponse&& booleanParameterResponse) = 0;

        virtual void sendResponse(ParameterErrorResponse&& parameterErrorResponse) = 0;

        virtual void sendResponse(SuccessMessageResponse&& successMessageResponse) = 0;

        virtual void sendResponse(ErrorMessageResponse&& errorMessageResponse) = 0;

        virtual void closeConnection() = 0;

    protected:

        bool checkIfRequestHasBeenReceived(AtomicFlag* requestReceivedFlag) const {
            bool valueToReturn = requestReceivedFlag->load();
            requestReceivedFlag->store(false);
            return valueToReturn;
        }

        void receiveGetAllParameterNamesRequest() {
            receivedGetAllParameterNamesRequest.store(true);
        }

        // TODO : Test this function
        void receiveGetParameterValueRequest(ParameterName&& parameterName) {
            getParameterValueContentBuffer.receiveNewContent(std::forward<ParameterName>(parameterName));
        }

        // TODO : Test this function
        void receiveSetParameterValueRequest(SetUnsignedIntegerParameterValueContent&& askedParameterValue) {
            setUnsignedIntegerParameterValueContentBuffer
                    .receiveNewContent(std::forward<SetUnsignedIntegerParameterValueContent>(askedParameterValue));
        }

        // TODO : Test this function
        void receiveSetParameterValueRequest(SetSignedIntegerParameterValueContent&& askedParameterValue) {
            setSignedIntegerParameterValueContentBuffer
                    .receiveNewContent(std::forward<SetSignedIntegerParameterValueContent>(askedParameterValue));
        }

        // TODO : Test this function
        void receiveSetParameterValueRequest(SetRealNumberParameterValueContent&& askedParameterValue) {
            setRealNumberParameterValueContentBuffer
                    .receiveNewContent(std::forward<SetRealNumberParameterValueContent>(askedParameterValue));
        }

        // TODO : Test this function
        void receiveSetParameterValueRequest(SetBooleanParameterValueContent&& askedParameterValue) {
            setBooleanParameterValueContentBuffer
                    .receiveNewContent(std::forward<SetBooleanParameterValueContent>(askedParameterValue));
        }

        void receiveCalibrationRequest() {
            receivedCalibrationRequest.store(true);
        }

        void receiveClearCalibrationRequest() {
            receivedClearCalibrationRequest.store(true);
        }

        AtomicFlag receivedGetAllParameterNamesRequest;
        AtomicFlag receivedCalibrationRequest;
        AtomicFlag receivedClearCalibrationRequest;

        GetParameterValueContentBuffer getParameterValueContentBuffer;
        SetUnsignedIntegerParameterValueContentBuffer setUnsignedIntegerParameterValueContentBuffer;
        SetSignedIntegerParameterValueContentBuffer setSignedIntegerParameterValueContentBuffer;
        SetRealNumberParameterValueContentBuffer setRealNumberParameterValueContentBuffer;
        SetBooleanParameterValueContentBuffer setBooleanParameterValueContentBuffer;
    };
}

#endif //SENSORGATEWAY_SERVERCOMMUNICATIONPROTOCOLSTRATEGY_HPP

