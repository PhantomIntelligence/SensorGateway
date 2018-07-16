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

#include <uWS.h>
#include "ServerCommunicationStrategy.hpp"
#include "spirit-sensor-gateway/domain/Frame.h"


namespace ServerCommunication {

    std::string const SERVER_ADDRRESS = "ws://localhost:8080/connect-gateway";

    class WebSocketServerCommunicationStrategy : public ServerCommunicationStrategy<DataFlow::Frame> {

    protected:
        using super = ServerCommunicationStrategy<DataFlow::Frame>;

        using super::MESSAGE;

    public:

        WebSocketServerCommunicationStrategy();

        ~WebSocketServerCommunicationStrategy();

        void openConnection() override;

        void closeConnection() override;

        void processMessage(MESSAGE&& message);

        void start();

        int connect(std::string const& serverAddress);

    private:


        void initializeClientCallbacks();

        void sendMessage(MESSAGE&& message) override;

        struct ConnectionMetadata {
            int id;
            std::string status;
            std::string URI;
            std::string server;
            std::string errorReason;
        };

        ConnectionMetadata connectionMetadata;

        std::string serverAddrress = SERVER_ADDRRESS;
    };
}


#endif //SPIRITSENSORGATEWAY_WEBSOCKETSERVERCOMMUNICATIONSTRATEGY_H
