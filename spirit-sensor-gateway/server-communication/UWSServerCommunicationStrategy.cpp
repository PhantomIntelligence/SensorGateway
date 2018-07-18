#include "UWSServerCommunicationStrategy.h"

using ServerCommunication::UWSServerCommunicationStrategy;


UWSServerCommunicationStrategy::UWSServerCommunicationStrategy() {
    initializeClientCallbacks();
}

UWSServerCommunicationStrategy::~UWSServerCommunicationStrategy() {

}

void UWSServerCommunicationStrategy::openConnection() {
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

int UWSServerCommunicationStrategy::connect(std::string const& serverAddress) {
    return 0;
}
