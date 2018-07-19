#include "UWSServerCommunicationStrategy.h"

using ServerCommunication::UWSServerCommunicationStrategy;


UWSServerCommunicationStrategy::UWSServerCommunicationStrategy() {
}

UWSServerCommunicationStrategy::~UWSServerCommunicationStrategy() {

}

void UWSServerCommunicationStrategy::openConnection() {
    WebSocketPointerPromise webSocketPointerPromise;
    setWebSocketCallbacksAndConnection(SERVER_CONNECTION_ADDRESS, &hub, &webSocketPointerPromise);
    auto webSocketConnectionThread = JoinableThread(&startWebSocket, this);
    auto futureWebSocket = webSocketPointerPromise.get_future();
    futureWebSocket.wait();
    webSocket = futureWebSocket.get();
}

void UWSServerCommunicationStrategy::closeConnection() {

}

void UWSServerCommunicationStrategy::sendMessage(MESSAGE&& message) {
    std::cout << "MessageWillSend" << std::endl;
    auto convertedMessage = JsonConverter::convertFrameToJsonString(std::forward<MESSAGE>(message));
    std::cout << "MessageStagedToBeSent" << std::endl;
    webSocket->send(convertedMessage.c_str(), convertedMessage.size(), uWS::OpCode::BINARY);
    std::cout << "MessageSent" << std::endl;

}

void UWSServerCommunicationStrategy::start() {

}

void
UWSServerCommunicationStrategy::setConnectionCallback(Hub* hub, WebSocketPointerPromise* webSocketPointerPromise) {
    hub->onConnection([&webSocketPointerPromise](WebSocket* ws, uWS::HttpRequest req) {
        std::cout << "Connected!" << std::endl;
        std::cout << webSocketPointerPromise << std::endl;
        webSocketPointerPromise->set_value(ws);
    });

}


void UWSServerCommunicationStrategy::startWebSocket(UWSServerCommunicationStrategy* context) {
    context->hub.run();
}


void UWSServerCommunicationStrategy::setWebSocketCallbacksAndConnection(std::string serverAddress, Hub* hub,
                                                                        WebSocketPointerPromise* webSocketPointerPromise) {
//    setErrorCallback(&hub);
//    setConnectionCallback(&hub, webSocketPointerPromise);
//    setDisconnectionCallback(&hub);
//    setOnMessageCallBack(&hub);
    hub->onConnection([&webSocketPointerPromise](WebSocket* ws, uWS::HttpRequest req) {
        std::cout << "Connected!" << std::endl;
        webSocketPointerPromise->set_value(ws);
    });

    hub->onMessage([](WebSocket* ws, char* message, size_t length, uWS::OpCode opCode) {
        std::cout << std::string(message, length) << std::endl;
        ws->send(message, length, opCode);
    });


    hub->onDisconnection([&hub](WebSocket* ws, int code, char* message, size_t length) {
        std::cout << "Client got disconnected with data: " << ws->getUserData() << ", code: " << code
                  << ", message: <" << std::string(message, length) << ">" << std::endl;
        hub->getDefaultGroup<uWS::CLIENT>().close();
    });

    hub->onError([](void* user) {
        std::cout << "WebSocket Connection Error" << std::endl;
    });
    hub->connect(serverAddress);

}

void ServerCommunication::UWSServerCommunicationStrategy::setErrorCallback(Hub* hub) {
    hub->onError([](void* user) {
        std::cout << "WebSocket Connection Error" << std::endl;
    });

}

void ServerCommunication::UWSServerCommunicationStrategy::setDisconnectionCallback(Hub* hub) {
    hub->onDisconnection([&hub](WebSocket* ws, int code, char* message, size_t length) {
        std::cout << "Client got disconnected with data: " << ws->getUserData() << ", code: " << code
                  << ", message: <" << std::string(message, length) << ">" << std::endl;
        hub->getDefaultGroup<uWS::CLIENT>().close();
    });
}

void ServerCommunication::UWSServerCommunicationStrategy::setOnMessageCallBack(Hub* hub) {
    hub->onMessage([](WebSocket* ws, char* message, size_t length, uWS::OpCode opCode) {
        std::cout << std::string(message, length) << std::endl;
        ws->send(message, length, opCode);
    });
}
