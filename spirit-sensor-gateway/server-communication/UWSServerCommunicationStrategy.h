/**
	Copyright 2014-2018 Phantom Intelligence Inc.
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
#ifndef SPIRITSENSORGATEWAY_UWSSERVERCOMMUNICATIONSTRATEGY_H
#define SPIRITSENSORGATEWAY_UWSSERVERCOMMUNICATIONSTRATEGY_H

#include <uWS.h>
#include "ServerCommunicator.hpp"
#include "spirit-sensor-gateway/domain/Frame.h"
#include "JsonConverter.h"


namespace ServerCommunication {
    using Hub = uWS::Hub;
    using WebSocket = uWS::WebSocket<uWS::CLIENT>;
    using WebSocketPointerPromise = std::promise<WebSocket*>;

    namespace CLOSE_CODE {
        int const NORMAL_CLOSURE = 1000;
        int const GOING_AWAY = 1001;
        int const PROTOCOL_ERROR = 1002;
        int const UNSUPPORTED_DATA = 1003;
        int const NO_STATUS_RECEIVED = 1005;
        int const ABNORMAL_CLOSURE = 1006;
        int const INVALID_FRAME_PAYLOAD_DATA = 1007;
        int const POLICY_VIOLATION = 1008;
        int const MESSAGE_TOO_BIG = 1009;
        int const MISSING_EXTENSION = 1010;
        int const INTERNAL_ERROR = 1011;
        int const SERVICE_RESTART = 1012;
        int const TRY_AGAIN_LATER = 1013;
        int const BAD_GATEWAY = 1014;
        int const TLS_HANDSHAKE = 1015;
    }

    class UWSServerCommunicationStrategy : public ServerCommunicationStrategy<DataFlow::Frame> {

    protected:
        using super = ServerCommunicationStrategy<DataFlow::Frame>;

        using super::MESSAGE;

        std::string const CLOSE_MESSAGE = "Closing connection for client";

    public:

        UWSServerCommunicationStrategy();

        ~UWSServerCommunicationStrategy() noexcept;

        UWSServerCommunicationStrategy(UWSServerCommunicationStrategy const& other) = delete;

        UWSServerCommunicationStrategy(UWSServerCommunicationStrategy&& other) noexcept = delete;

        UWSServerCommunicationStrategy& operator=(UWSServerCommunicationStrategy const& other)& = delete;

        UWSServerCommunicationStrategy& operator=(UWSServerCommunicationStrategy&& other) noexcept= delete;

        void openConnection(std::string const& serverAddress) override;

        void closeConnection() override;

        void sendMessage(MESSAGE&& message) override;

    private:


        static void startWebSocket(UWSServerCommunicationStrategy* context,
                                   WebSocketPointerPromise* webSocketPointerPromise,
                                   std::string const& serverAddress);

        Hub hub;

        WebSocket* webSocket;

        JoinableThread webSocketConnectionThread;

    };
}

#endif //SPIRITSENSORGATEWAY_UWSSERVERCOMMUNICATIONSTRATEGY_H
