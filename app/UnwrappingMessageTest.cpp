
#include "spirit-sensor-gateway/communication-protocol-strategy/KvaserCanProtocolStrategy.cpp"
#include <iostream>
#include <cstdio>


int main(){
    auto file = std::fopen("AWLMessages.txt", "w+");

    KvaserCanProtocolStrategy * kvaserCanProtocolStrategy = new KvaserCanProtocolStrategy();

    for (int i = 0; i < 10000000; i++){
        AWLMessage message = kvaserCanProtocolStrategy->unwrapMessage();
        auto test = message.messageData[0];
//        auto messageData = std::string(reinterpret_cast<char*> (message.messageData), 8* message.messageLength);
        std::fprintf(file, "messageData:\t%s\t\n", &test);
    }

    delete kvaserCanProtocolStrategy;
    fflush(file);
    fclose(file);
    return 0;
};