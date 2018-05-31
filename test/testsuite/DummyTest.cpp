#ifndef SPIRIT_SENSOR_GATEWAY_MAINMENUTESTCONTEXT_CPP
#define SPIRIT_SENSOR_GATEWAY_MAINMENUTESTCONTEXT_CPP

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "spirit-sensor-gateway/dummy/DummyClass.cpp"
using dummy::DummyClass;

class DummyTest : public ::testing::Test {
};

TEST_F(DummyTest, given_nothing_when_askedIfTrue_then_returnsFalse) {
    DummyClass dummy;

    auto isTrue = dummy.isTrue();

    ASSERT_FALSE(isTrue);
}

TEST_F(DummyTest, given_nothing_when_askedIfTrue_then_returnsTrue) {
    DummyClass dummy;

    auto isTrue = dummy.isTrue();

    ASSERT_TRUE(isTrue);
}

#endif //SPIRIT_SENSOR_GATEWAY_MAINMENUTESTCONTEXT_CPP
