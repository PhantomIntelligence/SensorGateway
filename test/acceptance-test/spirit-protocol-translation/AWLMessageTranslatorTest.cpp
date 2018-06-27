#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <experimental/filesystem>
#include <fstream>
#include "spirit-sensor-gateway/spirit-protocol-translation/AWLMessageTranslator.h"

namespace filesystem = std::experimental::filesystem;

class AWLMessageTranslatorTest:  public ::testing::Test {
    public:
        std::vector<AWLMessage> fetchAWLMessagesFromFile(std::string const &fileName){
            std::string currentPath = filesystem::current_path();
            std::ifstream file = std::ifstream(currentPath + "/" + fileName);
            std::vector<AWLMessage> messages = readAWLMessagesFromFile(file);
            return messages;
        }
    private:
        std::vector<AWLMessage> readAWLMessagesFromFile(std::ifstream& file){
            std::vector<AWLMessage> messages;
            if (file) {
                std::string fileLine;
                while (std::getline(file, fileLine)) {
                    AWLMessage message = readAWLMessageFromFileLine(fileLine);
                    messages.push_back(message);
                }
            } else {
                std::cerr << "An error occurred while opening the file.\n";
            }
            return messages;
        }

        AWLMessage readAWLMessageFromFileLine(std::string& fileLine){
            AWLMessage message{};
            std::string messageContent[11] = fetchAWLMessageContentFromFileLine(fileLine);
            message.id = stoi(messageContent[0]);
            message.length = (unsigned) stoi(messageContent[1]);
            message.timestamp = (unsigned) stoi(messageContent[2]);
            for (auto dataPosition = 0; dataPosition < MAX_NUMBER_OF_DATA_IN_AWL_MESSAGE; dataPosition++) {
                auto dataValue = std::stoi(messageContent[3 + dataPosition]);
                message.data[dataPosition] = static_cast<unsigned char>(dataValue);
            }
            return message;
        }

        void fetchAWLMessageContentFromFileLine(std::string stringToSplit) {
            std::string messageContentSeparator = "-";
            size_t position = 0;
            auto contentPosition = 0;
            while ((position = stringToSplit.find(messageContentSeparator)) != std::string::npos) {
                std::string data = stringToSplit.substr(0, position);
                awlData[contentPosition] = data;
                contentPosition++;
                stringToSplit.erase(0, position + messageContentSeparator.length());
            }
            awlData[contentPosition] = stringToSplit;
        }
};

TEST_F(AWLMessageTranslatorTest, ordered){
    std::vector<AWLMessage> messages = fetchAWLMessagesFromFile("AWLMessages.txt");
}



