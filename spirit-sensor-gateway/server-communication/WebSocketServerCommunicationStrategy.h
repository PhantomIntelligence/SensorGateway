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
#ifndef SPIRITSENSORGATEWAY_WEBSOCKETSERVERCOMMUNICATIONSTRATEGY_H
#define SPIRITSENSORGATEWAY_WEBSOCKETSERVERCOMMUNICATIONSTRATEGY_H

#include <websocketpp/config/asio_no_tls_client.hpp>
#include "ServerCommunicationProtocolStrategy.hpp"
#include "spirit-sensor-gateway/domain/Frame.h"


namespace ServerCommunication {
    using Client = websocketpp::client<websocketpp::config::asio_client>;
    using ConnectionHandle = websocketpp::connection_hdl;

    struct ConnectionMetadata {
        int id;
        ConnectionHandle handle;
        std::string status;
        std::string URI;
        std::string server;
        std::string errorReason;
    };

    class WebSocketServerCommunicationStrategy : public ServerCommunicationProtocolStrategy<SpiritProtocol::Frame> {

    public:

        WebSocketServerCommunicationStrategy();

        ~WebSocketServerCommunicationStrategy();

        void openConnection() override;

        void closeConnection() override;

        void sendMessage() override;

        void start();

    private:

        void initializeClientCallbacks();

        ConnectionHandle connectionHandle;

        ConnectionMetadata connectionMetadata;
    };
}


#endif //SPIRITSENSORGATEWAY_WEBSOCKETSERVERCOMMUNICATIONSTRATEGY_H
