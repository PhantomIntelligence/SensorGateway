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
                sendGetParameterValueResponseCalled(false)
        // SetParameterValue
        {}

        ~RequestResponseSpecializedServerCommunicationStrategyMock() noexcept override = default;

        void openConnection(std::string const& serverAddress) override {
        }

        GetParameterValueContents fetchGetParameterValueContents() override {
            LockGuard guard(getParameterValueMutex);
            acknowledgeGetParameterValueContentsHasBeenCalled();

            GetParameterValueContents getParameterValueContents;

            std::string name;

            auto totalNumberOfRequestToReturn = numberOfUniqueValidGetParameterValueContentsToReturn +
                                                numberOfDuplicateValidGetParameterValueContentsToReturn +
                                                numberOfUniqueInvalidGetParameterValueContentsToReturn +
                                                numberOfDuplicateInvalidGetParameterValueContentsToReturn;
            assert(totalNumberOfRequestToReturn <= T::MAX_NUMBER_OF_CONCURRENT_REQUEST_OF_ONE_KIND);

            auto returnedContentStartIterator = getParameterValueContents.begin();

            // Valid unique requests
            std::copy_n(validParameterNames.begin(), numberOfUniqueValidGetParameterValueContentsToReturn, returnedContentStartIterator);

            for (auto counter = 0u; counter < numberOfUniqueValidGetParameterValueContentsToReturn; ++counter) {
                returnedContentStartIterator++;
            }

            // Valid duplicate requests
            uint16_t duplicateValidRequestCounter = 0;
            while (duplicateValidRequestCounter != numberOfDuplicateValidGetParameterValueContentsToReturn) {
                std::copy_n(validParameterNames.begin(), 1, returnedContentStartIterator);
                returnedContentStartIterator++;
                duplicateValidRequestCounter++;
            }

            // Invalid unique requests
            std::copy_n(invalidParameterNames.begin(), numberOfUniqueInvalidGetParameterValueContentsToReturn, returnedContentStartIterator);

            for (auto counter = 0u; counter < numberOfUniqueInvalidGetParameterValueContentsToReturn; ++counter) {
                returnedContentStartIterator++;
            }

            // Invalid duplicate requests
            uint16_t duplicateInvalidRequestCounter = 0;
            while (duplicateInvalidRequestCounter != numberOfDuplicateInvalidGetParameterValueContentsToReturn) {
                std::copy_n(invalidParameterNames.begin(), 1, returnedContentStartIterator);
                returnedContentStartIterator++;
                duplicateInvalidRequestCounter++;
            }


            // Backup for test validation
            std::copy_n(getParameterValueContents.cbegin(), totalNumberOfRequestToReturn, returnedGetParameterValueContents.begin());

            return getParameterValueContents;
        };

// TODO :
//        void sendGetParameterValueResponse(Response&& response) override {
//        sendGetParameterValueResponseCalled.store(true);
//        }

        void sendMessage(typename super::Message&& message) override {
        }

        void sendRawData(typename super::RawData&& rawData) override {
        }

        void sendResponse(ErrorMessageResponse&& errorMessageResponse) override {
        }

        void closeConnection() override {
        }

        void waitUntilSendGetParameterValueResponseIsCalled() {
            if (!hasSendGetParameterValueResponseBeenCalled()) {
                sendGetParameterValueResponseCalledAcknowledgement.get_future().wait();
            }
        }

        void setNumberOfUniqueValidGetParameterValueContentsToReturn(uint16_t newNumberToReturn) {
            numberOfUniqueValidGetParameterValueContentsToReturn = newNumberToReturn;
        }

        void setNumberOfDuplicateValidGetParameterValueContentsToReturn(uint16_t newNumberToReturn) {
            numberOfDuplicateValidGetParameterValueContentsToReturn = newNumberToReturn;
        }

        void setNumberOfUniqueInvalidGetParameterValueContentsToReturn(uint16_t newNumberToReturn) {
            numberOfUniqueInvalidGetParameterValueContentsToReturn = newNumberToReturn;
        }

        void setNumberOfDuplicateInvalidGetParameterValueContentsToReturn(uint16_t newNumberToReturn) {
            numberOfDuplicateInvalidGetParameterValueContentsToReturn = newNumberToReturn;
        }

        GetParameterValueContents const& getReturnedGetParameterValueRequest() noexcept {
            LockGuard guard(getParameterValueMutex);
            return returnedGetParameterValueContents;
        }

    private:

        void acknowledgeGetParameterValueContentsHasBeenCalled() {
            LockGuard guard(sendGetParameterValueResponseAckMutex);
            if (!hasSendGetParameterValueResponseBeenCalled()) {
                sendGetParameterValueResponseCalled.store(true);
                sendGetParameterValueResponseCalledAcknowledgement.set_value(true);
            }
        }


        bool hasSendGetParameterValueResponseBeenCalled() const {
            return sendGetParameterValueResponseCalled.load();
        }

        ParameterNames validParameterNames;
        ParameterNames invalidParameterNames;

        AtomicFlag sendGetParameterValueResponseCalled;
        Mutex sendGetParameterValueResponseAckMutex;
        mutable BooleanPromise sendGetParameterValueResponseCalledAcknowledgement;

        Mutex getParameterValueMutex;
        uint16_t numberOfUniqueValidGetParameterValueContentsToReturn;
        uint16_t numberOfDuplicateValidGetParameterValueContentsToReturn;
        uint16_t numberOfUniqueInvalidGetParameterValueContentsToReturn;
        uint16_t numberOfDuplicateInvalidGetParameterValueContentsToReturn;
        GetParameterValueContents returnedGetParameterValueContents;
    };
}

#endif // SENSORGATEWAY_REQUESTRESPONSESPECIALIZEDSERVERCOMMUNICATIONSTRATEGYMOCK_HPP
