
#include "spirit-sensor-gateway/communication-protocol-strategy/KvaserCanProtocolStrategy.cpp"
#include <iostream>
#include <cstdio>

#include <inttypes.h>

int main(){

    auto file = std::fopen("AWLMessages.txt", "w+");

    KvaserCanProtocolStrategy * kvaserCanProtocolStrategy = new KvaserCanProtocolStrategy();

    for (auto i = 0; i < 10000; i++){
        AWLMessage message = kvaserCanProtocolStrategy->readMessage();
        std::fprintf(file, "=================================================================================== \n");
        std::fprintf(file, "ID : %" PRIu64 "\n",message.id);
        std::fprintf(file, "Length : %" PRIu64 "\n",message.Length);
        std::fprintf(file, "timestamp : %" PRIu64 "\n",message.timestamp);
        for(int j=0 ; j<MESSAGE_DATA_LENGTH_IN_MESSAGE;j++) {
            std::fprintf(file, "Data position %d (decimal): %d \n", j, message.Data[j]);
            if(j==MESSAGE_DATA_LENGTH_IN_MESSAGE-1){
                std::fprintf(file, "Message en Hexadecimal : %x %x %x %x %x %x %x %x \n",message.Data[0],message.Data[1],message.Data[2],message.Data[3],message.Data[4],message.Data[5],message.Data[6],message.Data[7]);
            }
        }
    }

    kvaserCanProtocolStrategy->closeConnection();
    delete kvaserCanProtocolStrategy;
    fflush(file);
    fclose(file);
    return 0;
};