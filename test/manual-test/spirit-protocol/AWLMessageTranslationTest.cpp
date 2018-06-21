
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <array>


#include "spirit-sensor-gateway/spirit-protocol/AWLMessageTranslator.h"

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


    AWLMessageTranslator awlMessageTranslator;
    auto AwlMessagesFiles = std::ifstream("AWLMessagesCustom.txt");
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
        }
    } else {
        std::cerr << "Erreur d'ouverture de fichier\n";
    }
    return 0;
}
