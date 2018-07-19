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
#include "test/utilities/stub/WebSocketServerStub.h"
#include "test/utilities/stub/SpiritFramesStub.h"
#include "spirit-sensor-gateway/server-communication/UWSServerCommunicationStrategy.h"

using ServerCommunication::UWSServerCommunicationStrategy;
using Stub::WebSocketServerStub;


class UWSServerCommunicatorTest: public ::testing::Test {
protected:
    char const* AWLMESSAGES_INPUT_FILE_NAME = "AWLMessagesInputFile.txt";
    char const* EXPECTED_SPIRIT_FRAMES_OUTPUT_FILE_NAME = "ExpectedSpiritFramesOutputFile.txt";
};

TEST_F(UWSServerCommunicatorTest,
       given_someIncomingFrames_when_sendingMessages_then_messagesAreCorrectlyReceivedByTheWebSocketServer) {

    WebSocketServerStub webSocketServerStub;
    UWSServerCommunicationStrategy uwsServerCommunicationStrategy;
    uwsServerCommunicationStrategy.openConnection(webSocketServerStub.getAddress());




}

#endif //SPIRITSENSORGATEWAY_UWSSERVERCOMMUNICATORTEST_CPP
