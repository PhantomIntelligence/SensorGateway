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

#include <vector>
#include <string>
#include "spirit-sensor-gateway/domain/AWLMessage.h"


//TODO: use typedef to define each type of the AWL attributes
//TODO: create class AWLMessage if needed, instead of using this function
AWLMessage createAWLMessage(int id, int length, long timestamp, std::vector<int> data){
    AWLMessage awlMessage = {};
    awlMessage.id = (int64_t)id;
    awlMessage.timestamp = (uint64_t)timestamp;
    awlMessage.length = (uint32_t)length;
    for (auto dataPosition = 0; dataPosition < MAX_NUMBER_OF_DATA_IN_AWL_MESSAGE; dataPosition++) {
        awlMessage.data[dataPosition] = (unsigned char)data[dataPosition];
    }
    return awlMessage;
}

std::vector<AWLMessage> awlMessagesFixture = {createAWLMessage(10, 8, 2188169, {211, 55, 0, 11, 0, 0, 123, 0}),
                                              createAWLMessage(11, 8, 2188169, {211, 55, 106 ,0, 0, 0, 0, 0}), 
                                              createAWLMessage(10, 8, 2188169, {244, 55, 0, 12, 0, 0, 93, 0}),
                                              createAWLMessage(11, 8, 2188169, {244, 55, 109, 0, 0, 0, 0, 0}),
                                              createAWLMessage(10, 8, 2188169, {222, 55, 0, 13, 0, 0, 115, 0}),
                                              createAWLMessage(11, 8, 2188169, {222, 55, 109, 0, 255, 255, 0, 0}),
                                              createAWLMessage(10, 8, 2188169, {242, 55, 0, 14, 0, 0, 104, 0}),
                                              createAWLMessage(11, 8, 2188170, {242, 55, 115, 0, 250, 255, 0, 0}),
                                              createAWLMessage(10, 8, 2188170, {241, 55, 0, 15, 0, 0, 110, 0}),
                                              createAWLMessage(11, 8, 2188170, {241, 55, 117, 0, 0, 0, 0, 0}), 
                                              createAWLMessage(9, 8, 2188170, {61, 253, 16, 0, 0, 0, 0, 0}), 
                                              createAWLMessage(10, 8, 2188199, {195, 55, 1, 0, 0, 0, 139, 0}), 
                                              createAWLMessage(11, 8, 2188200, {195, 55, 109, 0, 0, 0, 0, 0}), 
                                              createAWLMessage(10, 8, 2188200, {2, 0, 2, 1, 0, 0, 135, 0}), 
                                              createAWLMessage(11, 8, 2188200, {2, 0, 110, 0, 0, 0, 0, 0}), 
                                              createAWLMessage(10, 8, 2188200, {206, 55, 4, 2, 0, 0, 125, 0}), 
                                              createAWLMessage(11, 8, 2188200, {206, 55, 105, 0, 0, 0, 0, 0}), 
                                              createAWLMessage(10, 8, 2188200, {190, 55, 8, 3, 0, 0, 127, 0}), 
                                              createAWLMessage(11, 8, 2188200, {190, 55, 106, 0, 1, 0, 0, 0}), 
                                              createAWLMessage(10, 8, 2188200, {221, 55, 16, 4, 0, 0, 122, 0}), 
                                              createAWLMessage(11, 8, 2188201, {221, 55, 104, 0, 0, 0, 0, 0}), 
                                              createAWLMessage(10, 8, 2188201, {192, 55, 32, 5, 0, 0, 120, 0}), 
                                              createAWLMessage(11, 8, 2188201, {192, 55, 108, 0, 0, 0, 0, 0}), 
                                              createAWLMessage(10, 8, 2188201, {236, 55, 64, 6, 0, 0, 114, 0}), 
                                              createAWLMessage(11, 8, 2188201, {236, 55, 114, 0, 0, 0, 0, 0}), 
                                              createAWLMessage(10, 8, 2188201, {240, 55, 128, 7, 0, 0, 110, 0}), 
                                              createAWLMessage(11, 8, 2188201, {240, 55, 117, 0, 1, 0, 0, 0}), 
                                              createAWLMessage(10, 8, 2188201, {204, 55, 0, 8, 0, 0, 126, 0}),
                                              createAWLMessage(11, 8, 2188201, {204, 55, 111, 0, 0, 0, 0}),
                                              createAWLMessage(10, 8, 2188202, {209, 55, 0, 9, 0, 0, 124, 0}),
                                              createAWLMessage(11, 8, 2188202, {209, 55, 111, 0, 0, 0, 0, 0}),
                                              createAWLMessage(10, 8, 2188202, {210, 55, 0, 10, 0, 0, 125, 0}),
                                              createAWLMessage(11, 8, 2188202, {210, 55, 107, 0, 0, 0, 0, 0}),
                                              createAWLMessage(10, 8, 2188202, {211, 55, 0, 11, 0, 0, 123, 0}),
                                              createAWLMessage(11, 8, 2188202, {211, 55, 106, 0, 0, 0, 0, 0}),
                                              createAWLMessage(10, 8, 2188202, {244, 55, 0, 12, 0, 0, 91, 0}),
                                              createAWLMessage(11, 8, 2188202, {244, 55, 109, 0, 1, 0, 0, 0}),
                                              createAWLMessage(10, 8, 2188203, {222, 55, 0, 13, 0, 0, 114, 0}),
                                              createAWLMessage(11, 8, 2188203, {222, 55, 109, 0, 0, 0, 0, 0}),
                                              createAWLMessage(10, 8, 2188203, {242, 55, 0, 14, 0, 0, 109, 0}),
                                              createAWLMessage(11, 8, 2188203, {242, 55, 116, 0, 254, 255, 0, 0}),
                                              createAWLMessage(10, 8, 2188203, {241, 55, 0, 15, 0, 0, 109, 0}),
                                              createAWLMessage(11, 8, 2188203, {241, 55, 117, 0, 0, 0, 0, 0}),
                                              createAWLMessage(9, 8, 2188203, {62, 253, 16, 0, 0, 0, 0, 0})};
