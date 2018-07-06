
#include <iostream>
#include <cstdio>
#include <inttypes.h>
#include "spirit-sensor-gateway/sensor-communication/KvaserCanProtocolStrategy.cpp"


int main(){

    const int NUMBER_DETECTION = 1000;
    auto file = std::fopen("AWLMessagesCustom.txt", "w+");

    KvaserCanProtocolStrategy kvaserCanProtocolStrategy;
    kvaserCanProtocolStrategy.openConnection();

    //FOR GENERATING TXT FILE
    for (auto i = 0; i < NUMBER_DETECTION; i++) {
        AWLMessage message = kvaserCanProtocolStrategy.readMessage();
        std::fprintf(file, "=================================================================================== \n");
        std::fprintf(file, "ID : %" PRIu64 "\n", message.id);
        std::fprintf(file, "length : %d \n", message.length);
        std::fprintf(file, "timestamp : %" PRIu64 "\n", message.timestamp);
        for (int j = 0; j < message.length; j++) {
            std::fprintf(file, "data position %d (decimal): %d \n", j, message.data[j]);
            if (j == message.length - 1) {
                std::fprintf(file, "Message en Hexadecimal : %x %x %x %x %x %x %x %x \n", message.data[0],
                             message.data[1], message.data[2], message.data[3], message.data[4], message.data[5],
                             message.data[6], message.data[7]);
            }
        }
    }


    kvaserCanProtocolStrategy.closeConnection();
    fflush(file);
    fclose(file);
    return 0;
};
