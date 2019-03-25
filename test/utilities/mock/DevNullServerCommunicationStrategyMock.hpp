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

#ifndef SENSORGATEWAY_DEVNULLSERVERCOMMUNICATIONSTRATEGYMOCK_HPP
#define SENSORGATEWAY_DEVNULLSERVERCOMMUNICATIONSTRATEGYMOCK_HPP

#include "test/utilities/data-model/DataModelFixture.h"

#include "sensor-gateway/server-communication/ServerCommunicationStrategy.hpp"

namespace Mock {

    template<typename T>
    class DevNullServerCommunicationStrategyMock final :
            public ServerCommunication::ServerCommunicationStrategy<T> {

    protected:

        using super = ServerCommunication::ServerCommunicationStrategy<T>;

        using typename super::Message;
        using typename super::RawData;
        using GetParameterValueContents = typename super::GetParameterValueContentBuffer::Contents;
        using typename super::AllParameterMetadataResponse;
        using typename super::UnsignedIntegerParameterResponse;
        using typename super::SignedIntegerParameterResponse;
        using typename super::RealNumberParameterResponse;
        using typename super::BooleanParameterResponse;
        using typename super::ParameterErrorResponse;
        using typename super::SuccessMessageResponse;
        using typename super::ErrorMessageResponse;

    public:

        DevNullServerCommunicationStrategyMock() = default;

        ~DevNullServerCommunicationStrategyMock() noexcept = default;

        void openConnection(std::string const& serverAddress) {
        }

        GetParameterValueContents fetchGetParameterValueContents() {
            GetParameterValueContents getParameterValueContents;
            return getParameterValueContents;
        }

        void sendMessage(Message&& message) {
        }

        void sendRawData(RawData&& rawData) {
        }

        void sendResponse(AllParameterMetadataResponse&& allParameterMetadataResponse) {
        }

        void sendResponse(UnsignedIntegerParameterResponse&& unsignedIntegerParameterResponse) {
        }

        void sendResponse(SignedIntegerParameterResponse&& signedIntegerParameterResponse) {
        }

        void sendResponse(RealNumberParameterResponse&& realNumberParameterResponse) {
        }

        void sendResponse(BooleanParameterResponse&& booleanParameterResponse) {
        }

        void sendResponse(ParameterErrorResponse&& parameterErrorResponse) {
        }

        void sendResponse(SuccessMessageResponse&& successMessageResponse) {
        }

        void sendResponse(ErrorMessageResponse&& errorMessageResponse) {
        }

        void closeConnection() {
        }
    };
}

#endif //SENSORGATEWAY_DEVNULLSERVERCOMMUNICATIONSTRATEGYMOCK_HPP
