

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "spirit-sensor-gateway/spirit-protocol-translation/AWLMessageTranslator.h"

class AWLMessageTranslatorTest :    public ::testing::Test{

    AWLMessageTranslator* awlMessageTranslatorTest =  new AWLMessageTranslator();

};



TEST_F(AWLMessageTranslatorTest,given_anAwlMessageWithAnIdOfNine_when_translatingTheAwlMessage_then_setTheCorrectFrameIdAndSystemIdOfTheFrame){
    
}