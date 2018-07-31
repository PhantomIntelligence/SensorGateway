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

#ifndef SPIRITSENSORGATEWAY_MESSAGETRANSLATORTEST_CPP
#define SPIRITSENSORGATEWAY_MESSAGETRANSLATORTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include "test/utilities/files/AWLMessagesFileManager.h"
#include "test/utilities/files/SpiritFramesFileManager.h"
#include "test/utilities/stub/AWLMessagesStub.h"
#include "test/utilities/stub/SpiritFramesStub.h"
#include "spirit-sensor-gateway/message-translation/AWLMessageToSpiritMessageTranslationStrategy.h"
#include "test/utilities/mock/FrameSinkMock.h"

using Mock::FrameProcessingScheduler;
using Mock::FrameSinkMock;
using TestUtilities::AWLMessagesFileManager;
using TestUtilities::SpiritFramesFileManager;
using Stub::createAWLMessageStub;
using Stub::createSpiritFramesStub;
using MessageTranslation::AWLMessageToSpiritMessageTranslationStrategy;

class AWLMessageToSpiritMessageTranslationStrategyTest : public ::testing::Test {
protected:
    char const* AWLMESSAGES_INPUT_FILE_NAME = "AWLMessagesInputFile.txt";
    char const* EXPECTED_SPIRIT_FRAMES_OUTPUT_FILE_NAME = "ExpectedSpiritFramesOutputFile.txt";

    virtual void SetUp() {
        auto awlMessages = createAWLMessageStub();
        auto spiritFrames = createSpiritFramesStub();
        awlMessagesFileManager.writeFileWithMessages(awlMessages, AWLMESSAGES_INPUT_FILE_NAME);
        spiritFramesFileManager.writeFileWithMessages(spiritFrames, EXPECTED_SPIRIT_FRAMES_OUTPUT_FILE_NAME);
    }

    AWLMessagesFileManager awlMessagesFileManager;
    SpiritFramesFileManager spiritFramesFileManager;
};

TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,
       given_someInputFileContainingValidAWLMessages_when_translatingAWLMessagesIntoSpiritFrames_then_returnCorrespondingSpriritFramesOutputFile) {
    auto ACTUAL_SPIRIT_FRAMES_OUTPUT_FILE_NAME = "ActualSpiritFramesOutputFile.txt";
    AWLMessageToSpiritMessageTranslationStrategy awlMessageTranslator;
    FrameSinkMock frameSinkMock(1);
    FrameProcessingScheduler scheduler(&frameSinkMock);
    awlMessageTranslator.linkConsumer(&scheduler);
    int counter = 0;

    auto messages = awlMessagesFileManager.readMessagesFromFile(AWLMESSAGES_INPUT_FILE_NAME);
    for (auto message : messages) {
        awlMessageTranslator.translateMessage(std::move(message));
        ++counter;
    }

    scheduler.terminateAndJoin();

    auto frames = frameSinkMock.getConsumedData();

    spiritFramesFileManager.writeFileWithFrames(frames, ACTUAL_SPIRIT_FRAMES_OUTPUT_FILE_NAME);
    ASSERT_TRUE(spiritFramesFileManager.areFilesEqual(EXPECTED_SPIRIT_FRAMES_OUTPUT_FILE_NAME,
                                                      ACTUAL_SPIRIT_FRAMES_OUTPUT_FILE_NAME));
}

#endif //SPIRITSENSORGATEWAY_MESSAGETRANSLATORTEST_CPP
