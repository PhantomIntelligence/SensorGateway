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

#ifndef SPIRITSENSORGATEWAY_DATAMODELFIXTURE_H
#define SPIRITSENSORGATEWAY_DATAMODELFIXTURE_H

#include <random>
#include <cassert>

#include "Data.h"

using ExampleDataModel::NativeComplement;
using ExampleDataModel::NativeData;
using ExampleDataModel::Parameters;

namespace TestFunctions {

    class DataTestUtil {

    public:

        /**
         * @brief This function initializes a NativeData with arbitrary values which are used in several tests.
         * @return A NativeData with specific values for several tests
         */
        static inline const NativeData initNativeData() {

            NativeSample distance;
            distance[0] = static_cast<uint16_t>(30);
            distance[1] = static_cast<uint16_t>(50);
            distance[2] = static_cast<uint16_t>(80);
            distance[3] = static_cast<uint16_t>(82);

            NativeSample velocity;
            velocity[0] = static_cast<uint16_t>(ONE_HUNDRED_CENTIMETERS_PER_SECONDS / 3);
            velocity[1] = static_cast<uint16_t>(2 * ONE_HUNDRED_CENTIMETERS_PER_SECONDS / 3);
            velocity[2] = static_cast<uint16_t>(ONE_HUNDRED_CENTIMETERS_PER_SECONDS);
            velocity[3] = static_cast<uint16_t>(ONE_HUNDRED_CENTIMETERS_PER_SECONDS);

            NativeSample intensity;
            intensity[0] = static_cast<uint16_t>(21);
            intensity[1] = static_cast<uint16_t>(40);
            intensity[2] = static_cast<uint16_t>(18);
            intensity[3] = static_cast<uint16_t>(6);

            NativeSample confidenceLevel;
            confidenceLevel[0] = static_cast<uint16_t>(99);
            confidenceLevel[1] = static_cast<uint16_t>(85);
            confidenceLevel[2] = static_cast<uint16_t>(65);
            confidenceLevel[3] = static_cast<uint16_t>(55);


            NativeComplement nativeComplement = NativeComplement{distance, velocity, intensity, confidenceLevel};
            NativeData nativeData = bundleNativeData(nativeComplement);
            return nativeData;

        }

        static const NativeComplement generateRandomNativeComplement() {

            auto numberOfDetections = drawNativeDetection();

            auto distance = drawRandomNativeSample(numberOfDetections, 20, 300);
            auto intensity = drawRandomNativeSample(numberOfDetections, 15, 50);
            auto velocity = drawRandomNativeSample(numberOfDetections, 0, ONE_HUNDRED_CENTIMETERS_PER_SECONDS);
            auto confidenceLevel = drawRandomNativeSample(numberOfDetections, 0, EXAMPLE_DATA_ARRAY_SIZE);

            NativeComplement nativeComplement = NativeComplement{distance, velocity, intensity, confidenceLevel};
            return nativeComplement;
        }

        /**
         * @brief Generates a Data containing only minimal information, used for tests.
         * @attention There is no warranty the Data will be unique.
         * @return A Data with random values
         */
        static const NativeData generateRandomNativeData() {

            auto numberOfDetections = drawNativeDetection();

            auto complement = generateRandomNativeComplement();

            NativeData nativeData = bundleNativeData(complement);
            return nativeData;
        }

        static const ExampleDataModel::ProcessedComplement initProcessedComplement() {
            Sample discreteDetection;
            discreteDetection[30] = static_cast<uint16_t>(21);
            discreteDetection[50] = static_cast<uint16_t>(40);
            discreteDetection[80] = static_cast<uint16_t>(18);
            discreteDetection[82] = static_cast<uint16_t>(42);
            uint16_t samplingDistance = 2;
            ExampleDataModel::ProcessedComplement complement{discreteDetection, samplingDistance};
            return complement;
        }

    private:


        static inline uint16_t drawNativeDetection() {
            std::default_random_engine randomEngine(std::random_device{}());
            std::uniform_int_distribution<uint16_t> detectionDistribution(0, EXAMPLE_DATA_NATIVE_SAMPLE_SIZE);
            uint16_t numberOfDetections = detectionDistribution(randomEngine);

            return numberOfDetections;
        }

        static NativeSample drawRandomNativeSample(uint16_t const& numberOfDetections,
                                                   uint16_t const& min,
                                                   uint16_t const& max) {

            assert(numberOfDetections <= EXAMPLE_DATA_NATIVE_SAMPLE_SIZE &&
                   "A NativeData Sample shall have exactly 4 values (See global typedef and local constant definition).");

            std::default_random_engine randomEngine(std::random_device{}());
            std::uniform_int_distribution<uint16_t> distribution(min, max);

            NativeSample sample;
            for (auto i = 0; i < numberOfDetections; ++i) {
                sample[i] = distribution(randomEngine);
            }
            for (int j = numberOfDetections; j < EXAMPLE_DATA_NATIVE_SAMPLE_SIZE; ++j) {
                sample[j] = 0;
            }
            return sample;
        }

        static const NativeData bundleNativeData(NativeComplement complement) {

            auto guardianParameters = Parameters(static_cast<uint16_t>(DEFAULT_FWHM),
                                                 static_cast<uint16_t>(DEFAULT_SAMPLING_TIME));

            NativeData nativeData(guardianParameters, complement);
            return nativeData;
        }

    };


}

#endif //SPIRITSENSORGATEWAY_DATAMODELFIXTURE_H
