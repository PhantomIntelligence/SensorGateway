#ifndef SENSORGATEWAY_KVASERREADMESSAGETOFILETEST_CPP
#define SENSORGATEWAY_KVASERREADMESSAGETOFILETEST_CPP

#include <cinttypes>
#include "sensor-gateway/sensor-communication/KvaserCanCommunicationStrategy.h"


int main() {

    const int NUMBER_OF_DETECTIONS = 42;
    auto file = std::fopen("AWLMessagesCustom.txt", "w+");

    SensorCommunication::KvaserCanCommunicationStrategy kvaserCanProtocolStrategy;
    kvaserCanProtocolStrategy.openConnection();


    //FOR GENERATING TXT FILE
    for (auto i = 0; i < NUMBER_OF_DETECTIONS; i++) {
        auto messages = kvaserCanProtocolStrategy.fetchMessages();
        for (auto j = 0; j < messages.size(); ++j) {
            auto message = messages[j];
            std::fprintf(file,
                         "=================================================================================== \n");
            std::fprintf(file, "SENSOR_ID : %" PRIu64 "\n", message.id);
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
    }


    fflush(file);
    fclose(file);

    kvaserCanProtocolStrategy.closeConnection();

    return 0;
};

#endif // SENSORGATEWAY_KVASERREADMESSAGETOFILETEST_CPP
