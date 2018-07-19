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
    webSocketConnectionThread = JoinableThread(&startWebSocket, this, &webSocketPointerPromise,serverAddress);
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

void UWSServerCommunicationStrategy::startWebSocket(UWSServerCommunicationStrategy* context,
                                                   WebSocketPointerPromise* webSocketPointerPromise,
                                                   std::string const& serverAddress) {
    Hub* hub = &(context->hub);
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
    context->hub.run();
}
