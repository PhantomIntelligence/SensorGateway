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

        using SetUnsignedIntegerParameterValueRequest = typename super::SetUnsignedIntegerParameterValueRequest;
        using SetUnsignedIntegerParameterValueRequests = std::list<SetUnsignedIntegerParameterValueRequest>;

        using SetSignedIntegerParameterValueRequest = typename super::SetSignedIntegerParameterValueRequest;
        using SetSignedIntegerParameterValueRequests = std::list<SetSignedIntegerParameterValueRequest>;

        using SetRealNumberParameterValueRequest = typename super::SetRealNumberParameterValueRequest;
        using SetRealNumberParameterValueRequests = std::list<SetRealNumberParameterValueRequest>;

        using SetBooleanParameterValueRequest = typename super::SetBooleanParameterValueRequest;
        using SetBooleanParameterValueRequests = std::list<SetBooleanParameterValueRequest>;


    public:

        explicit CatchingRequestHandlerMock() :
                super(nullptr, nullptr),
                expectedNumberOfGetParameterValueRequest(0),
                actualNumberOfGetParameterValueRequest(0),
                receivedExpectedNumberOfGetAllParameterNamesRequest(false),

                expectedNumberOfGetAllParameterNamesRequests(0),
                actualNumberOfGetAllParameterNamesRequests(0),
                receivedExpectedNumberOfGetParameterValueRequest(false),

                expectedNumberOfSetUnsignedIntegerParameterValueRequest(0),
                actualNumberOfSetUnsignedIntegerParameterValueRequest(0),
                receivedExpectedNumberOfSetUnsignedIntegerParameterValueRequest(false),

                expectedNumberOfSetSignedIntegerParameterValueRequest(0),
                actualNumberOfSetSignedIntegerParameterValueRequest(0),
                receivedExpectedNumberOfSetSignedIntegerParameterValueRequest(false),

                expectedNumberOfSetRealNumberParameterValueRequest(0),
                actualNumberOfSetRealNumberParameterValueRequest(0),
                receivedExpectedNumberOfSetRealNumberParameterValueRequest(false),

                expectedNumberOfSetBooleanParameterValueRequest(0),
                actualNumberOfSetBooleanParameterValueRequest(0),
                receivedExpectedNumberOfSetBooleanParameterValueRequest(false),

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

        bool hasReceivedExpectedNumberOfSetUnsignedIntegerParameterValueRequest() const {
            return receivedExpectedNumberOfSetUnsignedIntegerParameterValueRequest.load();
        }

        bool hasReceivedExpectedNumberOfSetSignedIntegerParameterValueRequest() const {
            return receivedExpectedNumberOfSetSignedIntegerParameterValueRequest.load();
        }

        bool hasReceivedExpectedNumberOfSetRealNumberParameterValueRequest() const {
            return receivedExpectedNumberOfSetRealNumberParameterValueRequest.load();
        }

        bool hasReceivedExpectedNumberOfSetBooleanParameterValueRequest() const {
            return receivedExpectedNumberOfSetBooleanParameterValueRequest.load();
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

        void handleSetUnsignedIntegerParameterValueRequest(
                SetUnsignedIntegerParameterValueRequest&& getParameterValueRequest) override {
            ++actualNumberOfSetUnsignedIntegerParameterValueRequest;
            receivedSetUnsignedIntegerParameterValueRequests.emplace_back(
                    std::forward<SetUnsignedIntegerParameterValueRequest>(getParameterValueRequest));
            if (actualNumberOfSetUnsignedIntegerParameterValueRequest.load() ==
                expectedNumberOfSetUnsignedIntegerParameterValueRequest) {
                receivedExpectedNumberOfSetUnsignedIntegerParameterValueRequest.store(true);
                receivedExpectedNumberOfSetUnsignedIntegerParameterValueRequestPromise.set_value(true);
            }
        }

        void handleSetSignedIntegerParameterValueRequest(
                SetSignedIntegerParameterValueRequest&& setSignedIntegerParameterValueRequest) override {
            ++actualNumberOfSetSignedIntegerParameterValueRequest;
            receivedSetSignedIntegerParameterValueRequests.emplace_back(
                    std::forward<SetSignedIntegerParameterValueRequest>(setSignedIntegerParameterValueRequest));
            if (actualNumberOfSetSignedIntegerParameterValueRequest.load() ==
                expectedNumberOfSetSignedIntegerParameterValueRequest) {
                receivedExpectedNumberOfSetSignedIntegerParameterValueRequest.store(true);
                receivedExpectedNumberOfSetSignedIntegerParameterValueRequestPromise.set_value(true);
            }
        }

        void handleSetRealNumberParameterValueRequest(
                SetRealNumberParameterValueRequest&& setRealNumberParameterValueRequest) override {
            ++actualNumberOfSetRealNumberParameterValueRequest;
            receivedSetRealNumberParameterValueRequests.emplace_back(
                    std::forward<SetRealNumberParameterValueRequest>(setRealNumberParameterValueRequest));
            if (actualNumberOfSetRealNumberParameterValueRequest.load() ==
                expectedNumberOfSetRealNumberParameterValueRequest) {
                receivedExpectedNumberOfSetRealNumberParameterValueRequest.store(true);
                receivedExpectedNumberOfSetRealNumberParameterValueRequestPromise.set_value(true);
            }
        }

        void
        handleSetBooleanParameterValueRequest(SetBooleanParameterValueRequest&& getParameterValueRequest) override {
            ++actualNumberOfSetBooleanParameterValueRequest;
            receivedSetBooleanParameterValueRequests.emplace_back(
                    std::forward<SetBooleanParameterValueRequest>(getParameterValueRequest));
            if (actualNumberOfSetBooleanParameterValueRequest.load() ==
                expectedNumberOfSetBooleanParameterValueRequest) {
                receivedExpectedNumberOfSetBooleanParameterValueRequest.store(true);
                receivedExpectedNumberOfSetBooleanParameterValueRequestPromise.set_value(true);
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

        void waitForExpectedNumberOfSetUnsignedIntegerParameterValueRequest() {
            if (!hasReceivedExpectedNumberOfSetUnsignedIntegerParameterValueRequest()) {
                receivedExpectedNumberOfSetUnsignedIntegerParameterValueRequestPromise.get_future().wait();
            }
        }

        void waitForExpectedNumberOfSetSignedIntegerParameterValueRequest() {
            if (!hasReceivedExpectedNumberOfSetSignedIntegerParameterValueRequest()) {
                receivedExpectedNumberOfSetSignedIntegerParameterValueRequestPromise.get_future().wait();
            }
        }

        void waitForExpectedNumberOfSetRealNumberParameterValueRequest() {
            if (!hasReceivedExpectedNumberOfSetRealNumberParameterValueRequest()) {
                receivedExpectedNumberOfSetRealNumberParameterValueRequestPromise.get_future().wait();
            }
        }

        void waitForExpectedNumberOfSetBooleanParameterValueRequest() {
            if (!hasReceivedExpectedNumberOfSetBooleanParameterValueRequest()) {
                receivedExpectedNumberOfSetBooleanParameterValueRequestPromise.get_future().wait();
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

        void setExpectedNumberOfSetUnsignedIntegerParameterValueRequest(uint16_t newExpectedValue) {
            if (newExpectedValue == 0) {
                receivedExpectedNumberOfSetUnsignedIntegerParameterValueRequest.store(true);
            }
            expectedNumberOfSetUnsignedIntegerParameterValueRequest = newExpectedValue;
        }

        void setExpectedNumberOfSetSignedIntegerParameterValueRequest(uint16_t newExpectedValue) {
            if (newExpectedValue == 0) {
                receivedExpectedNumberOfSetSignedIntegerParameterValueRequest.store(true);
            }
            expectedNumberOfSetSignedIntegerParameterValueRequest = newExpectedValue;
        }

        void setExpectedNumberOfSetRealNumberParameterValueRequest(uint16_t newExpectedValue) {
            if (newExpectedValue == 0) {
                receivedExpectedNumberOfSetRealNumberParameterValueRequest.store(true);
            }
            expectedNumberOfSetRealNumberParameterValueRequest = newExpectedValue;
        }

        void setExpectedNumberOfSetBooleanParameterValueRequest(uint16_t newExpectedValue) {
            if (newExpectedValue == 0) {
                receivedExpectedNumberOfSetBooleanParameterValueRequest.store(true);
            }
            expectedNumberOfSetBooleanParameterValueRequest = newExpectedValue;
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

        uint16_t expectedNumberOfSetUnsignedIntegerParameterValueRequest;
        AtomicCounter actualNumberOfSetUnsignedIntegerParameterValueRequest;
        mutable BooleanPromise receivedExpectedNumberOfSetUnsignedIntegerParameterValueRequestPromise;
        AtomicFlag receivedExpectedNumberOfSetUnsignedIntegerParameterValueRequest;
        SetUnsignedIntegerParameterValueRequests receivedSetUnsignedIntegerParameterValueRequests;

        uint16_t expectedNumberOfSetSignedIntegerParameterValueRequest;
        AtomicCounter actualNumberOfSetSignedIntegerParameterValueRequest;
        mutable BooleanPromise receivedExpectedNumberOfSetSignedIntegerParameterValueRequestPromise;
        AtomicFlag receivedExpectedNumberOfSetSignedIntegerParameterValueRequest;
        SetSignedIntegerParameterValueRequests receivedSetSignedIntegerParameterValueRequests;

        uint16_t expectedNumberOfSetRealNumberParameterValueRequest;
        AtomicCounter actualNumberOfSetRealNumberParameterValueRequest;
        mutable BooleanPromise receivedExpectedNumberOfSetRealNumberParameterValueRequestPromise;
        AtomicFlag receivedExpectedNumberOfSetRealNumberParameterValueRequest;
        SetRealNumberParameterValueRequests receivedSetRealNumberParameterValueRequests;

        uint16_t expectedNumberOfSetBooleanParameterValueRequest;
        AtomicCounter actualNumberOfSetBooleanParameterValueRequest;
        mutable BooleanPromise receivedExpectedNumberOfSetBooleanParameterValueRequestPromise;
        AtomicFlag receivedExpectedNumberOfSetBooleanParameterValueRequest;
        SetBooleanParameterValueRequests receivedSetBooleanParameterValueRequests;

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
