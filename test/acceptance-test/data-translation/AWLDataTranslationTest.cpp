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

#ifndef SENSORGATEWAY_AWLDATATRANSLATORTEST_CPP
#define SENSORGATEWAY_AWLDATATRANSLATORTEST_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <fstream>
#include "test/utilities/files/AWLMessagesFileManager.h"
#include "test/utilities/files/SensorMessageFileManager.h"
#include "test/utilities/stub/AWLMessageStub.h"
#include "test/utilities/stub/SensorMessageStub.h"
#include "test/utilities/mock/ArbitraryDataSinkMock.hpp"
#include "sensor-gateway/data-translation/AWLTranslationStrategy.h"

using TestUtilities::AWLMessagesFileManager;
using TestUtilities::SensorMessageFileManager;
using Stub::createAWLMessageStub;
using Stub::createSensorMessageStub;
using DataTranslation::AWLTranslationStrategy;

using SensorMessageSinkMock = Mock::ArbitraryDataSinkMock<DataFlow::SensorMessage>;
using SensorMessageProcessingScheduler = DataFlow::DataProcessingScheduler<DataFlow::SensorMessage, SensorMessageSinkMock, 1>;

class AWLTranslationStrategyTest : public ::testing::Test {
protected:
    char const* AWLDATAS_INPUT_FILE_NAME = "AWLMessagesInputFile.txt";
    char const* EXPECTED__FRAMES_OUTPUT_FILE_NAME = "ExpectedSensorMessagesOutputFile.txt";

    virtual void SetUp() {
        auto awlMessages = createAWLMessageStub();
        auto SensorMessages = createSensorMessageStub();
        awlMessagesFileManager.writeFileWithMessages(awlMessages, AWLDATAS_INPUT_FILE_NAME);
        sensorMessageFileManager.writeFileWithMessages(SensorMessages, EXPECTED__FRAMES_OUTPUT_FILE_NAME);
    }

    AWLMessagesFileManager awlMessagesFileManager;
    SensorMessageFileManager sensorMessageFileManager;
};

TEST_F(AWLTranslationStrategyTest,
       given_someInputFileContainingValidAWLMessages_when_translatingAWLMessagesIntoSensorMessages_then_returnCorrespondingSpiritSensorMessagesOutputFile) {
    auto ACTUAL__FRAMES_OUTPUT_FILE_NAME = "ActualSensorMessagesOutputFile.txt";
    AWLTranslationStrategy awlMessageTranslator;
    SensorMessageSinkMock sensorMessageSinkMock(1);
    SensorMessageProcessingScheduler scheduler(&sensorMessageSinkMock);
    awlMessageTranslator.linkConsumer(&scheduler);
    int counter = 0;

    auto messages = awlMessagesFileManager.readMessagesFromFile(AWLDATAS_INPUT_FILE_NAME);
    for (auto message : messages) {
        awlMessageTranslator.translateMessage(std::move(message));
        ++counter;
    }

    scheduler.terminateAndJoin();
    sensorMessageSinkMock.waitConsumptionToBeReached();

    auto sensorMessages = sensorMessageSinkMock.getConsumedData();

    sensorMessageFileManager.writeFileWithSensorMessages(sensorMessages, ACTUAL__FRAMES_OUTPUT_FILE_NAME);
    ASSERT_TRUE(sensorMessageFileManager.areFilesEqual(EXPECTED__FRAMES_OUTPUT_FILE_NAME,
                                               ACTUAL__FRAMES_OUTPUT_FILE_NAME));
}

#endif //SENSORGATEWAY_AWLDATATRANSLATORTEST_CPP
