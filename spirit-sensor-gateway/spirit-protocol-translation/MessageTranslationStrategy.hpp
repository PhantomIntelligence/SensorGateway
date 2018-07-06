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
#ifndef SPIRITSENSORGATEWAY_MESSAGETRANSLATIONSTRATEGY_H
#define SPIRITSENSORGATEWAY_MESSAGETRANSLATIONSTRATEGY_H


#include "spirit-sensor-gateway/common/DataSource.hpp"

using namespace DataFlow;

template <class INPUT,class OUTPUT>
       class MessageTranslationStrategy: public DataSource<OUTPUT> {
        public:
            MessageTranslationStrategy() = default ;

            virtual ~MessageTranslationStrategy() = default;

            virtual void translateBasicMessage(INPUT* inputMessage) = 0;

       protected:

            OUTPUT* currentOutputMessage;
};

#endif //SPIRITSENSORGATEWAY_MESSAGETRANSLATIONSTRATEGY_H
