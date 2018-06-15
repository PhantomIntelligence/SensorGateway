#ifndef SPIRIT_SENSOR_GATEWAY_MAINMENUTESTCONTEXT_CPP
#define SPIRIT_SENSOR_GATEWAY_MAINMENUTESTCONTEXT_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "spirit-sensor-gateway/communication-protocol-strategy/KvaserCanProtocolStrategy.cpp"
using CommunicationProtocolStrategy::KvaserCanProtocolStrategy;

class KvaserCanProtocolStrategyTest : public ::testing::Test {
};

//TEST_F(KvaserCanProtocolStrategyTest, given_someCanMessage_when_unwrappingMessageFromProtocol_then_returnsCorrespondingAwlMessage) {
//    KvaserCanProtocolStrategy kvaserCanProtocolStrategy;
//    AWLMessage actualAWLMessage = kvaserCanProtocolStrategy.readMessage();
//    ASSERT_EQ(actualAWLMessage, expectedAWLMessage);
//}

#endif //SPIRIT_SENSOR_GATEWAY_MAINMENUTESTCONTEXT_CPP