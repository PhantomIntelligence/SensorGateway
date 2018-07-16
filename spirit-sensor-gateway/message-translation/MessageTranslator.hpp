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
#ifndef SPIRITSENSORGATEWAY_MESSAGETRANSLATOR_H
#define SPIRITSENSORGATEWAY_MESSAGETRANSLATOR_H

#include "spirit-sensor-gateway/common/data-flow/DataSink.hpp"
#include "MessageTranslationStrategy.hpp"

namespace SensorAccessLinkElement {

    template<class I, class O>
    class MessageTranslator : public DataFlow::DataSink<I> {

    protected:
        typedef I INPUT;
        typedef O OUTPUT;

    public:
        explicit MessageTranslator(MessageTranslation::MessageTranslationStrategy<INPUT, OUTPUT>* messageTranslationStrategy) :
                messageTranslationStrategy(messageTranslationStrategy) {};

        ~MessageTranslator(){

        }

        void consume(I&& inputMessage) override {
            messageTranslationStrategy->translateBasicMessage(std::move(inputMessage));
        };

    private:
        MessageTranslation::MessageTranslationStrategy<INPUT, OUTPUT>* messageTranslationStrategy;

    };
}

#endif //SPIRITSENSORGATEWAY_MESSAGETRANSLATOR_H
