
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <array>
#include <ios>


#include "spirit-sensor-gateway/spirit-protocol-translation/AWLMessageTranslator.h"

void stringSplit(std::string stringToSplit, std::string *awlData) {
    std::string separator = "-";
    size_t position = 0;
    int positionData = 0;
    while ((position = stringToSplit.find(separator)) != std::string::npos) {
        std::string data = stringToSplit.substr(0, position);
        awlData[positionData] = data;
        positionData++;
        stringToSplit.erase(0, position + separator.length());
    }
    awlData[positionData] = stringToSplit;
}

int main() {

    auto file = std::fopen("frameList.txt", "w+");
    AWLMessageTranslator awlMessageTranslator;
    auto AwlMessagesFiles = std::ifstream(
            "/home/phantom/SpiritSensorGateway/test/manual-test/spirit-protocol/AWLMessagesCustom.txt");
    if (AwlMessagesFiles) {
        std::string ligne;
        while (std::getline(AwlMessagesFiles, ligne)) {
            std::string awlData[11];
            stringSplit(ligne, awlData);
            AWLMessage message{};
            message.id = stoi(awlData[0]);
            message.length = (unsigned) stoi(awlData[1]);
            message.timestamp = (unsigned) stoi(awlData[2]);
            for (int i = 0; i < MAX_NUMBER_OF_DATA_IN_AWL_MESSAGE; i++) {
                auto dataValue = std::stoi(awlData[3 + i]);
                message.data[i] = static_cast<unsigned char>(dataValue);
            }
            AWLMessage *pointerToMessage = &message;
            awlMessageTranslator.translateBasicMessage(pointerToMessage);
        }
    } else {
        std::cerr << "Erreur d'ouverture de fichier\n";
    }

    std::vector<SpiritFrame> framelist;
    framelist = awlMessageTranslator.getSpiritFrames();
    for (std::vector<SpiritFrame>::iterator itFrame = framelist.begin(); itFrame != framelist.end(); ++itFrame) {
        uint16_t idFrame = itFrame->getFrameID();
        uint16_t systemId = itFrame->getSystemID();
        //uint32_t errorFlag = itFrame->errorFlag;
        std::unordered_map<uint16_t, SpiritPixel> sensorPilexlList = itFrame->getPixels();

        std::fprintf(file,
                     "=========================================================================================\n");
        std::fprintf(file, "Frame : %d \n", idFrame);
        std::fprintf(file, "System : %d \n", systemId);

        //std::fprintf(file, "Flags : %d \n",errorFlag);

        for (std::unordered_map<uint16_t, SpiritPixel>::iterator itPixel = sensorPilexlList.begin();
             itPixel != sensorPilexlList.end(); ++itPixel) {
            uint16_t idPixel = itPixel->second.getId();
            std::unordered_map<uint16_t, SpiritTrack>  sensorTrackList = itPixel->second.getTracks();

            std::fprintf(file, "Pixel %d : \n", idPixel);

            for (std::unordered_map<uint16_t, SpiritTrack>::iterator itTrack = sensorTrackList.begin();
                itTrack != sensorTrackList.end(); ++itTrack) {
                uint16_t trackId = itTrack->second.getTrackID();
                int16_t acceleration = itTrack->second.getAcceleration();
                uint16_t distance = itTrack->second.getDistance();
                uint16_t intensity = itTrack->second.getIntensity();
                uint8_t confidenceLevel = itTrack->second.getConfidenceLevel();
                int16_t speed = itTrack->second.getSpeed();

                std::fprintf(file, ".. Track : %d \n", trackId);
                std::fprintf(file, "..... Acceleration : %d \n", acceleration);
                std::fprintf(file, "..... Distance : %d \n", distance);
                std::fprintf(file, "..... Intensity : %d \n", intensity);
                std::fprintf(file, "..... Confidence Level : %d \n", confidenceLevel);
                std::fprintf(file, "..... Speed : %d \n", speed);
            }
        }
    }
    fflush(file);
    fclose(file);

    return 0;
}

