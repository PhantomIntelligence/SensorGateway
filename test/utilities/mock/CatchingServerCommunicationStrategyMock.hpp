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

        using GetParameterValueContent = typename super::GetParameterValueContent;
        using GetParameterValueContents = typename super::GetParameterValueContents;
        using GetParameterValueContentList = std::list<GetParameterValueContent>;
//        using SetParameterValueBooleanContent = std::list<super::SetParameterValueBooleanContent>;
        using ErrorMessageResponse = typename super::ErrorMessageResponse;

    public:
        CatchingServerCommunicationStrategyMock() :
                openConnectionCalled(false),
                sendMessageCalled(false),
                sendRawDataCalled(false),
                sendErrorMessageResponseCalled(false),
                closeConnectionCalled(false),
                fetchGetParameterValueContentsCalled(false),
                hasToReturnSpecificGetParameterValueContents(false) {}

        ~CatchingServerCommunicationStrategyMock() noexcept override = default;

        void openConnection(std::string const& serverAddress) override {
            openConnectionCalled.store(true);
        }

        GetParameterValueContents fetchGetParameterValueContents() override {
            acknowledgeGetParameterValueContentsHasBeenCalled();

            if (hasToReturnSpecificGetParameterValueContents && !getParameterValueContentsToReturn.empty()) {
                GetParameterValueContents getParameterValueContents;
                auto numberOfValues = getParameterValueContentsToReturn.size();
                for (auto i = 0; i < numberOfValues; ++i) {
                    getParameterValueContents.at(i) = getParameterValueContentsToReturn.front();
                    getParameterValueContentsToReturn.pop_front();
                }
                return getParameterValueContents;
            }

            // WARNING! This mock implementation of fetchGetParameterValueContents needs to be slowed down because the way gtest works. DO NOT REMOVE.
            yield();

            GetParameterValueContent getParameterValueContent;
            GetParameterValueContents getParameterValueContents = {getParameterValueContent};
            return getParameterValueContents;
        };

        void sendMessage(Message&& message) override {
            sendMessageCalled.store(true);
            receivedMessages.push_back(message);
        }

        void sendRawData(RawData&& rawData) override {
            sendRawDataCalled.store(true);
            receivedRawData.push_back(rawData);
        }

        void sendResponse(ErrorMessageResponse&& errorMessageResponse) override {
            sendErrorMessageResponseCalled.store(true);
            sentErrorMessageResponse = errorMessageResponse;
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

        bool hasSendErrorMessageResponseBeenCalled() const {
            return sendErrorMessageResponseCalled.load();
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

        ErrorMessageResponse const& getSentErrorMessageResponse() const {
            return sentErrorMessageResponse;
        };

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
//                if (!hasFetchRawDataCyclesBeenCalled()) {
//                    fetchMessagesCalledBeforeFetchRawDataCycles.store(true);
//                }
            }
        }

        Messages receivedMessages;
        RawDataCycles receivedRawData;
        AtomicFlag openConnectionCalled;
        AtomicFlag sendMessageCalled;
        AtomicFlag sendRawDataCalled;
        AtomicFlag sendErrorMessageResponseCalled;
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

        ErrorMessageResponse sentErrorMessageResponse;
    };
}

#endif //SENSORGATEWAY_CATCHINGSERVERCOMMUNICATIONSTRATEGYMOCK_HPP
