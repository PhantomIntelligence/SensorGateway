
#include <iostream>
#include <cstdio>
#include <inttypes.h>
#include "spirit-sensor-gateway/communication-protocol-strategy/KvaserCanProtocolStrategy.cpp"


int main(){

    const int NUMBER_DETECTION = 1000;
    auto file = std::fopen("AWLMessagesFixture.h", "w+");
    auto jsonFile = std::fopen("AWLMessages.json", "w+");
    auto customFile = std::fopen("AWLMessagesFixture.h","w+");

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


    //FOR GENERATING JSON FILE
    std::fprintf(jsonFile, "[\n");
    std::fprintf(jsonFile,"{ \"id\": 33, \"length\": 8, \"timestamp\": 3973015, \"data\": [4, 0, 64, 6, 0, 0, 117, 0]},\n");    for (auto i = 0; i < NUMBER_DETECTION; i++) {
        AWLMessage message = kvaserCanProtocolStrategy.readMessage();
        std::fprintf(jsonFile, "{");
        std::fprintf(jsonFile, " \"id\": %" PRIu64 ",", message.id);
        std::fprintf(jsonFile, " \"length\": %d,", message.length);
        std::fprintf(jsonFile, " \"timestamp\": %" PRIu64 ",", message.timestamp);
        std::fprintf(jsonFile, " \"data\": [");
        for (int j = 0; j < message.length; j++) {
            std::fprintf(jsonFile, "%d",message.data[j]);

            if (j < message.length - 1) {
                std::fprintf(jsonFile, ", ");
            }
        }
        std::fprintf(jsonFile, "]");
        if (i < NUMBER_DETECTION - 1) {
            std::fprintf(jsonFile, "}, \n");
            if(i == NUMBER_DETECTION/2){
                std::fprintf(jsonFile,"{ \"id\": 27, \"length\": 8, \"timestamp\": 3973015, \"data\": [4, 0, 64, 6, 0, 0, 117, 0]},\n");
            }
        }
        else{
            std::fprintf(jsonFile, "} \n");
        }
    }
    std::fprintf(jsonFile, "]");

    //FOR GENERATING CUSTOM FILE
    std::fprintf(customFile,"33-8-3973015-4-0-64-6-0-0-117-0\n");
    for (auto i = 0; i < NUMBER_DETECTION; i++) {
        AWLMessage message = kvaserCanProtocolStrategy.readMessage();
        std::fprintf(customFile, "%" PRIu64"-", message.id);
        std::fprintf(customFile, "%d-", message.length);
        std::fprintf(customFile, "%" PRIu64"-", message.timestamp);
        for (int j = 0; j < message.length; j++) {
            if (j < message.length - 1) {
                std::fprintf(customFile,"%d-",message.data[j]);
            }
            else{
                std::fprintf(customFile, "%d",message.data[j]);
            }
        }
        if(i == NUMBER_DETECTION/2){
            std::fprintf(customFile,"\n27-8-3973015-4-0-64-6-0-0-117-0\n");
        }
        else{
            std::fprintf(customFile, "\n");
        }
    }

    kvaserCanProtocolStrategy.closeConnection();
    fflush(file);
    fclose(file);
    fflush(jsonFile);
    fclose(jsonFile);
    fflush(customFile);
    fclose(customFile);
    return 0;
};
