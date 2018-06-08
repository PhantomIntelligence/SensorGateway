
#include "spirit-sensor-gateway/communication-protocol-strategy/KvaserCanProtocolStrategy.cpp"
#include <iostream>
#include <fstream>


int main(){
    std::ofstream outfile ("AWLMessages.txt");
    KvaserCanProtocolStrategy * kvaserCanProtocolStrategy= new KvaserCanProtocolStrategy();

    for (int i = 0; i < 11; i++){
        AWLMessage message = kvaserCanProtocolStrategy->unwrapMessage();
        outfile << "messageId: " << message.messageID << " messageFlags: " << message.messageFlags
                << " messageData: " << std::string(reinterpret_cast<char*> (message.messageData), message.messageLength)  << " messageTimestamp: " << message.messageTimestamp << " messageLength: " << message.messageLength << std::endl;
    }

    outfile.close();
    delete kvaserCanProtocolStrategy;
    return 0;
};