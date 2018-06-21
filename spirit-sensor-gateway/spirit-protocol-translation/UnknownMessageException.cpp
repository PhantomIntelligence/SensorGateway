#include "UnknownMessageException.h"

    UnknownMessageException::UnknownMessageException(AWLMessage *awlMessage) {

    };

    std::string UnknownMessageException::getUnknownMessageException (){
        return unknownMessageException;
    };
