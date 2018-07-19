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

#ifndef SPIRITSENSORGATEWAY_UWSSERVERCOMMUNICATORTEST_CPP
#define SPIRITSENSORGATEWAY_UWSSERVERCOMMUNICATORTEST_CPP

#include <gtest/gtest.h>
#include "spirit-sensor-gateway/server-communication/UWSServerCommunicationStrategy.h"
#include "test/utilities/stub/SpiritFramesStub.h"
#include "test/utilities/stub/WebSocketServerStub.h"

using ServerCommunication::UWSServerCommunicationStrategy;
using ServerCommunication::JsonConverter;
using Stub::WebSocketServerStub;
using Stub::createRandomIDFrame;
using DataFlow::Frame;


class UWSServerCommunicatorTest : public ::testing::Test {
protected:
    std::string const SERVER_ACTUAL_MESSAGE_LOGFILE = "ActualWebSocketServerMessageFile.txt";
    std::string const SERVER_EXPECTED_MESSAGE_LOGFILE = "ExpectedWebSocketServerMessageFile.txt";
    std::FILE* expectedLogFile;
};

TEST_F(UWSServerCommunicatorTest,
       given_someIncomingFrames_when_sendingMessages_then_messagesAreCorrectlyReceivedByTheWebSocketServer) {

    expectedLogFile = std::fopen(SERVER_EXPECTED_MESSAGE_LOGFILE.c_str(), "w");
    WebSocketServerStub webSocketServerStub(SERVER_ACTUAL_MESSAGE_LOGFILE);
    UWSServerCommunicationStrategy uwsServerCommunicationStrategy;
    webSocketServerStub.run();
    uwsServerCommunicationStrategy.openConnection(webSocketServerStub.getAddress());

    for (auto i = 0; i < 10; i++) {
        auto frame = createRandomIDFrame();
        auto frameCopy = Frame(frame);
        auto expectedFormattedFrame = JsonConverter::convertFrameToJsonString(frameCopy);
        std::fprintf(expectedLogFile, "%s", expectedFormattedFrame.c_str());
        uwsServerCommunicationStrategy.sendMessage(std::move(frame));
    }

    uwsServerCommunicationStrategy.closeConnection();

    fflush(expectedLogFile);
    fclose(expectedLogFile);

}

#endif //SPIRITSENSORGATEWAY_UWSSERVERCOMMUNICATORTEST_CPP
