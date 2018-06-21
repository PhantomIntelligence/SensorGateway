#include "UnknownMessageException.h"

    UnknownMessageException::UnknownMessageException(AWLMessage *awlMessage) {
        message =  "An unknwon message was received";
        message += ("ID : %",awlMessage->id);
        message += ("Message length : % ",awlMessage->length);
        message += ("Message timestamp: % ",awlMessage->timestamp);
        message += ("Message data: % % % % % % % %",awlMessage->data[0],awlMessage->data[1],awlMessage->data[2],awlMessage->data[3],awlMessage->data[4],awlMessage->data[5],awlMessage->data[6],awlMessage->data[7]);
    };

    std::string UnknownMessageException::getMessage(){
        return message;
    };
