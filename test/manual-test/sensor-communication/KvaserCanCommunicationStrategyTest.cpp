#ifndef SENSORGATEWAY_KVASERREADMESSAGETOFILETEST_CPP
#define SENSORGATEWAY_KVASERREADMESSAGETOFILETEST_CPP

#include <cinttypes>
#include "sensor-gateway/sensor-communication/KvaserCanCommunicationStrategy.hpp"

int main() {

    const int NUMBER_OF_DETECTIONS = 42;
    auto file = std::fopen("AWLMessagesCustom.txt", "w+");
    auto sensorChannelId = 0;

    using Structures = typename Sensor::AWL::Structures<>;
    SensorCommunication::KvaserCanCommunicationStrategy<Structures> kvaserCanProtocolStrategy(sensorChannelId);
    kvaserCanProtocolStrategy.openConnection();


    //FOR GENERATING TXT FILE
    for (auto i = 0; i < NUMBER_OF_DETECTIONS; i++) {
        auto messages = kvaserCanProtocolStrategy.fetchMessages();
        for (uint64_t j = 0; j < messages.size(); ++j) {
            auto message = messages[j];
            std::fprintf(file,
                         "=================================================================================== \n");
            std::fprintf(file, "SENSOR_ID : %" PRIu64 "\n", message.id);
            std::fprintf(file, "length : %d \n", message.length);
            std::fprintf(file, "timestamp : %" PRIu64 "\n", message.timestamp);
            for (uint32_t k = 0; k < message.length; k++) {
                std::fprintf(file, "data position %d (decimal): %d \n", k, message.data[k]);
                if (k == message.length - 1) {
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
}

#endif // SENSORGATEWAY_KVASERREADMESSAGETOFILETEST_CPP
