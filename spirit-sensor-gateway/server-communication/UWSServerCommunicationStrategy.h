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
#include "ServerCommunicationStrategy.hpp"
#include "spirit-sensor-gateway/domain/Frame.h"


namespace ServerCommunication {

    std::string const SERVER_CONNECTION_ADDRESS = "ws://localhost:8080/connect-gateway";

    class UWSServerCommunicationStrategy : public ServerCommunicationStrategy<DataFlow::Frame> {

    protected:
        using super = ServerCommunicationStrategy<DataFlow::Frame>;

        using super::MESSAGE;

    public:

        UWSServerCommunicationStrategy();

        ~UWSServerCommunicationStrategy() noexcept;

        UWSServerCommunicationStrategy(UWSServerCommunicationStrategy const& other) = delete;

        UWSServerCommunicationStrategy(UWSServerCommunicationStrategy&& other) noexcept = delete;

        UWSServerCommunicationStrategy& operator=(UWSServerCommunicationStrategy const& other)& = delete;

        UWSServerCommunicationStrategy& operator=(UWSServerCommunicationStrategy&& other) noexcept= delete;

        void openConnection() override;

        void closeConnection() override;

        void sendMessage(MESSAGE&& message) override;

        void start();

        int connect(std::string const& serverAddress);

    private:

        void initializeClientCallbacks();

        struct ConnectionMetadata {
            int id;
            std::string status;
            std::string URI;
            std::string server;
            std::string errorReason;
        };

        ConnectionMetadata connectionMetadata;

        std::string serverAddrress = SERVER_CONNECTION_ADDRESS;
    };
}


#endif //SPIRITSENSORGATEWAY_UWSSERVERCOMMUNICATIONSTRATEGY_H
