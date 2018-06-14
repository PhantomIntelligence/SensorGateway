//
// Created by samuelbouchard on 13/06/18.
//

#include "spirit-sensor-gateway/application-messages-implementation/MessageInterpretationStrategy.cpp"
#include <iostream>
#include <cstdio>
#include <fstream>



int main() {
    auto fileWithRawData = std::ifstream("AWLMessages.txt");

    MessageInterpretationStrategy  *messageInterpretationStrategy = new MessageInterpretationStrategy();
    std::string currentReadingLine;
    while(std::getline(fileWithRawData,currentReadingLine))
    {
        if(currentReadingLine.find("ID")){
            std::cout<<currentReadingLine<<std::endl;
        }
    }


    return 0;

}