#include <uWS.h>
#include <iostream>
#include "WebSocketServerStub.h"

using Stub::WebSocketServerStub;
using uWS::Hub;
using WebSocket = uWS::WebSocket<uWS::SERVER>;
std::string const WEBSOCKET_SERVER_STUB_ADDRESS = "ws://localhost:8080";
int const WEBSOCKET_SERVER_STUB_PORT = 8080;


WebSocketServerStub::WebSocketServerStub() {

}

WebSocketServerStub::~WebSocketServerStub() noexcept {

}

void  WebSocketServerStub::startServerStub() {
    Hub hub;

    hub.onMessage([&hub](uWS::WebSocket<uWS::SERVER> *ws, char *message, size_t length, uWS::OpCode opCode){
        auto messageContent = std::string(message,length);
        std::cout << messageContent << std::endl;
    });

//    hub.onConnection([&hub](WebSocket* ws,uWS::HttpRequest req){
//
//    });

    hub.listen(WEBSOCKET_SERVER_STUB_PORT);
    hub.run();


}

std::string const& Stub::WebSocketServerStub::getAddress() {
    return WEBSOCKET_SERVER_STUB_ADDRESS;
}
