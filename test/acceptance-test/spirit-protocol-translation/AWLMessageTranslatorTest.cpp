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
#include "test/utilities/AWLMessagesFileManager.h"
#include "test/utilities/SpiritFramesFileManager.h"
#include "test/acceptance-test/fixtures/AWLMessagesFixture.cpp"
#include "test/acceptance-test/fixtures/SpiritFramesFixture.cpp"
#include "spirit-sensor-gateway/spirit-protocol-translation/AWLMessageTranslator.h"

using namespace TestUtilities;

class AWLMessageTranslatorTest : public ::testing::Test {
protected:
    virtual void SetUp(){
        awlMessagesFileManager.writeFileWithMessages(awlMessagesFixture, "AWLMessagesInputFile.txt");
        spiritFramesFileManager.writeFileWithMessages(spiritFramesFixture, "ExpectedSpiritFramesOutputFile.txt");
    }

    AWLMessagesFileManager awlMessagesFileManager;
    SpiritFramesFileManager spiritFramesFileManager;
};

TEST_F(AWLMessageTranslatorTest, given_someInputFileContainingValidAWLMessages_when_translatingAWLMessagesIntoSpiritFrames_then_returnCorrespondingSpriritFramesOutputFile) {
    AWLMessageTranslator awlMessageTranslator;
    std::vector<AWLMessage> messages = awlMessagesFileManager.readMessagesFromFile("AWLMessagesInputFile.txt");
    for (auto message : messages) {
        awlMessageTranslator.translateBasicMessage(&message);
    }
    std::vector<Frame> frames = awlMessageTranslator.getFrames();
    spiritFramesFileManager.writeFileWithMessages(frames, "ActualSpiritFramesOutputFile.txt");
    ASSERT_TRUE(spiritFramesFileManager.areFilesEqual("ExpectedSpiritFramesOutputFile.txt", "ActualSpiritFramesOutputFile.txt"));
}



