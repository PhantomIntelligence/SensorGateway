
#include "spirit-sensor-gateway/communication-protocol-strategy/KvaserCanProtocolStrategy.cpp"
#include <iostream>
#include <cstdio>

#include <inttypes.h>

int main(){

    auto file = std::fopen("AWLMessages.txt", "w+");

    KvaserCanProtocolStrategy * kvaserCanProtocolStrategy = new KvaserCanProtocolStrategy();

    for (auto i = 0; i < 10000; i++){
        AWLMessage message = kvaserCanProtocolStrategy->unwrapMessage();
        std::fprintf(file, "=================================================================================== \n");
        std::fprintf(file, "ID : %" PRIu64 "\n",message.messageID);
        std::fprintf(file, "Length : %" PRIu64 "\n",message.messageLength);
        std::fprintf(file, "Timestamp : %" PRIu64 "\n",message.messageTimestamp);
        for(int j=0 ; j<MESSAGE_DATA_LENGTH;j++) {
            std::fprintf(file, "Data position %d (decimal): %d \n", j, message.messageData[j]);
            if(j==MESSAGE_DATA_LENGTH-1){
                std::fprintf(file, "Message en Hexadecimal : %x %x %x %x %x %x %x %x \n",message.messageData[0],message.messageData[1],message.messageData[2],message.messageData[3],message.messageData[4],message.messageData[5],message.messageData[6],message.messageData[7]);
            }
        }
    }

    kvaserCanProtocolStrategy->closeConnection();
    delete kvaserCanProtocolStrategy;
    fflush(file);
    fclose(file);
    return 0;
};