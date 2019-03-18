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

    namespace Details {
        constexpr int const PADDING_WIDTH = 4;
        constexpr int const NAME_WIDTH = 24;
        constexpr int const EXPECTED_VALUE_WIDTH = 12;
        constexpr int const ACTUAL_VALUE_WIDTH = 12;

        std::string const NAME_VALUE_SPACER(NAME_WIDTH - 1, ' ');
        std::string const EXPECTED_VALUE_SPACER(EXPECTED_VALUE_WIDTH - 1, ' ');
        std::string const ACTUAL_VALUE_SPACER(ACTUAL_VALUE_WIDTH - 1, ' ');

        constexpr int const COLUMN_SIZE = 4;
        constexpr int const TOTAL_WIDTH = PADDING_WIDTH +
                                          NAME_WIDTH +
                                          EXPECTED_VALUE_WIDTH +
                                          ACTUAL_VALUE_WIDTH +
                                          COLUMN_SIZE;
        using DifferentialPrinting = std::array<std::string, COLUMN_SIZE>;
        using MessageDifferencesForPrinting = std::list<DifferentialPrinting>;
        DifferentialPrinting const HEADER = {{NAME_VALUE_SPACER, "EXPECTED  ", " ACTUAL  "}};
        DifferentialPrinting const SPACER = {{NAME_VALUE_SPACER, EXPECTED_VALUE_SPACER, ACTUAL_VALUE_SPACER}};
        DifferentialPrinting const BREAK = {{std::string(NAME_WIDTH - 1, '~'),
                                                    std::string(EXPECTED_VALUE_WIDTH - 1, '~'),
                                                    std::string(ACTUAL_VALUE_WIDTH - 1, '~')}};

        template<typename T>
        static void
        addDiff(MessageDifferencesForPrinting* messageDifferencesForPrinting, std::string const& differenceName,
                T&& expected, T&& actual) {
            DifferentialPrinting newDifference = {{(differenceName + " "),
                                                          (std::to_string(expected) + "  "),
                                                          (std::to_string(actual) + "  ")}};
            messageDifferencesForPrinting->emplace_back(newDifference);
        }

        template<typename T>
        static void
        addDiffIfDifferent(MessageDifferencesForPrinting* differences, std::string const& differenceName, T& expected,
                           T& actual) noexcept {
            if (expected != actual) {
                Details::addDiff(differences, differenceName, expected, actual);
            }
        }


        static void
        addDiffTitle(MessageDifferencesForPrinting* messageDifferencesForPrinting, std::string const& differenceName) {
            DifferentialPrinting newDifference = {{(differenceName + " "), EXPECTED_VALUE_SPACER, ACTUAL_VALUE_SPACER}};
            messageDifferencesForPrinting->emplace_back(newDifference);
        }

        static void
        addSpacer(MessageDifferencesForPrinting* messageDifferencesForPrinting) {
            messageDifferencesForPrinting->push_back(SPACER);
        }

        static void
        addBreak(MessageDifferencesForPrinting* messageDifferencesForPrinting) {
            messageDifferencesForPrinting->push_back(BREAK);
        }

        static auto printLine = [](std::ostringstream* message, DifferentialPrinting const& lineContent) {
            auto const& name = std::get<0>(lineContent);
            auto const& expectedValue = std::get<1>(lineContent);
            auto const& actualValue = std::get<2>(lineContent);

            std::string padding(PADDING_WIDTH, ' ');
            (*message) << padding << '|';

            message->width(NAME_WIDTH);
            (*message) << name << '|';

            message->width(EXPECTED_VALUE_WIDTH);
            (*message) << (" " + expectedValue) << '|';

            message->width(ACTUAL_VALUE_WIDTH);
            (*message) << (" " + actualValue) << '|';

            (*message) << '\n';
            return message->str();
        };

        static auto printBreak = [](std::ostringstream* message, char const& breakingChar) {
            message->width(Details::TOTAL_WIDTH);
            message->fill(breakingChar);
            (*message) << breakingChar << "\n";
            message->fill(' ');
        };

        static auto printGlobalBreak = [](std::ostringstream* message) {
            printBreak(message, '#');
        };

        static auto printLineBreak = [](std::ostringstream* message) {
            printBreak(message, '~');
        };
    }

    static std::string completeMessage(std::ostringstream* message) {
        Details::printGlobalBreak(message);
        Details::printGlobalBreak(message);
        return message->str();
    }

    static std::string
    formatMessage(std::ostringstream* message, Details::MessageDifferencesForPrinting* remainingDifferences) {
        if (remainingDifferences->empty()) {
            return completeMessage(message);
        }

        auto const& difference = remainingDifferences->front();

        Details::printLine(message, difference);

        remainingDifferences->pop_front();
        return formatMessage(message, remainingDifferences);
    }

    static ::testing::AssertionResult
    formatFailureMessage(Details::MessageDifferencesForPrinting* messageDifferencesForPrinting) {

        std::ostringstream message("\n\n", std::ios_base::ate);
        Details::printGlobalBreak(&message);
        Details::printGlobalBreak(&message);
        Details::printLine(&message, Details::HEADER);
        Details::printGlobalBreak(&message);

        return ::testing::AssertionFailure()
                << formatMessage(&message, messageDifferencesForPrinting);
    }

    template<typename T>
    static ::testing::AssertionResult sensorMessageHaveSameContent(T& expected, T& actual) {
        Details::MessageDifferencesForPrinting messageDifferencesForPrinting;

        auto expectedPixels = expected.getPixels();
        auto actualPixels = actual.getPixels();

        auto sameSize = expectedPixels->size() == actualPixels->size();
        if (!sameSize) {
            Details::addDiff(&messageDifferencesForPrinting, "Number of pixels", expectedPixels->size(),
                             actualPixels->size());
            return formatFailureMessage(&messageDifferencesForPrinting);
        }

        bool samePixels = true;
        using PixelType = typename std::remove_reference<decltype(std::get<0>(*expectedPixels))>::type;
        using PixelId= typename PixelType::IdType;
        using DifferentPixel = std::tuple<PixelId, PixelType const&, PixelType const&>;
        using DifferentPixels = std::vector<DifferentPixel>;

        DifferentPixels differentPixels;

        for (auto pixelId = 0u; pixelId < actualPixels->size(); ++pixelId) {
            auto expectedPixel = expectedPixels->at(pixelId);
            auto actualPixel = actualPixels->at(pixelId);
            if (expectedPixel != actualPixel) {
                samePixels = false;
                differentPixels.emplace_back(std::make_tuple(pixelId, expectedPixel, actualPixel));
            }
        }


        if (!samePixels) {
            PixelId orderId;
            PixelType expectedPixel;
            PixelType actualPixel;
            for (auto diff : differentPixels) {
                std::tie(orderId, expectedPixel, actualPixel) = diff;
                Details::addBreak(&messageDifferencesForPrinting);
                Details::addDiffTitle(&messageDifferencesForPrinting, ("Pixel # " + std::to_string(orderId) + ": "));

                Details::addDiffIfDifferent(&messageDifferencesForPrinting, " id: ", expectedPixel.id, actualPixel.id);

                auto expectedNumberOfTracks = expectedPixel.getCurrentNumberOfTracksInPixel();
                auto actualNumberOfTracks = actualPixel.getCurrentNumberOfTracksInPixel();
                Details::addDiffIfDifferent(&messageDifferencesForPrinting, "Number of tracks: ",
                                            expectedNumberOfTracks, actualNumberOfTracks);

                auto expectedTracks = expectedPixel.getTracks();
                auto actualTracks = actualPixel.getTracks();

                if ((*expectedTracks) != (*actualTracks)) {
                    Details::addDiffTitle(&messageDifferencesForPrinting, "Tracks... ");

                    auto expectedTrackSize = expectedTracks->size();
                    auto actualTrackSize = actualTracks->size();

                    Details::addDiffIfDifferent(&messageDifferencesForPrinting, " size: ", expectedTrackSize, actualTrackSize);

                    if (expectedTrackSize == actualTrackSize) {

                        for (auto trackIndex = 0u; trackIndex < expectedTrackSize; ++trackIndex) {
                            auto expectedTrack = expectedTracks->at(trackIndex);
                            auto actualTrack = actualTracks->at(trackIndex);

                            if (expectedTrack != actualTrack) {
                                Details::addDiffTitle(&messageDifferencesForPrinting, ("Track # " + std::to_string(trackIndex) + ": "));
                                Details::addDiffIfDifferent(&messageDifferencesForPrinting, " id: ", expectedTrack.id, actualTrack.id);
                                Details::addDiffIfDifferent(&messageDifferencesForPrinting, " distance: ",
                                                            expectedTrack.distance, actualTrack.distance);
                                Details::addDiffIfDifferent(&messageDifferencesForPrinting, " intensity: ",
                                                            expectedTrack.intensity, actualTrack.intensity);
                                Details::addDiffIfDifferent(&messageDifferencesForPrinting, " speed: ",
                                                            expectedTrack.speed,
                                                            actualTrack.speed);
                                Details::addDiffIfDifferent(&messageDifferencesForPrinting, " acceleration: ",
                                                            expectedTrack.acceleration, actualTrack.acceleration);
                                Details::addDiffIfDifferent(&messageDifferencesForPrinting, " confidence level: ",
                                                            expectedTrack.confidenceLevel, actualTrack.confidenceLevel);
                            }
                        }
                    }
                }
                Details::addBreak(&messageDifferencesForPrinting);
            }
            return formatFailureMessage(&messageDifferencesForPrinting);
        }
        return ::testing::AssertionSuccess();
    }

    template<typename T>
    static ::testing::AssertionResult sameSensorMessage(T& expected, T& actual) {
        if (expected == actual) {
            return ::testing::AssertionSuccess();
        }

        bool sameMessage = true;
        Details::MessageDifferencesForPrinting messageDifferencesForPrinting;

        auto expectedMessageId = expected.messageId;
        auto actualMessageId = actual.messageId;

        Details::addDiffTitle(&messageDifferencesForPrinting, "Sensor Message ");
        if (expectedMessageId != actualMessageId) {
            Details::addDiff(&messageDifferencesForPrinting, "Message id:", expectedMessageId, actualMessageId);
            sameMessage = false;
        }

        auto expectedSensorId = expected.sensorId;
        auto actualSensorId = actual.sensorId;

        if (expectedSensorId != actualSensorId) {
            Details::addDiff(&messageDifferencesForPrinting, "Sensor id:", expectedSensorId, actualSensorId);
            sameMessage = false;
        }

        if (!sameMessage) {
            return formatFailureMessage(&messageDifferencesForPrinting);
        }

        return sensorMessageHaveSameContent(expected, actual);
    }
}

#endif //SENSORGATEWAY_MESSAGEASSERTION_HPP
