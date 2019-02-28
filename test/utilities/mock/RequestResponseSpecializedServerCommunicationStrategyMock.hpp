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

#ifndef SENSORGATEWAY_REQUESTRESPONSESPECIALIZEDSERVERCOMMUNICATIONSTRATEGYMOCK_HPP
#define SENSORGATEWAY_REQUESTRESPONSESPECIALIZEDSERVERCOMMUNICATIONSTRATEGYMOCK_HPP

#include <test/utilities/data-model/DataModelFixture.h>
#include "sensor-gateway/server-communication/ServerCommunicationStrategy.hpp"
#include "test/utilities/data-model/FakeParameterConstants.hpp"

namespace Mock {

    /**
     * @brief This mock serves is used for Medium and Big tests.
     * It will not retain which call has been made.
     * It offers handy setting-up methods to ensure test cases are easily covered.
     */
    template<typename T>
    class RequestResponseSpecializedServerCommunicationStrategyMock final
            : public ServerCommunication::ServerCommunicationStrategy<T> {

    protected:

        using super = ServerCommunication::ServerCommunicationStrategy<T>;

        using GetParameterValueContent = typename super::GetParameterValueContent;
        using GetParameterValueContents = typename super::GetParameterValueContents;
        using GetParameterValueContentList = std::list<GetParameterValueContent>;
//        using SetParameterValueBooleanContent = std::list<super::SetParameterValueBooleanContent>;
        using ParameterErrorResponse = typename super::ParameterErrorResponse;
        using ErrorMessageResponse = typename super::ErrorMessageResponse;

        using ParameterNames = std::list<std::string>;

    public:
        RequestResponseSpecializedServerCommunicationStrategyMock() :
                validParameterNames(TestFunctions::Parameters::availableNames<typename T::Parameters>()),
                invalidParameterNames(TestFunctions::Parameters::nonAvailableNames<typename T::Parameters>()),
                // GetParameterValue
                numberOfUniqueValidGetParameterValueContentsToReturn(0),
                numberOfDuplicateValidGetParameterValueContentsToReturn(0),
                numberOfUniqueInvalidGetParameterValueContentsToReturn(0),
                numberOfDuplicateInvalidGetParameterValueContentsToReturn(0),
                // SetParameterValue
                // Response
                sendResponseParameterErrorCalled(false),
                sendResponseErrorMessageCalled(false) {}

        ~RequestResponseSpecializedServerCommunicationStrategyMock() noexcept override = default;

        void openConnection(std::string const& serverAddress) override {
        }

        GetParameterValueContents fetchGetParameterValueContents() override {
            LockGuard guard(getParameterValueMutex);

            GetParameterValueContents getParameterValueContents;

            // Fix the number of request to return so it won't be changed mid-operation (thread safety)
            auto numberOfUniqueValid = loadMockSetting(&numberOfUniqueValidGetParameterValueContentsToReturn);
            auto numberOfDuplicateValid = loadMockSetting(&numberOfDuplicateValidGetParameterValueContentsToReturn);
            auto numberOfUniqueInvalid = loadMockSetting(&numberOfUniqueInvalidGetParameterValueContentsToReturn);
            auto numberOfDuplicateInvalid = loadMockSetting(&numberOfDuplicateInvalidGetParameterValueContentsToReturn);

            auto totalNumberOfRequestToReturn = numberOfUniqueValid + numberOfDuplicateValid +
                                                numberOfUniqueInvalid + numberOfDuplicateInvalid;
            assert(totalNumberOfRequestToReturn <= T::MAX_NUMBER_OF_CONCURRENT_REQUEST_OF_ONE_KIND);

            auto returnedContentStartIterator = getParameterValueContents.begin();

            if (numberOfUniqueValid > 0) {
                std::copy_n(validParameterNames.begin(), numberOfUniqueValid, returnedContentStartIterator);

                for (auto counter = 0u; counter < numberOfUniqueValid; ++counter) {
                    returnedContentStartIterator++;

                    // Reset after request are "Received" by the SensorGateway
                    decrementMockSetting(&numberOfUniqueValidGetParameterValueContentsToReturn);
                }
            }

            if (numberOfDuplicateValid > 0) {
                uint16_t duplicateValidRequestCounter = 0;
                while (duplicateValidRequestCounter != numberOfDuplicateValid) {
                    std::copy_n(validParameterNames.begin(), 1, returnedContentStartIterator);
                    returnedContentStartIterator++;
                    duplicateValidRequestCounter++;

                    // Reset after request are "Received" by the SensorGateway
                    decrementMockSetting(&numberOfDuplicateValidGetParameterValueContentsToReturn);
                }
            }

            if (numberOfUniqueInvalid > 0) {
                std::copy_n(invalidParameterNames.begin(), numberOfUniqueInvalid, returnedContentStartIterator);

                for (auto counter = 0u; counter < numberOfUniqueInvalid; ++counter) {
                    returnedContentStartIterator++;

                    // Reset after request are "Received" by the SensorGateway
                    decrementMockSetting(&numberOfUniqueInvalidGetParameterValueContentsToReturn);
                }
            }

            // Invalid duplicate requests
            if (numberOfDuplicateInvalid > 0) {
                uint16_t duplicateInvalidRequestCounter = 0;
                while (duplicateInvalidRequestCounter != numberOfDuplicateInvalid) {
                    std::copy_n(invalidParameterNames.begin(), 1, returnedContentStartIterator);
                    returnedContentStartIterator++;
                    duplicateInvalidRequestCounter++;

                    // Reset after request are "Received" by the SensorGateway
                    decrementMockSetting(&numberOfDuplicateInvalidGetParameterValueContentsToReturn);
                }
            }


            // Backup for test validation
            std::copy_n(getParameterValueContents.cbegin(), totalNumberOfRequestToReturn,
                        returnedGetParameterValueContents.begin());

            return getParameterValueContents;
        };

        void sendMessage(typename super::Message&& message) override {
        }

        void sendRawData(typename super::RawData&& rawData) override {
        }

        void sendResponse(ParameterErrorResponse&& parameterErrorResponse) override {
            acknowledgeSendResponseParameterErrorHasBeenCalled();
        }

        void sendResponse(ErrorMessageResponse&& errorMessageResponse) override {
            acknowledgeSendResponseErrorMessageHasBeenCalled();
        }

        void closeConnection() override {
        }

        void waitUntilSendResponseParameterErrorIsCalled() {
            if (!hasSendResponseParameterErrorBeenCalled()) {
                sendResponseParameterErrorCalledAcknowledgement.get_future().wait();
            }
        }

        void waitUntilSendResponseErrorMessageIsCalled() {
            if (!hasSendResponseErrorMessageBeenCalled()) {
                sendResponseErrorMessageCalledAcknowledgement.get_future().wait();
            }
        }

        void increaseNumberOfUniqueValidGetParameterValueContentsToReturnBy(uint16_t newNumberToReturn) {
            for (auto i = 0u; i < newNumberToReturn; ++i) {
                incrementMockSetting(&numberOfUniqueValidGetParameterValueContentsToReturn);
            }
        }

        void increaseNumberOfDuplicateValidGetParameterValueContentsToReturnBy(uint16_t newNumberToReturn) {
            for (auto i = 0u; i < newNumberToReturn; ++i) {
                incrementMockSetting(&numberOfDuplicateValidGetParameterValueContentsToReturn);
            }
        }

        void increaseNumberOfUniqueInvalidGetParameterValueContentsToReturnBy(uint16_t newNumberToReturn) {
            for (auto i = 0u; i < newNumberToReturn; ++i) {
                incrementMockSetting(&numberOfUniqueInvalidGetParameterValueContentsToReturn);
            }
        }

        void increaseNumberOfDuplicateInvalidGetParameterValueContentsToReturnBy(uint16_t newNumberToReturn) {
            for (auto i = 0u; i < newNumberToReturn; ++i) {
                incrementMockSetting(&numberOfDuplicateInvalidGetParameterValueContentsToReturn);
            }
        }

        GetParameterValueContents const& getReturnedGetParameterValueRequest() noexcept {
            LockGuard guard(getParameterValueMutex);
            return returnedGetParameterValueContents;
        }

        bool hasSendResponseParameterErrorBeenCalled() const {
            return sendResponseParameterErrorCalled.load();
        }

        bool hasSendResponseErrorMessageBeenCalled() const {
            return sendResponseErrorMessageCalled.load();
        }

    private:

        void incrementMockSetting(AtomicCounter* atomicCounter) {
            LockGuard guard(mockSettingsMutex);
            ++(*atomicCounter);
        }

        void decrementMockSetting(AtomicCounter* atomicCounter) {
            LockGuard guard(mockSettingsMutex);
            --(*atomicCounter);
        }

        auto loadMockSetting(AtomicCounter* atomicCounter) {
            LockGuard guard(mockSettingsMutex);
            return atomicCounter->load();
        }

        void acknowledgeSendResponseParameterErrorHasBeenCalled() {
            LockGuard guard(sendResponseParameterErrorAckMutex);
            if (!hasSendResponseErrorMessageBeenCalled()) {
                sendResponseParameterErrorCalled.store(true);
                sendResponseParameterErrorCalledAcknowledgement.set_value(true);
            }
        }

        void acknowledgeSendResponseErrorMessageHasBeenCalled() {
            LockGuard guard(sendResponseErrorMessageAckMutex);
            if (!hasSendResponseErrorMessageBeenCalled()) {
                sendResponseErrorMessageCalled.store(true);
                sendResponseErrorMessageCalledAcknowledgement.set_value(true);
            }
        }

        ParameterNames validParameterNames;
        ParameterNames invalidParameterNames;

        AtomicFlag sendResponseErrorMessageCalled;
        AtomicFlag sendResponseParameterErrorCalled;
        Mutex sendResponseParameterErrorAckMutex;
        Mutex sendResponseErrorMessageAckMutex;
        mutable BooleanPromise sendResponseParameterErrorCalledAcknowledgement;
        mutable BooleanPromise sendResponseErrorMessageCalledAcknowledgement;

        Mutex getParameterValueMutex;
        Mutex mockSettingsMutex;
        AtomicCounter numberOfUniqueValidGetParameterValueContentsToReturn;
        AtomicCounter numberOfDuplicateValidGetParameterValueContentsToReturn;
        AtomicCounter numberOfUniqueInvalidGetParameterValueContentsToReturn;
        AtomicCounter numberOfDuplicateInvalidGetParameterValueContentsToReturn;
        GetParameterValueContents returnedGetParameterValueContents;
    };
}

#endif // SENSORGATEWAY_REQUESTRESPONSESPECIALIZEDSERVERCOMMUNICATIONSTRATEGYMOCK_HPP
