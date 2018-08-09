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
#ifndef SENSORGATEWAY_MESSAGETRANSLATIONSTRATEGY_H
#define SENSORGATEWAY_MESSAGETRANSLATIONSTRATEGY_H

namespace MessageTranslation {

    template<class I, class O>
    class MessageTranslationStrategy : public DataFlow::DataSource<O> {

    protected:
        typedef I INPUT;
        typedef O OUTPUT;

    public:
        MessageTranslationStrategy() :
                currentOutputMessage(OUTPUT::returnDefaultData()) {}

        virtual ~MessageTranslationStrategy() noexcept = default;

        virtual void translateMessage(INPUT&& inputMessage) = 0;

    protected:

        OUTPUT currentOutputMessage;
    };
}

#endif //SENSORGATEWAY_MESSAGETRANSLATIONSTRATEGY_H
