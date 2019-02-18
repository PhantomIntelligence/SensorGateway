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

#ifndef SERVERGATEWAY_REQUESTRESPONSESPECIALIZEDSERVERCOMMUNICATIONSTRATEGYMOCK_HPP
#define SERVERGATEWAY_REQUESTRESPONSESPECIALIZEDSERVERCOMMUNICATIONSTRATEGYMOCK_HPP

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

        using AvailableParameters = typename T::Parameters;
        using AllFakeParameters = Sensor::FakeParameter::AllFakeParameters;

        using ParameterNames = std::list<std::string>;

        static auto availableParameterNames() noexcept {
            AvailableParameters availableParameters;
            ParameterNames validParameters;
            auto validNames = availableParameters.getNames();

            ParameterNames validParameterNames;
            std::string name;
            for(auto validNameIndex = 0u; validNameIndex < AvailableParameters::NUMBER_OF_AVAILABLE_PARAMETERS; ++validNameIndex) {
                name = validNames[validNameIndex];
                validParameterNames.push_front(name);
            }

            return validParameterNames;
        }

        static auto nonAvailableParameterNames() noexcept {
            // Ensure we have AT LEAST 1 invalid parameter name
            AvailableParameters availableParameters;
            AllFakeParameters allFakeParameters;
            auto allNames = allFakeParameters.getNames();

            ParameterNames invalidParameterNames = {TestFunctions::DataTestUtil::createRandomStringOfLength(100)};
            for (uint16_t nameIndex = 0; nameIndex < AllFakeParameters::NUMBER_OF_AVAILABLE_PARAMETERS; ++nameIndex) {
                auto parameterName = allNames[nameIndex];
                if (!availableParameters.isAvailable(parameterName)) {
                    invalidParameterNames.push_front(parameterName);
                }
            }
            return invalidParameterNames;
        }

    public:
        RequestResponseSpecializedServerCommunicationStrategyMock() :
                validParameterNames(availableParameterNames()),
                invalidParameterNames(nonAvailableParameterNames()),
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
            acknowledgeGetParameterValueContentsHasBeenCalled();

            GetParameterValueContents getParameterValueContents;

            auto totalContentCounter = 0u;
            auto uniqueValidRequestCounter = 0u;
            std::string name;
            while (uniqueValidRequestCounter != numberOfUniqueValidGetParameterValueContentsToReturn) {
                ++uniqueValidRequestCounter;

                name = validParameterNames.front();
                getParameterValueContents[totalContentCounter] = GetParameterValueContent(name);
                validParameterNames.pop_front();

                totalContentCounter = uniqueValidRequestCounter;
            }
// TODO:
//            uint_16_t duplicateValidRequestCounter = 0;
//            while (duplicateValidRequestCounter != numberOfDuplicateValidGetParameterValueContentsToReturn) {
//                ++duplicateValidRequestCounter;
//            }
//            uint_16_t uniqueInvalidRequestCounter = 0;
//            while (uniqueInvalidRequestCounter != numberOfUniqueInvalidGetParameterValueContentsToReturn) {
//                ++uniqueInvalidRequestCounter;
//            }
//            uint_16_t duplicateInvalidRequestCounter = 0;
//            while (duplicateInvalidRequestCounter != numberOfDuplicateInvalidGetParameterValueContentsToReturn) {
//                ++duplicateInvalidRequestCounter;
//            }
            std::copy(getParameterValueContents.cbegin(), getParameterValueContents.cend(),
                      returnedGetParameterValueContents.begin());

            // WARNING! This mock implementation of sendGetParameterValueResponse needs to be slowed down because the way gtest works. DO NOT REMOVE.
            yield();

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

        uint16_t numberOfUniqueValidGetParameterValueContentsToReturn;
        uint16_t numberOfDuplicateValidGetParameterValueContentsToReturn;
        uint16_t numberOfUniqueInvalidGetParameterValueContentsToReturn;
        uint16_t numberOfDuplicateInvalidGetParameterValueContentsToReturn;
        GetParameterValueContents returnedGetParameterValueContents;
    };
}

#endif // SERVERGATEWAY_REQUESTRESPONSESPECIALIZEDSERVERCOMMUNICATIONSTRATEGYMOCK_HPP
