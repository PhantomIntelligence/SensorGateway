/**
	Copyright 2018 Phantom Intelligence Inc.

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
#include "src/ChatClient.cpp"
#include "src/SecureStreaming/SpdLogWriter.cpp"
#include "src/SecureStreaming/Logger.cpp"

std::string extractServerUri(std::map<std::string, std::string> runtimeArgs);

Streaming::SpdLogWriter spdLogWriter("ChatClient");
Streaming::Logger demoLogger(&spdLogWriter);
Streaming::KeyPairGenerator keyPairGenerator(&demoLogger);
Streaming::Codec codec(&demoLogger);

int main(int argc, char* argv[]) {
    Logger logger();
    std::map<std::string, std::string> runtimeArgs = Demo::fetchRuntimeArgs(argc, argv);
    std::string serverUri = extractServerUri(runtimeArgs);
    std::string  input;

    Demo::ChatClient client(serverUri, &keyPairGenerator, &codec);
    try {
        client.start();
        while (client.isRunning()){
            std::getline(std::cin, input);
            client.processUserInput(input);
        }
        client.safeClose();

    } catch (std::runtime_error& error) {
        std::cout << std::endl << error.what() << std::endl;
    }

    std::cout << std::endl << "Demo client terminated" << std::endl;

    return 0;
}

std::string extractServerUri(std::map<std::string, std::string> runtimeArgs) {
    auto serverAddress = runtimeArgs["-s"];
    auto port = runtimeArgs["-p"];

    if (serverAddress.empty()) {serverAddress = Demo::DEFAULT_SERVER_ADDRESS;}
    if (port.empty()) {port = std::to_string(Demo::DEFAULT_SERVER_PORT);}

    std::string serverUri = "ws://" + serverAddress + ":" + port;

    return serverUri;
}

