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

#include <test/mock/FrameSinkMock.h>
#include "spirit-sensor-gateway/sensor-communication/SensorCommunicator.hpp"
#include "spirit-sensor-gateway/sensor-communication/SensorCommunicationStrategy.hpp"
#include "spirit-sensor-gateway/server-communication/ServerCommunicationStrategy.hpp"
#include "spirit-sensor-gateway/message-translation/MessageTranslator.hpp"
#include "spirit-sensor-gateway/server-communication/ServerCommunicator.hpp"
#include "spirit-sensor-gateway/common/data-flow/DataSink.hpp"

using SensorAccessLinkElement::SensorCommunicator;
using SensorAccessLinkElement::MessageTranslator;
using SensorAccessLinkElement::ServerCommunicator;
using SensorCommunication::SensorCommunicationStrategy;
using MessageTranslation::MessageTranslationStrategy;
using ServerCommunication::ServerCommunicationStrategy;
using DataFlow::DataProcessingScheduler;
using DataFlow::DataSink;

template<class I, class O>
class SensorAccessLink {
protected:
    typedef DataProcessingScheduler<I, MessageTranslator<I, O>, 1> MessageTranslationScheduler;
    typedef DataProcessingScheduler<O, ServerCommunicator<O>, 1> ServerCommunicationScheduler;

public:
    explicit SensorAccessLink(SensorCommunicationStrategy<I>* sensorCommunicationStrategy,
                              MessageTranslationStrategy<I, O>* messageTranslationStrategy,
                              ServerCommunicationStrategy<O>* serverCommunicationStrategy) :
           serverCommunicator(serverCommunicationStrategy),
           messageTranslator(messageTranslationStrategy),
           sensorCommunicator(sensorCommunicationStrategy) {
  //         serverCommunicationScheduler(&serverCommunicator) {
  //         messageTranslationScheduler(&messageTranslator){
//            serverCommunicationScheduler(&serverCommunicator) {

        //messageTranslationStrategy->linkConsumer(&serverCommunicationScheduler);
        //sensorCommunicator.linkConsumer(&messageTranslationScheduler);
    };

    ~ SensorAccessLink() = default;

    void start() {
        //sensorCommunicator.start();
    };

    void terminateAndJoin(){
        //sensorCommunicator.terminateAndJoin();
        //messageTranslationScheduler.terminateAndJoin();
        //serverCommunicationScheduler.terminateAndJoin();
    }

private:
    SensorCommunicator<I> sensorCommunicator;
    MessageTranslator<I, O> messageTranslator;
    ServerCommunicator<O> serverCommunicator;
//    MessageTranslationScheduler messageTranslationScheduler;
 //   ServerCommunicationScheduler serverCommunicationScheduler;
};


#endif //SPIRITSENSORGATEWAY_SENSORACCESSLINK_H
