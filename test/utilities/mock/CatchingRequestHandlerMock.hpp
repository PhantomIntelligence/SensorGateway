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
                receivedExpectedNumberOfGetAllParameterNamesRequest(false),

                expectedNumberOfGetAllParameterNamesRequests(0),
                actualNumberOfGetAllParameterNamesRequests(0),
                receivedExpectedNumberOfGetParameterValueRequest(false),

                expectedNumberOfCalibrationRequests(0),
                actualNumberOfCalibrationRequests(0),
                receivedExpectedNumberOfCalibrationRequest(false),

                expectedNumberOfClearCalibrationRequests(0),
                actualNumberOfClearCalibrationRequests(0),
                receivedExpectedNumberOfClearCalibrationRequest(false) {}

        ~CatchingRequestHandlerMock() noexcept override = default;

        bool hasReceivedExpectedNumberOfGetAllParameterNamesRequest() const {
            return receivedExpectedNumberOfGetAllParameterNamesRequest.load();
        }

        bool hasReceivedExpectedNumberOfGetParameterValueRequest() const {
            return receivedExpectedNumberOfGetParameterValueRequest.load();
        }

        bool hasReceivedExpectedNumberOfCalibrationRequest() const {
            return receivedExpectedNumberOfCalibrationRequest.load();
        }

        bool hasReceivedExpectedNumberOfClearCalibrationRequest() const {
            return receivedExpectedNumberOfClearCalibrationRequest.load();
        }

        void handleGetAllParameterNamesRequest() override {
            ++actualNumberOfGetAllParameterNamesRequests;
            if (actualNumberOfGetAllParameterNamesRequests.load() == expectedNumberOfGetAllParameterNamesRequests) {
                receivedExpectedNumberOfGetAllParameterNamesRequest.store(true);
                receivedExpectedNumberOfGetAllParameterNamesRequestPromise.set_value(true);
            }
            if (actualNumberOfGetAllParameterNamesRequests.load() > expectedNumberOfGetAllParameterNamesRequests) {
                receivedExpectedNumberOfGetAllParameterNamesRequest.store(false);
            }
        }

        void handleGetParameterValueRequest(GetParameterValueRequest&& getParameterValueRequest) override {
            ++actualNumberOfGetParameterValueRequest;
            receivedGetParameterValueRequests.emplace_back(
                    std::forward<GetParameterValueRequest>(getParameterValueRequest));
            if (actualNumberOfGetParameterValueRequest.load() == expectedNumberOfGetParameterValueRequest) {
                receivedExpectedNumberOfGetParameterValueRequest.store(true);
                receivedExpectedNumberOfGetParameterValueRequestPromise.set_value(true);
            }
        }

        void handleCalibrationRequest() override {
            ++actualNumberOfCalibrationRequests;
            if (actualNumberOfCalibrationRequests.load() == expectedNumberOfCalibrationRequests) {
                receivedExpectedNumberOfCalibrationRequest.store(true);
                receivedExpectedNumberOfCalibrationRequestPromise.set_value(true);
            }
            if (actualNumberOfCalibrationRequests.load() > expectedNumberOfCalibrationRequests) {
                receivedExpectedNumberOfCalibrationRequest.store(false);
            }
        }

        void handleClearCalibrationRequest() override {
            ++actualNumberOfClearCalibrationRequests;
            if (actualNumberOfClearCalibrationRequests.load() == expectedNumberOfClearCalibrationRequests) {
                receivedExpectedNumberOfClearCalibrationRequest.store(true);
                receivedExpectedNumberOfClearCalibrationRequestPromise.set_value(true);
            }
            if (actualNumberOfClearCalibrationRequests.load() > expectedNumberOfClearCalibrationRequests) {
                receivedExpectedNumberOfClearCalibrationRequest.store(false);
            }
        }

        void waitForExpectedNumberOfGetAllParameterNamesRequest() {
            if (!hasReceivedExpectedNumberOfGetAllParameterNamesRequest()) {
                receivedExpectedNumberOfGetAllParameterNamesRequestPromise.get_future().wait();
            }
        }

        void waitForExpectedNumberOfGetParameterValueRequest() {
            if (!hasReceivedExpectedNumberOfGetParameterValueRequest()) {
                receivedExpectedNumberOfGetParameterValueRequestPromise.get_future().wait();
            }
        }

        void waitForExpectedNumberOfCalibrationRequest() {
            if (!hasReceivedExpectedNumberOfCalibrationRequest()) {
                receivedExpectedNumberOfCalibrationRequestPromise.get_future().wait();
            }
        }

        void waitForExpectedNumberOfClearCalibrationRequest() {
            if (!hasReceivedExpectedNumberOfClearCalibrationRequest()) {
                receivedExpectedNumberOfClearCalibrationRequestPromise.get_future().wait();
            }
        }

        void setExpectedNumberOfGetAllParameterNamesRequest(uint16_t newExpectedValue) {
            if (newExpectedValue == 0) {
                receivedExpectedNumberOfGetAllParameterNamesRequest.store(true);
            }
            expectedNumberOfGetAllParameterNamesRequests = newExpectedValue;
        }

        void setExpectedNumberOfGetParameterValueRequest(uint16_t newExpectedValue) {
            if (newExpectedValue == 0) {
                receivedExpectedNumberOfGetParameterValueRequest.store(true);
            }
            expectedNumberOfGetParameterValueRequest = newExpectedValue;
        }

        void setExpectedNumberOfCalibrationRequest(uint16_t newExpectedValue) {
            if (newExpectedValue == 0) {
                receivedExpectedNumberOfCalibrationRequest.store(true);
            }
            expectedNumberOfCalibrationRequests = newExpectedValue;
        }

        void setExpectedNumberOfClearCalibrationRequest(uint16_t newExpectedValue) {
            if (newExpectedValue == 0) {
                receivedExpectedNumberOfClearCalibrationRequest.store(true);
            }
            expectedNumberOfClearCalibrationRequests = newExpectedValue;
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

        uint16_t expectedNumberOfGetAllParameterNamesRequests;
        AtomicCounter actualNumberOfGetAllParameterNamesRequests;
        mutable BooleanPromise receivedExpectedNumberOfGetAllParameterNamesRequestPromise;
        AtomicFlag receivedExpectedNumberOfGetAllParameterNamesRequest;

        uint16_t expectedNumberOfCalibrationRequests;
        AtomicCounter actualNumberOfCalibrationRequests;
        mutable BooleanPromise receivedExpectedNumberOfCalibrationRequestPromise;
        AtomicFlag receivedExpectedNumberOfCalibrationRequest;

        uint16_t expectedNumberOfClearCalibrationRequests;
        AtomicCounter actualNumberOfClearCalibrationRequests;
        mutable BooleanPromise receivedExpectedNumberOfClearCalibrationRequestPromise;
        AtomicFlag receivedExpectedNumberOfClearCalibrationRequest;
    };
}

#endif //SENSORGATEWAY_CATCHINGREQUESTHANDLERMOCK_HPP
