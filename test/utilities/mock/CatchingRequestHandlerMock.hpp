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

#ifndef SENSORGATEWAY_CATCHINGREQUESTHANDLERMOCK_HPP
#define SENSORGATEWAY_CATCHINGREQUESTHANDLERMOCK_HPP

#include "sensor-gateway/server-communication/RequestHandler.hpp"

namespace Mock {

    template<class SENSOR_STRUCTURES, class SERVER_STRUCTURES>
    class CatchingRequestHandlerMock final
            : public SensorAccessLinkElement::RequestHandler<SENSOR_STRUCTURES, SERVER_STRUCTURES> {
    protected:
        using super = SensorAccessLinkElement::RequestHandler<SENSOR_STRUCTURES, SERVER_STRUCTURES>;

        using GetParameterValueRequest = typename super::GetParameterValueRequest;
        using GetParameterValueRequests = std::list<GetParameterValueRequest>;

    public:

        explicit CatchingRequestHandlerMock() :
                super(nullptr, nullptr),
                expectedNumberOfGetParameterValueRequest(0),
                actualNumberOfGetParameterValueRequest(0),
                receivedExpectedNumberOfGetParameterValueRequest(false) {}

        ~CatchingRequestHandlerMock() noexcept override = default;

        bool hasReceivedExpectedNumberOfGetParameterValueRequest() const {
            return receivedExpectedNumberOfGetParameterValueRequest.load();
        }

        void handleGetParameterValueRequest(GetParameterValueRequest&& getParameterValueRequest) override {
            ++actualNumberOfGetParameterValueRequest;
            receivedGetParameterValueRequests.emplace_back(std::forward<GetParameterValueRequest>(getParameterValueRequest));
            if (actualNumberOfGetParameterValueRequest.load() == expectedNumberOfGetParameterValueRequest) {
                receivedExpectedNumberOfGetParameterValueRequest.store(true);
                receivedExpectedNumberOfGetParameterValueRequestPromise.set_value(true);
            }
        }

        void waitForExpectedNumberOfGetParameterValueRequest() {
            if (!hasReceivedExpectedNumberOfGetParameterValueRequest()) {
                receivedExpectedNumberOfGetParameterValueRequestPromise.get_future().wait();
            }
        }

        void setExpectedNumberOfGetParameterValueRequest(uint16_t newExpectedValue) {
            expectedNumberOfGetParameterValueRequest = newExpectedValue;
        }

        GetParameterValueRequests const& getReceivedGetParameterValueRequests() const {
            return receivedGetParameterValueRequests;
        }

    private:

        uint16_t expectedNumberOfGetParameterValueRequest;
        AtomicCounter actualNumberOfGetParameterValueRequest;
        mutable BooleanPromise receivedExpectedNumberOfGetParameterValueRequestPromise;
        AtomicFlag receivedExpectedNumberOfGetParameterValueRequest;

        GetParameterValueRequests receivedGetParameterValueRequests;

    };
}

#endif //SENSORGATEWAY_CATCHINGREQUESTHANDLERMOCK_HPP
