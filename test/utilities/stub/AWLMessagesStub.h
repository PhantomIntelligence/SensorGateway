/**
	Copyright 2014, 2018 Phantom Intelligence Inc.
	Licensed under the Apache License, Version 2.0 (the License);
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at
		http://www.apache.org/licenses/LICENSE, 2.0
	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an AS IS BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
*/
#ifndef SPIRITSENSORGATEWAY_AWLMESSAGESSTUB_H
#define SPIRITSENSORGATEWAY_AWLMESSAGESSTUB_H

#include <vector>
#include <string>
#include "spirit-sensor-gateway/domain/AWLMessage.h"

namespace Stub {
    using DataFlow::AWLMessage;

    std::vector<AWLMessage> createAWLMessageStub() {
        std::vector<AWLMessage> awlMessages = {AWLMessage(10, 2188169, 8, {211, 55, 0, 11, 0, 0, 123, 0}),
                                               AWLMessage(11, 2188169, 8, {211, 55, 106, 0, 0, 0, 0, 0}),
                                               AWLMessage(10, 2188169, 8, {244, 55, 0, 12, 0, 0, 93, 0}),
                                               AWLMessage(11, 2188169, 8, {244, 55, 109, 0, 0, 0, 0, 0}),
                                               AWLMessage(10, 2188169, 8, {222, 55, 0, 13, 0, 0, 115, 0}),
                                               AWLMessage(11, 2188169, 8, {222, 55, 109, 0, 255, 255, 0, 0}),
                                               AWLMessage(10, 2188169, 8, {242, 55, 0, 14, 0, 0, 104, 0}),
                                               AWLMessage(11, 2188170, 8, {242, 55, 115, 0, 250, 255, 0, 0}),
                                               AWLMessage(10, 2188170, 8, {241, 55, 0, 15, 0, 0, 110, 0}),
                                               AWLMessage(11, 2188170, 8, {241, 55, 117, 0, 0, 0, 0, 0}),
                                               AWLMessage(9, 2188170, 8, {61, 253, 16, 0, 0, 0, 0, 0}),
                                               AWLMessage(10, 2188199, 8, {195, 55, 1, 0, 0, 0, 139, 0}),
                                               AWLMessage(11, 2188200, 8, {195, 55, 109, 0, 0, 0, 0, 0}),
                                               AWLMessage(10, 2188200, 8, {2, 0, 2, 1, 0, 0, 135, 0}),
                                               AWLMessage(11, 2188200, 8, {2, 0, 110, 0, 0, 0, 0, 0}),
                                               AWLMessage(10, 2188200, 8, {206, 55, 4, 2, 0, 0, 125, 0}),
                                               AWLMessage(11, 2188200, 8, {206, 55, 105, 0, 0, 0, 0, 0}),
                                               AWLMessage(10, 2188200, 8, {190, 55, 8, 3, 0, 0, 127, 0}),
                                               AWLMessage(11, 2188200, 8, {190, 55, 106, 0, 1, 0, 0, 0}),
                                               AWLMessage(10, 2188200, 8, {221, 55, 16, 4, 0, 0, 122, 0}),
                                               AWLMessage(11, 2188201, 8, {221, 55, 104, 0, 0, 0, 0, 0}),
                                               AWLMessage(10, 2188201, 8, {192, 55, 32, 5, 0, 0, 120, 0}),
                                               AWLMessage(11, 2188201, 8, {192, 55, 108, 0, 0, 0, 0, 0}),
                                               AWLMessage(10, 2188201, 8, {236, 55, 64, 6, 0, 0, 114, 0}),
                                               AWLMessage(11, 2188201, 8, {236, 55, 114, 0, 0, 0, 0, 0}),
                                               AWLMessage(10, 2188201, 8, {240, 55, 128, 7, 0, 0, 110, 0}),
                                               AWLMessage(11, 2188201, 8, {240, 55, 117, 0, 1, 0, 0, 0}),
                                               AWLMessage(10, 2188201, 8, {204, 55, 0, 8, 0, 0, 126, 0}),
                                               AWLMessage(11, 2188201, 8, {204, 55, 111, 0, 0, 0, 0}),
                                               AWLMessage(10, 2188202, 8, {209, 55, 0, 9, 0, 0, 124, 0}),
                                               AWLMessage(11, 2188202, 8, {209, 55, 111, 0, 0, 0, 0, 0}),
                                               AWLMessage(10, 2188202, 8, {210, 55, 0, 10, 0, 0, 125, 0}),
                                               AWLMessage(11, 2188202, 8, {210, 55, 107, 0, 0, 0, 0, 0}),
                                               AWLMessage(10, 2188202, 8, {211, 55, 0, 11, 0, 0, 123, 0}),
                                               AWLMessage(11, 2188202, 8, {211, 55, 106, 0, 0, 0, 0, 0}),
                                               AWLMessage(10, 2188202, 8, {244, 55, 0, 12, 0, 0, 91, 0}),
                                               AWLMessage(11, 2188202, 8, {244, 55, 109, 0, 1, 0, 0, 0}),
                                               AWLMessage(10, 2188203, 8, {222, 55, 0, 13, 0, 0, 114, 0}),
                                               AWLMessage(11, 2188203, 8, {222, 55, 109, 0, 0, 0, 0, 0}),
                                               AWLMessage(10, 2188203, 8, {242, 55, 0, 14, 0, 0, 109, 0}),
                                               AWLMessage(11, 2188203, 8, {242, 55, 116, 0, 254, 255, 0, 0}),
                                               AWLMessage(10, 2188203, 8, {241, 55, 0, 15, 0, 0, 109, 0}),
                                               AWLMessage(11, 2188203, 8, {241, 55, 117, 0, 0, 0, 0, 0}),
                                               AWLMessage(9, 2188203, 8, {62, 253, 16, 0, 0, 0, 0, 0})
        };
        return awlMessages;
    }

}

#endif //SPIRITSENSORGATEWAY_AWLMESSAGESSTUB_H
