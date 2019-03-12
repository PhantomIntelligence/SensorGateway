/**
	Copyright 2014-2019 Phantom Intelligence Inc.

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

#ifndef SENSORGATEWAY_MESSAGEASSERTION_HPP
#define SENSORGATEWAY_MESSAGEASSERTION_HPP

#include <gtest/gtest.h>
#include "sensor-gateway/common/data-structure/gateway/SensorMessage.hpp"
#include <typeinfo>

namespace Assert {

    template<typename T>
    static ::testing::AssertionResult sensorMessageHaveSameContent(T& expected, T& actual) {
        auto expectedPixels = expected.getPixels();
        auto actualPixels = actual.getPixels();

        auto sameSize = expectedPixels->size() == actualPixels->size();
        if (!sameSize) {
            return ::testing::AssertionFailure()
                    << "\n SensorMessages do not have the same content... \n"
                    << "\t | --> number of Pixel differs \n"
                    << "\t | --> expected size : " << expectedPixels->size() << "\n"
                    << "\t | --> actual size   : " << actualPixels->size() << "\n"
                    << "\t | -----------------------" << std::endl;
        }

        bool samePixelUntilNow = true;
        for (auto pixelId = 0u; pixelId < actualPixels->size() && samePixelUntilNow; ++pixelId) {
            auto expectedPixel = expectedPixels->at(pixelId);
            auto actualPixel = actualPixels->at(pixelId);
            samePixelUntilNow = expectedPixel == actualPixel && samePixelUntilNow;
            if (!samePixelUntilNow) {
                return ::testing::AssertionFailure()
                        << "\n SensorMessages do not have the same content... \n"
                        << "\t | --> Pixel # " << pixelId << " differs... \n"
                        << "\t | --> expected : "
                        << " { id     : " << expectedPixel.id << ",\n"
                        << "   tracks : " << "info not implemented for this message yet, was not needed till now... }\n"
                        << "\t | --> actual : "
                        << " { id     : " << actualPixel.id << ",\n"
                        << "   tracks : " << "info not implemented for this message yet, was not needed till now... }\n"
                        << "\t | -----------------------" << std::endl;
            }
        }
        return ::testing::AssertionSuccess();
    }

    template<typename T>
    static ::testing::AssertionResult sameSensorMessage(T& expected, T& actual) {
        if (expected == actual) {
            return ::testing::AssertionSuccess();
        }

        auto expectedMessageId = expected.messageId;
        auto actualMessageId = actual.messageId;

        if (expectedMessageId != actualMessageId) {
            return ::testing::AssertionFailure()
                    << "\n SensorMessages have ... \n"
                    << "\t | --> different message id \n"
                    << "\t | --> expected  : " << expectedMessageId << "\n"
                    << "\t | --> actual    : " << actualMessageId << "\n"
                    << "\t | -----------------------" << std::endl;
        }

        auto expectedSensorId = expected.sensorId;
        auto actualSensorId = actual.sensorId;

        if (expectedSensorId != actualSensorId) {
            return ::testing::AssertionFailure()
                    << "\n SensorMessages have ... \n"
                    << "\t | --> different sensor id \n"
                    << "\t | --> expected  : " << expectedSensorId << "\n"
                    << "\t | --> actual    : " << actualSensorId << "\n"
                    << "\t | -----------------------" << std::endl;
        }

        return sensorMessageHaveSameContent(expected, actual);
    }
}

#endif //SENSORGATEWAY_MESSAGEASSERTION_HPP
