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
#include "test/utilities/files/AWLMessagesFileManager.h"
#include "test/utilities/files/SpiritFramesFileManager.h"
#include "test/acceptance-test/fixtures/AWLMessagesFixture.cpp"
#include "test/acceptance-test/fixtures/SpiritFramesFixture.cpp"
#include "spirit-sensor-gateway/protocol-translation/AWLMessageToSpiritMessageTranslationStrategy.h"

using namespace TestUtilities;
using ProtocolTranslation::AWLMessageToSpiritMessageTranslationStrategy;

class AWLMessageToSpiritMessageTranslationStrategyTest : public ::testing::Test {
protected:
    char const*  AWLMESSAGES_INPUT_FILE_NAME = "AWLMessagesInputFile.txt";
    char const*  EXPECTED_SPIRIT_FRAMES_OUTPUT_FILE_NAME = "ExpectedSpiritFramesOutputFile.txt";
    virtual void SetUp(){
        awlMessagesFileManager.writeFileWithMessages(awlMessagesFixture, AWLMESSAGES_INPUT_FILE_NAME);
        spiritFramesFileManager.writeFileWithMessages(spiritFramesFixture, EXPECTED_SPIRIT_FRAMES_OUTPUT_FILE_NAME);
    }
    AWLMessagesFileManager awlMessagesFileManager;
    SpiritFramesFileManager spiritFramesFileManager;
};


using FrameSink = DataFlow::DataSink<Frame>;

class FrameSinkMock : public FrameSink {

protected:

    using FrameSink::DATA;

public:

    FrameSinkMock(uint8_t numberOfDataToConsumeGoal) :
            actualNumberOfDataConsumed(0),
            numberOfDataToConsumeGoal(numberOfDataToConsumeGoal) {

    }

    void consume(DATA&& data) override {
        ++actualNumberOfDataConsumed;
        consumedData.push_back(data);
    }

    bool hasBeenCalledExpectedNumberOfTimes() const {
        return actualNumberOfDataConsumed.load() == numberOfDataToConsumeGoal;
    };


    std::vector<Frame> getConsumedData() const noexcept {
        return consumedData;
    }

private:

    AtomicCounter actualNumberOfDataConsumed;
    AtomicCounter numberOfDataToConsumeGoal;

    std::vector<Frame> consumedData;
};

using FrameProcessingScheduler = DataFlow::DataProcessingScheduler<Frame, FrameSinkMock, 1>;

TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest, given_someInputFileContainingValidAWLMessages_when_translatingAWLMessagesIntoSpiritFrames_then_returnCorrespondingSpriritFramesOutputFile) {
    char const* ACTUAL_SPIRIT_FRAMES_OUTPUT_FILE_NAME = "ActualSpiritFramesOutputFile.txt";
    AWLMessageToSpiritMessageTranslationStrategy awlMessageTranslator;
    FrameSinkMock frameSinkMock(1);
    FrameProcessingScheduler scheduler(&frameSinkMock);
    awlMessageTranslator.linkConsumer(&scheduler);
    
    
    std::vector<AWLMessage> messages = awlMessagesFileManager.readMessagesFromFile(AWLMESSAGES_INPUT_FILE_NAME);
    for (auto message : messages) {
        awlMessageTranslator.translateBasicMessage(std::move(message));
    }

    scheduler.terminateAndJoin();
    
    std::vector<Frame> frames = frameSinkMock.getConsumedData(); 
    spiritFramesFileManager.writeFileWithMessages(frames, ACTUAL_SPIRIT_FRAMES_OUTPUT_FILE_NAME);
    ASSERT_TRUE(spiritFramesFileManager.areFilesEqual(EXPECTED_SPIRIT_FRAMES_OUTPUT_FILE_NAME,
                                                      ACTUAL_SPIRIT_FRAMES_OUTPUT_FILE_NAME));
}



