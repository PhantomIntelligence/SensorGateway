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

#ifndef SPIRITSENSORGATEWAY_SENSORACCESSLINK_H
#define SPIRITSENSORGATEWAY_SENSORACCESSLINK_H

#include "spirit-sensor-gateway/sensor-communication/SensorCommunicator.hpp"
#include "spirit-sensor-gateway/message-translation/MessageTranslator.hpp"

using DataFlow::DataProcessingScheduler;
using SensorAccessLinkElement::SensorCommunicator;
using SensorAccessLinkElement::MessageTranslator;

template<class I, class O>
class SensorAccessLink {

protected:

    typedef MessageTranslator<I, O> MessageTranslator;
    typedef SensorCommunicator<I> SensorCommunicator;
    typedef DataProcessingScheduler<I, MessageTranslator, 1> TranslationScheduler;

public:
    explicit SensorAccessLink(SensorCommunicator* sensorCommunicator, MessageTranslator* messageTranslator) :
            sensorCommunicator(sensorCommunicator), messageTranslator(messageTranslator), translationScheduler() {
        translationScheduler = TranslationScheduler(messageTranslator);
        sensorCommunicator->linkConsumer(&translationScheduler);
    };

    ~ SensorAccessLink() = default;

    void start() {
        sensorCommunicator->start();
    };

    void terminateAndJoin(){
        sensorCommunicator->terminateAndJoin();
        translationScheduler.terminateAndJoin();
    }

    void getOutputMessages(){
    }


private:
    SensorCommunicator* sensorCommunicator;
    MessageTranslator* messageTranslator;
    TranslationScheduler translationScheduler;

};


#endif //SPIRITSENSORGATEWAY_SENSORACCESSLINK_H
