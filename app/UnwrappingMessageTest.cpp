
#include "spirit-sensor-gateway/communication-protocol-strategy/KvaserCanProtocolStrategy.cpp"
#include <iostream>
#include <fstream>


int main(){
    std::ofstream outfile ("AWLMessages.txt");
    KvaserCanProtocolStrategy kvaserCanProtocolStrategy;

    for (int i = 0; i < 11; i++){
        AWLMessage message = kvaserCanProtocolStrategy.unwrapMessage();
        outfile << "messageId: " << std::to_string(message.messageID) << " messageFlags: " << message.messageFlags << "" << std::endl;
    }

    outfile.close();
    delete kvaserCanProtocolStrategy;

    return 0;
};