#include <uWS.h>
#include <iostream>
#include "WebSocketServerStub.h"

using Stub::WebSocketServerStub;
using uWS::Hub;
using WebSocket = uWS::WebSocket<uWS::SERVER>;
std::string const WEBSOCKET_SERVER_STUB_ADDRESS = "ws://localhost:8080";
int const WEBSOCKET_SERVER_STUB_PORT = 8080;


WebSocketServerStub::WebSocketServerStub(std::string const& logFileName):webSocketServerStubThread(JoinableThread(doNothing)) {
    webSocketServerStubThread.exitSafely();
    logFile = std::fopen(logFileName.c_str(), "w");

}

WebSocketServerStub::~WebSocketServerStub() noexcept {

}

void Stub::WebSocketServerStub::run() {
    webSocketServerStubThread = JoinableThread(&startServerStub, logFile);
}

void WebSocketServerStub::startServerStub(std::FILE* logFile) {
    Hub hub;

    hub.onMessage([&logFile](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode){
        auto messageContent = std::string(message,length);
        std::fprintf(logFile, "%s", message);
    });

    hub.onConnection([&hub](WebSocket* ws,uWS::HttpRequest req){
        std::cout<<"Connected to the server stub"<<std::endl;
    });

    hub.listen(WEBSOCKET_SERVER_STUB_PORT);
    hub.run();


}

std::string const& Stub::WebSocketServerStub::getAddress() {
    return WEBSOCKET_SERVER_STUB_ADDRESS;
}
