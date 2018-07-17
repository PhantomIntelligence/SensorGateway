#include "UWSServerCommunicationStrategy.h"

using ServerCommunication::UWSServerCommunicationStrategy;


UWSServerCommunicationStrategy::UWSServerCommunicationStrategy() {
    initializeClientCallbacks();
}

UWSServerCommunicationStrategy::~UWSServerCommunicationStrategy() {

}

void UWSServerCommunicationStrategy::openConnection() {
    uWS::Hub hub;


    hub.onError([](void *user) {
        switch ((long) user) {
            case 1:
                std::cout << "Client emitted error on invalid URI" << std::endl;
                break;
            case 2:
                std::cout << "Client emitted error on resolve failure" << std::endl;
                break;
            case 3:
                std::cout << "Client emitted error on connection timeout (non-SSL)" << std::endl;
                break;
            case 5:
                std::cout << "Client emitted error on connection timeout (SSL)" << std::endl;
                break;
            case 6:
                std::cout << "Client emitted error on HTTP response without upgrade (non-SSL)" << std::endl;
                break;
            case 7:
                std::cout << "Client emitted error on HTTP response without upgrade (SSL)" << std::endl;
                break;
            case 10:
                std::cout << "Client emitted error on poll error" << std::endl;
                break;
            case 11:
                static int protocolErrorCount = 0;
                protocolErrorCount++;
                std::cout << "Client emitted error on invalid protocol" << std::endl;
                if (protocolErrorCount > 1) {
                    std::cout << "FAILURE:  " << protocolErrorCount << " errors emitted for one connection!" << std::endl;
                    exit(-1);
                }
                break;
            default:
                std::cout << "FAILURE: " << user << " should not emit error!" << std::endl;
                exit(-1);
        }
    });

    hub.onConnection([](uWS::WebSocket<uWS::CLIENT>* ws, uWS::HttpRequest req) {
        switch ((long) ws->getUserData()) {
            case 8:
                std::cout << "Client established a remote connection over non-SSL" << std::endl;
                ws->close(1000);
                break;
            case 9:
                std::cout << "Client established a remote connection over SSL" << std::endl;
                ws->close(1000);
                break;
            default:
                std::cout << "FAILURE: " << ws->getUserData() << " should not connect!" << std::endl;
                exit(-1);
        }
    });

    hub.onDisconnection([](uWS::WebSocket<uWS::CLIENT>* ws, int code, char* message, size_t length) {
        std::cout << "Client got disconnected with data: " << ws->getUserData() << ", code: " << code << ", message: <"
                  << std::string(message, length) << ">" << std::endl;
    });

    hub.onDisconnection([](uWS::WebSocket<uWS::CLIENT> *ws, int code, char *message, size_t length) {
        std::cout << "Client got disconnected with data: " << ws->getUserData() << ", code: " << code << ", message: <" << std::string(message, length) << ">" << std::endl;
    });

    auto user = nullptr;
    std::map<std::string, std::string> extraHeaders;
    int timeoutInMilliseconds = 2000;

    hub.connect(SERVER_CONNECTION_ADDRESS, user, extraHeaders, timeoutInMilliseconds);

    hub.run();

}

void UWSServerCommunicationStrategy::closeConnection() {

}

void UWSServerCommunicationStrategy::sendMessage(MESSAGE&& message) {

}

void UWSServerCommunicationStrategy::start() {

}

void UWSServerCommunicationStrategy::initializeClientCallbacks() {
}

//void UWSServerCommunicationStrategy::onOpen(ConnectionHandle connectionHandle){

//}

void UWSServerCommunicationStrategy::processMessage(DataFlow::Frame&& message) {

}

int UWSServerCommunicationStrategy::connect(std::string const& serverAddress) {
    return 0;
}
