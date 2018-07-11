//#include <websocketpp/client.hpp>
#include "WebSocketServerCommunicationStrategy.h"

using ServerCommunication::WebSocketServerCommunicationStrategy;


WebSocketServerCommunicationStrategy::WebSocketServerCommunicationStrategy(){
    initializeClientCallbacks();
}

WebSocketServerCommunicationStrategy::~WebSocketServerCommunicationStrategy() {

}

void WebSocketServerCommunicationStrategy::openConnection() {

}

void WebSocketServerCommunicationStrategy::closeConnection() {

}

void WebSocketServerCommunicationStrategy::sendMessage(MESSAGE&& message) {

}

void WebSocketServerCommunicationStrategy::start() {

}

void WebSocketServerCommunicationStrategy::initializeClientCallbacks() {
    //Set the endpoint logging behavior to silent
//    client.clear_access_channels(websocketpp::log::alevel::all);
//    client.clear_error_channels(websocketpp::log::alevel::all);

//    client.init_asio();
//    client.start_perpetual();

}

//void WebSocketServerCommunicationStrategy::onOpen(ConnectionHandle connectionHandle){

//}

void WebSocketServerCommunicationStrategy::processMessage(SpiritProtocol::Frame&& message) {

}

int WebSocketServerCommunicationStrategy::connect(std::string const& serverAddress) {
    return 0;
}
