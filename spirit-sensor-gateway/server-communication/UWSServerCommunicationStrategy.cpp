#include "UWSServerCommunicationStrategy.h"

using ServerCommunication::UWSServerCommunicationStrategy;


UWSServerCommunicationStrategy::UWSServerCommunicationStrategy() {
}

UWSServerCommunicationStrategy::~UWSServerCommunicationStrategy() {

}

void UWSServerCommunicationStrategy::openConnection() {
    WebSocketPointerPromise webSocketPointerPromise;
    setWebSocketCallbacksAndConnection(serverAddress, &webSocketPointerPromise);
    auto webSocketConnectionThread = JoinableThread(&startWebSocket, &hub);
    auto futureWebSocket = webSocketPointerPromise.get_future();
    futureWebSocket.wait();
    webSocket = futureWebSocket.get();
}

void UWSServerCommunicationStrategy::closeConnection() {

}

void UWSServerCommunicationStrategy::sendMessage(MESSAGE&& message) {
    auto convertedMessage = JsonConverter::convertFrameToJsonString(std::forward<MESSAGE>(message));
    webSocket->send(convertedMessage.c_str(), convertedMessage.size(), uWS::OpCode::BINARY);
}

void UWSServerCommunicationStrategy::start() {

}

void
UWSServerCommunicationStrategy::setConnectionCallback(Hub* hub, WebSocketPointerPromise* webSocketPointerPromise) {
    hub->onConnection([&webSocketPointerPromise](WebSocket* ws, uWS::HttpRequest req) {
        std::cout << "Connected!" << std::endl;
        webSocketPointerPromise->set_value(ws);
    });

}


void UWSServerCommunicationStrategy::startWebSocket(Hub* hub) {
    hub->run();
}


void UWSServerCommunicationStrategy::setWebSocketCallbacksAndConnection(std::string serverAddress,
                                                                        WebSocketPointerPromise* webSocketPointerPromise) {
    setErrorCallback(&hub);
    setConnectionCallback(&hub, webSocketPointerPromise);
    setDisconnectionCallback(&hub);
    hub.connect(serverAddress);
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

void ServerCommunication::UWSServerCommunicationStrategy::setOnMessageCallBack() {
    hub.onMessage([](WebSocket* ws, char* message, size_t length, uWS::OpCode opCode) {
        std::cout << std::string(message, length) << std::endl;
        ws->send(message, length, opCode);
    });
}
