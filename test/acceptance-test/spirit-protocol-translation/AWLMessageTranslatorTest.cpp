#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <experimental/filesystem>
#include <fstream>
#include "spirit-sensor-gateway/spirit-protocol-translation/AWLMessageTranslator.h"

namespace filesystem = std::experimental::filesystem;

class AWLMessageTranslatorTest:  public ::testing::Test {
public:
    AWLMessageTranslator* awlMessageTranslator = new AWLMessageTranslator();

    void buildFramesFile(std::vector<Frame> frames, std::ifstream& framesFile){

    }

    std::vector<AWLMessage> fetchAWLMessagesFromFile(std::string const &fileName){
        std::string currentPath = filesystem::current_path();
        std::ifstream file = std::ifstream(currentPath + "/" + fileName);
        std::vector<AWLMessage> messages = parseAWLMessagesFromFile(file);
        return messages;
    }

    virtual void SetUp(){

    }

    virtual void TearDown(){

    }

private:
    std::vector<AWLMessage> parseAWLMessagesFromFile(std::ifstream& file){
        std::vector<AWLMessage> messages;
        if (file) {
            std::string fileLine;
            while (std::getline(file, fileLine)) {
                AWLMessage message = parseAWLMessageFromFileLine(fileLine);
                messages.push_back(message);
            }
        } else {
            std::cerr << "An error occurred while opening the file.\n";
        }
        return messages;
    }

    AWLMessage parseAWLMessageFromFileLine(std::string& fileLine){
        AWLMessage message{};
        std::string messageContent[11];
        parseAWLMessageContentFromFileLine(fileLine, messageContent);
        message.id = stoi(messageContent[0]);
        message.length = (unsigned) stoi(messageContent[1]);
        message.timestamp = (unsigned) stoi(messageContent[2]);
        for (auto dataPosition = 0; dataPosition < MAX_NUMBER_OF_DATA_IN_AWL_MESSAGE; dataPosition++) {
            auto dataValue = std::stoi(messageContent[3 + dataPosition]);
            message.data[dataPosition] = static_cast<unsigned char>(dataValue);
        }
        return message;
    }

    void parseAWLMessageContentFromFileLine(std::string fileLine, std::string* messageContentArray) {
        std::string messageContentSeparator = "-";
        size_t separatorPosition = 0;
        auto contentPosition = 0;
        while ((separatorPosition = fileLine.find(messageContentSeparator)) != std::string::npos) {
            std::string content = fileLine.substr(0, separatorPosition);
            messageContentArray[contentPosition] = content;
            contentPosition++;
            fileLine.erase(0, separatorPosition + messageContentSeparator.length());
        }
        messageContentArray[contentPosition] = fileLine;
    }
};

TEST_F(AWLMessageTranslatorTest, ordered){
    std::vector<AWLMessage> messages = fetchAWLMessagesFromFile("AWLMessages.txt");
    for(auto message : messages){
        awlMessageTranslator->translateBasicMessage(&message);
    }
    std::vector<Frame> frames = awlMessageTranslator->getFrames();
    //buildFramesFile(frames, framesFile);
}



