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

#ifndef SENSORGATEWAY_SENSORREQUEST_HPP
#define SENSORGATEWAY_SENSORREQUEST_HPP

#include "ServerCommunicationPayloadTypes.hpp"

namespace ServerCommunication {

    namespace RequestTypes {
        namespace Names {
            using GetValueOfParameter = StringLiteral<decltype("get value of parameter "_ToString)>;
            using SetValueOfParameter = StringLiteral<decltype("set value of parameter "_ToString)>;
            using GetAllParameterMetadata = StringLiteral<decltype("get all parameters metadata "_ToString)>;
        }
    }

    template<typename N, typename PayloadType>
    class ServerRequest {

    public:

        using Payload = PayloadType;

        explicit ServerRequest(Payload const& payload) noexcept :
                ServerRequest(payload, false, false) {
        }

        explicit ServerRequest() noexcept :
                ServerRequest(ServerRequest::returnDefaultData()) {
        }

        ~ServerRequest() noexcept = default;

        ServerRequest(ServerRequest const& other) :
                ServerRequest(other.payload, other.badRequest, other.causedAnError) {
        }

        ServerRequest(ServerRequest&& other) noexcept :
                ServerRequest(std::move(other.payload), std::move(other.badRequest), std::move(other.causedAnError)) {
        }

        ServerRequest& operator=(ServerRequest const& other)& {
            ServerRequest temporary(other);
            swap(*this, temporary);
            return *this;
        }

        ServerRequest& operator=(ServerRequest&& other)& noexcept {
            swap(*this, other);
            return *this;
        }

        void swap(ServerRequest& current, ServerRequest& other) noexcept {
            std::swap(current.payload, other.payload);
            std::swap(current.badRequest, other.badRequest);
            std::swap(current.causedAnError, other.causedAnError);
        }

        bool operator==(ServerRequest const& other) const {
            auto samePayload = (payload == other.payload);
            auto sameBadness = (badRequest == other.badRequest);
            auto sameErrorCause = (causedAnError == other.causedAnError);
            bool requestsAreEqual = (samePayload &&
                                     sameErrorCause &&
                                     sameBadness);
            return requestsAreEqual;
        }

        bool operator!=(ServerRequest const& other) const {
            return !operator==(other);
        }

        void markAsBadRequest() noexcept {
            badRequest = true;
        }

        void markAsErrorCause() noexcept {
            causedAnError = true;
        }

        bool const& isBadRequest() const noexcept {
            return badRequest;
        }

        bool const& hasCausedAnError() const noexcept {
            return causedAnError;
        }

        static ServerRequest const& returnDefaultData() noexcept;

        PayloadTypes::MessagePayload const& toResponseMessagePayload() const noexcept {
            return responseMessagePayload;
        }

        std::string const& getPayloadName() const noexcept {
            return payload.getName();
        }

//        template<typename T = typename Payload::ContentType,
//                typename std::enable_if_t<!std::is_same<typename Payload::ContentType, typename Payload::Type>::value>
//        >
        auto getRequestedValue() const noexcept -> typename Payload::ContentType {
            return payload.getContentValue();
        }

        std::string payloadToString() const noexcept {
            return payload.toString();
        }

        bool isGetRequest() const noexcept {
            return std::is_same<N, RequestTypes::Names::GetValueOfParameter>::value;
        };

        bool isSetRequest() const noexcept {
            return std::is_same<N, RequestTypes::Names::SetValueOfParameter>::value;
        };

    private:
        ServerRequest(Payload const& payload, bool const& badRequest, bool const& causedAnError) noexcept :
                payload(payload), badRequest(badRequest), causedAnError(causedAnError),
                responseMessagePayload(N::toString() + getPayloadName(), isGetRequest(), isSetRequest()) {
        }

        Payload payload;
        bool badRequest;
        bool causedAnError;
        PayloadTypes::MessagePayload const responseMessagePayload;

    };

    namespace Defaults {
        using ServerCommunication::ServerRequest;

        template<typename PayloadType>
        PayloadType const DEFAULT_SERVER_REQUEST_PAYLOAD{};

        template<typename N, typename PayloadType>
        ServerRequest<N, PayloadType> const DEFAULT_SERVER_REQUEST = ServerRequest<N, PayloadType>(
                DEFAULT_SERVER_REQUEST_PAYLOAD<PayloadType>);
    }

    template<typename N, typename PayloadType>
    ServerRequest<N, PayloadType> const& ServerRequest<N, PayloadType>::returnDefaultData() noexcept {
        return Defaults::DEFAULT_SERVER_REQUEST<N, PayloadType>;
    }

    namespace RequestTypes {
        using GetParameterValue = ServerRequest<Names::GetValueOfParameter, PayloadTypes::MessagePayload>;
        using SetUnsignedIntegerParameterValue = ServerRequest<Names::SetValueOfParameter, PayloadTypes::UnsignedIntegerParameterPayload>;
        using SetSignedIntegerParameterValue = ServerRequest<Names::SetValueOfParameter, PayloadTypes::SignedIntegerParameterPayload>;
        using SetRealNumberParameterValue = ServerRequest<Names::SetValueOfParameter, PayloadTypes::RealNumberParameterPayload>;
        using SetBooleanParameterValue = ServerRequest<Names::SetValueOfParameter, PayloadTypes::BooleanParameterPayload>;

        using GetAllParameterMetadata = ServerRequest<Names::GetAllParameterMetadata, PayloadTypes::MessagePayload>;
    }

}


#endif //SENSORGATEWAY_SENSORREQUEST_HPP
