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

#ifndef SPIRITSENSORGATEWAY_UWSCONNECTIONTEST_CPP
#define SPIRITSENSORGATEWAY_UWSCONNECTIONTEST_CPP

#include "spirit-sensor-gateway/server-communication/UWSServerCommunicationStrategy.h"
#include "test/utilities/data-model/DataModelFixture.h"

namespace ManualTest {

    void coutMessage(std::string const& message) {
        std::cout << message << std::endl;
    }

    static int protocolErrorCount = 0;

    void handleMultipleConnectionErrorForOneConnection(std::string const& message) {
        protocolErrorCount++;
        std::cout << message << std::endl;
        if (protocolErrorCount > 1) {
            std::cout << "FAILURE:  " << protocolErrorCount
                      << " errors emitted for one connection!" << std::endl;
            exit(-1);
        }
    }

    void coutMessageAndCrash(std::string const& message) {
        coutMessage(message);
        exit(-1);
    }

    void handleOtherErrors(std::vector<long> handledCodes, long receivedCode,
                           std::function<void(std::string const&)> const& callback,
                           std::string const& message) {
        bool codeIsHandled = false;
        for (auto iterator = handledCodes.cbegin(); iterator != handledCodes.cend() && !codeIsHandled; ++iterator) {
            if (receivedCode == *iterator) {
                codeIsHandled = true;
            }
        }

        if (!codeIsHandled) {
            callback(message);
        }
    }

    void handleError(long expectedCode, long receivedCode,
                     std::function<void(std::string const&)> const& callback,
                     std::string const& message) {
        if (expectedCode == receivedCode) {
            callback(message);
        }
    }

    /**
     * Basic test that demonstrate capacity to connect using the library
     * @warning: This test expects a server to be running and to be able to connect
     * @note: This test comes from https://github.com/uNetworking/uWebSockets/blob/master/tests/main.cpp
     */
    int testBasicUWSConnectionCalls() {
        uWS::Hub hub;

        // TODO: Split these cases into multiple functions
        hub.onError([](void* user) {
            long receivedCode = (long) user;
            std::vector<long> handledCodes = {1, 2, 3, 4, 5, 6, 7};
            handleError(1, receivedCode, coutMessage, "Client emitted error on invalid URI");
            handleError(2, receivedCode, handleMultipleConnectionErrorForOneConnection,
                        "Client emitted error on invalid protocol");
            handleError(3, receivedCode, coutMessage, "Client emitted error on resolve failure");
            handleError(4, receivedCode, coutMessage, "Client emitted error on connection timeout (non-SSL)");
            handleError(5, receivedCode, coutMessage, "Client emitted error on connection timeout (SSL)");
            handleError(6, receivedCode, coutMessage,
                        "Client emitted error on HTTP response without upgrade (non-SSL)");
            handleError(7, receivedCode, coutMessage, "Client emitted error on HTTP response without upgrade (SSL)");
            handleOtherErrors(handledCodes, receivedCode, coutMessageAndCrash, "FAILURE: should not emit error!");
        });

        hub.onConnection([](uWS::WebSocket<uWS::CLIENT>* ws, uWS::HttpRequest req) {
            switch ((long) ws->getUserData()) {
                case 8:
                    std::cout << "Client established a remote connection over non-SSL" << std::endl;
                    ws->close(1000);
                    break;
                case 9:
                    std::cout << "Client established a remote connection over SSL" << std::endl;
                    ws->close(1000);
                    break;
                default:
                    std::cout << "FAILURE: " << ws->getUserData() << " should not connect!" << std::endl;
                    exit(-1);
            }
        });

        hub.onDisconnection([](uWS::WebSocket<uWS::CLIENT>* ws, int code, char* message, size_t length) {
            std::cout << "Client got disconnected with data: " << ws->getUserData() << ", code: " << code
                      << ", message: <" << std::string(message, length) << ">" << std::endl;
        });


        // TESTS THAT SHOULD PRODUCE AN ERROR
        auto user = (void*) 1; // should emit an invalid url error
        auto const INVALID_URI = "invalid URI";
        hub.connect(INVALID_URI, user);

        user = (void*) 2; // should emit an invalid protocol failure
        auto const UNRESOLVABLE_PROTOCOL = "valid://validButUnknown.address";
        hub.connect(UNRESOLVABLE_PROTOCOL, user);

        user = (void*) 3; // should emit a resolution failure
        auto const UNRESOLVABLE_URI = "ws://validButUnknown.address";
        hub.connect(UNRESOLVABLE_URI, user);

        int timeoutInMilliseconds = 10;
        std::map<std::string, std::string> extraHeaders = {};

        user = (void*) 4; // should emit an error on timeout of non-ssl ws
        auto const VALID_ADDRESS_WS = "ws://localhost:8080/connect-gateway";
        hub.connect(VALID_ADDRESS_WS, user, extraHeaders, timeoutInMilliseconds);

        user = (void*) 5; // should emit an error on timeout of ssl ws
        auto const VALID_ADDRESS_WSS = "wss://localhost:8080/connect-gateway";
        hub.connect(VALID_ADDRESS_WSS, user, extraHeaders, timeoutInMilliseconds);

        user = (void*) 6; // should emit an error on HTTP with no upgrade on non-ssl ws
        auto const GOOGLE_WS = "ws://google.com";
        hub.connect(GOOGLE_WS, user);

        user = (void*) 7; // should emit an error on HTTP with no upgrade on ssl ws
        auto const GOOGLE_WSS = "ws://google.com";
        hub.connect(GOOGLE_WSS, user);


        // THIS TEST SHOULD WORK NOW BUT STOP WORKING WHEN SSL IS CONFIGURED
        user = (void*) 8; // should connect with ws
        hub.connect(VALID_ADDRESS_WS, user);


        // THIS TEST IS NOT WORKING NOW, IT SHOULD BE THE ONLY ONE TO WORK WHEN SSL IS CONFIGURED
//    user = (void*) 9; // should connect with wss
//    hub.connect(VALID_ADDRESS_WSS, user);

        hub.run();
        return 0;
    }

    using TestFunctions::DataTestUtil;
    using WebSocket = uWS::WebSocket<uWS::CLIENT>;
    using WebSocketPointerPromise = std::promise<WebSocket*>;

    void connectAndStartWebSocket(uWS::Hub* hub, WebSocketPointerPromise* webSocketPointerPromise) {
        int length = 0;
        hub->onConnection([&webSocketPointerPromise, &length](WebSocket* ws, uWS::HttpRequest req) {
            std::cout << "Connected!" << std::endl;
            webSocketPointerPromise->set_value(ws);
        });

        hub->onMessage([](WebSocket* ws, char* message, size_t length, uWS::OpCode opCode) {
            std::cout << std::string(message, length) << std::endl;
            ws->send(message, length, opCode);
        });


        hub->onDisconnection([](WebSocket* ws, int code, char* message, size_t length) {
            std::cout << "Client got disconnected with data: " << ws->getUserData() << ", code: " << code
                      << ", message: <" << std::string(message, length) << ">" << std::endl;
        });

        hub->onError([](void* user) {
            long receivedCode = (long) user;
            std::vector<long> handledCodes = {};
            handleOtherErrors(handledCodes, receivedCode, coutMessageAndCrash, "An error has occured!");
        });


        auto const VALID_ADDRESS_WS = "ws://localhost:8080/connect-gateway";
        hub->connect(VALID_ADDRESS_WS);

        hub->run();
    }

    int testMessageSending() {
        uWS::Hub hub;
        WebSocketPointerPromise webSocketPointerPromise;

        auto webSocketConnectionThread = JoinableThread(&connectAndStartWebSocket, &hub, &webSocketPointerPromise);
        auto futureWebSocket = webSocketPointerPromise.get_future();
        futureWebSocket.wait();
        auto webSocket = futureWebSocket.get();

        auto numberOfSentMessages = 100;
        DataModel::SimpleData simpleData = DataTestUtil::createRandomSimpleData();
        std::string textToSend;
        for (int i = 0; i < numberOfSentMessages; ++i) {
            simpleData = DataTestUtil::createRandomSimpleData();
            textToSend = simpleData.toString();
            std::cout << "sending : " << textToSend << std::endl;
            webSocket->send(textToSend.c_str(), textToSend.size(), uWS::OpCode::TEXT);
        }

        webSocket->close();
        webSocketConnectionThread.join();

        return 0;
    }


}

int main() {
//    auto testOutput = ManualTest::testBasicUWSConnectionCalls();
    auto testOutput = ManualTest::testMessageSending();
    if (testOutput == 0) {

    }
    return 0;
}

#endif // SPIRITSENSORGATEWAY_UWSCONNECTIONTEST_CPP
