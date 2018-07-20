#include "UWSServerCommunicationStrategy.h"

using ServerCommunication::UWSServerCommunicationStrategy;


UWSServerCommunicationStrategy::UWSServerCommunicationStrategy() :
        webSocketConnectionThread(JoinableThread(doNothing)) {
    webSocketConnectionThread.exitSafely();
}

UWSServerCommunicationStrategy::~UWSServerCommunicationStrategy() {
    webSocketConnectionThread.exitSafely();
}

void UWSServerCommunicationStrategy::openConnection(std::string const& serverAddress) {
    WebSocketPointerPromise webSocketPointerPromise;
    webSocketConnectionThread = JoinableThread(&startWebSocket, this, &webSocketPointerPromise, serverAddress);
    auto futureWebSocket = webSocketPointerPromise.get_future();
    futureWebSocket.wait();
    webSocket = futureWebSocket.get();
}

void UWSServerCommunicationStrategy::closeConnection() {
    webSocket->close(CLOSE_CODE::NORMAL_CLOSURE, CLOSE_MESSAGE.c_str(), CLOSE_MESSAGE.size());
    webSocketConnectionThread.join();
}

void UWSServerCommunicationStrategy::sendMessage(MESSAGE&& message) {
    auto convertedMessage = JsonConverter::convertFrameToJsonString(std::forward<MESSAGE>(message));
    webSocket->send(convertedMessage.c_str(), convertedMessage.size(), uWS::OpCode::TEXT);
}

void UWSServerCommunicationStrategy::startWebSocket(UWSServerCommunicationStrategy* context,
                                                    WebSocketPointerPromise* webSocketPointerPromise,
                                                    std::string const& serverAddress) {
    Hub* hub = &(context->hub);
    hub->onConnection([&webSocketPointerPromise](WebSocket* ws, uWS::HttpRequest req) {
        // TODO: Place this in a logger
//        std::cout << "Connected!" << std::endl;
        webSocketPointerPromise->set_value(ws);
    });

    hub->onMessage([](WebSocket* ws, char* message, size_t length, uWS::OpCode opCode) {
        // TODO: Place this in a logger
//        std::cout << std::string(message, length) << std::endl;
        ws->send(message, length, opCode);
    });

    hub->onDisconnection([&hub](WebSocket* ws, int code, char* message, size_t length) {
        // TODO: Place this in a logger
//        std::cout << "Client got disconnected with data: " << ws->getUserData() << ", code: " << code
//                  << ", message: <" << std::string(message, length) << ">" << std::endl;
    });

    hub->onError([](void* user) {
        // TODO: Place this in a logger
//        std::cout << "WebSocket Connection Error" << std::endl;
    });

    hub->connect(serverAddress);
    context->hub.run();
}
