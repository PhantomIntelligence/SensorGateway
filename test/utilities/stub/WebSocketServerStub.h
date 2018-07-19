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
#ifndef SPIRITSENSORGATEWAY_WEBSOCKETSERVERSTUB_H
#define SPIRITSENSORGATEWAY_WEBSOCKETSERVERSTUB_H

#include "spirit-sensor-gateway/common/ConstantFunctionsDefinition.h"

namespace Stub {

    class WebSocketServerStub {

    public:

        WebSocketServerStub(std::string const& logFileName);

        ~WebSocketServerStub() noexcept;

        std::string const& getAddress();

        void run();

    private:

        static void startServerStub(std::FILE* logFile);

        JoinableThread webSocketServerStubThread;

        std::FILE* logFile;
    };
}

#endif //SPIRITSENSORGATEWAY_WEBSOCKETSERVERSTUB_H
