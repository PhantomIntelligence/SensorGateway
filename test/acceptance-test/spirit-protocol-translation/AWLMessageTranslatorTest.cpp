/**
	Copyright 2014-2018 Phantom Intelligence Inc.
	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at
		http://www.apache.org/licenses/LICENSE-2.0
	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include "test/acceptance-test/fixtures/AWLMessagesFixture.cpp"
#include "test/acceptance-test/fixtures/SpiritFramesFixture.cpp"
#include "test/utilities/AWLMessagesFileManager.h"
#include "test/utilities/SpiritFramesFileManager.h"
#include "spirit-sensor-gateway/spirit-protocol-translation/AWLMessageTranslator.h"

using namespace TestUtilities;

class AWLMessageTranslatorTest : public ::testing::Test {
protected:
    virtual void SetUp(){
        awlMessagesFileManager.buildFileFromSensorMessages(awlMessagesFixture, "AWLMessagesInputFile.txt");
        spiritFramesFileManager.buildFileFromSpiritFrames(spiritFramesFixture, "ExpectedSpiritFramesOutputFile.txt");
    }

    bool areFilesEqual(std::string const& firstFilename, std::string const& secondFilename) {
        bool filesAreEqual = true;
        auto firstFile = std::ifstream(firstFilename);
        auto secondFile = std::ifstream(secondFilename);

        if (firstFile and secondFile) {
            std::string lineFromFirstFile;
            std::string lineFromSecondFile;
            while (std::getline(firstFile, lineFromFirstFile) and std::getline(secondFile, lineFromSecondFile)) {
                if (lineFromFirstFile.compare(lineFromSecondFile) != 0){
                    filesAreEqual = false;
                    break;
                }
            }
        } else {
            std::cerr << "An error occurred while opening a file.\n";
        }

        return filesAreEqual;
    }

    AWLMessagesFileManager awlMessagesFileManager;
    SpiritFramesFileManager spiritFramesFileManager;
};

TEST_F(AWLMessageTranslatorTest, given_someInputFileContainingValidAWLMessages_when_translatingAWLMessagesIntoSpiritFrames_then_returnCorrespondingSpriritFramesOutputFile) {
    AWLMessageTranslator awlMessageTranslator;
    std::vector<AWLMessage> messages = awlMessagesFileManager.parseSensorMessagesFromFile("AWLMessagesInputFile.txt");
    for (auto message : messages) {
        awlMessageTranslator.translateBasicMessage(&message);
    }
    std::vector<Frame> frames = awlMessageTranslator.getFrames();
    spiritFramesFileManager.buildFileFromSpiritFrames(frames, "ActualSpiritFramesOutputFile.txt");
    ASSERT_TRUE(areFilesEqual("ExpectedSpiritFramesOutputFile.txt", "ActualSpiritFramesOutputFile.txt"));

}



