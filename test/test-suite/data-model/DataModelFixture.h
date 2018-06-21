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
            distance[0] = static_cast<uData16>(30);
            distance[1] = static_cast<uData16>(50);
            distance[2] = static_cast<uData16>(80);
            distance[3] = static_cast<uData16>(82);

            NativeSample velocity;
            velocity[0] = static_cast<uData16>(ONE_HUNDRED_CENTIMETERS_PER_SECONDS / 3);
            velocity[1] = static_cast<uData16>(2 * ONE_HUNDRED_CENTIMETERS_PER_SECONDS / 3);
            velocity[2] = static_cast<uData16>(ONE_HUNDRED_CENTIMETERS_PER_SECONDS);
            velocity[3] = static_cast<uData16>(ONE_HUNDRED_CENTIMETERS_PER_SECONDS);

            NativeSample intensity;
            intensity[0] = static_cast<uData16>(21);
            intensity[1] = static_cast<uData16>(40);
            intensity[2] = static_cast<uData16>(18);
            intensity[3] = static_cast<uData16>(6);

            NativeSample confidenceLevel;
            confidenceLevel[0] = static_cast<uData16>(99);
            confidenceLevel[1] = static_cast<uData16>(85);
            confidenceLevel[2] = static_cast<uData16>(65);
            confidenceLevel[3] = static_cast<uData16>(55);


            NativeComplement nativeComplement = NativeComplement{distance, velocity, intensity, confidenceLevel};
            NativeData nativeData = bundleNativeData(nativeComplement);
            return nativeData;

        }

        static const NativeComplement generateRandomNativeComplement() {

            auto numberOfDetection = drawNativeDetection();

            auto distance = drawRandomNativeSample(numberOfDetection, 20, 300);
            auto intensity = drawRandomNativeSample(numberOfDetection, 15, 50);
            auto velocity = drawRandomNativeSample(numberOfDetection, 0, ONE_HUNDRED_CENTIMETERS_PER_SECONDS);
            auto confidenceLevel = drawRandomNativeSample(numberOfDetection, 0, EXAMPLE_DATA_ARRAY_SIZE);

            NativeComplement nativeComplement = NativeComplement{distance, velocity, intensity, confidenceLevel};
            return nativeComplement;
        }

        /**
         * @brief Generates a Data containing only minimal information, used for tests.
         * @attention There is no warranty the Data will be unique.
         * @return A Data with random values
         */
        static const NativeData generateRandomNativeData() {

            auto numberOfDetection = drawNativeDetection();

            auto complement = generateRandomNativeComplement();

            NativeData nativeData = bundleNativeData(complement);
            return nativeData;
        }

        static const ExampleDataModel::ProcessedComplement initProcessedComplement() {
            Sample discreteDetection;
            discreteDetection[30] = static_cast<uData16>(21);
            discreteDetection[50] = static_cast<uData16>(40);
            discreteDetection[80] = static_cast<uData16>(18);
            discreteDetection[82] = static_cast<uData16>(42);
            uData16 samplingDistance = 2;
            ExampleDataModel::ProcessedComplement complement{discreteDetection, samplingDistance};
            return complement;
        }

    private:


        static inline uData16 drawNativeDetection() {
            std::default_random_engine randomEngine(std::random_device{}());
            std::uniform_int_distribution<uData16> detectionDistribution(0, EXAMPLE_DATA_NATIVE_SAMPLE_SIZE);
            uData16 numberOfDetection = detectionDistribution(randomEngine);

            return numberOfDetection;
        }

        static NativeSample drawRandomNativeSample(uData16 const& numberOfDetection,
                                                   uData16 const& min,
                                                   uData16 const& max) {

            assert(numberOfDetection <= EXAMPLE_DATA_NATIVE_SAMPLE_SIZE &&
                   "A NativeData Sample shall have exactly 4 values (See global typedef and local constant definition).");

            std::default_random_engine randomEngine(std::random_device{}());
            std::uniform_int_distribution<uData16> distribution(min, max);

            NativeSample sample;
            for (auto i = 0; i < numberOfDetection; ++i) {
                sample[i] = distribution(randomEngine);
            }
            for (int j = numberOfDetection; j < EXAMPLE_DATA_NATIVE_SAMPLE_SIZE; ++j) {
                sample[j] = 0;
            }
            return sample;
        }

        static const NativeData bundleNativeData(NativeComplement complement) {

            auto guardianParameters = Parameters(static_cast<uData16>(DEFAULT_FWHM),
                                                 static_cast<uData16>(DEFAULT_SAMPLING_TIME));

            NativeData nativeData(guardianParameters, complement);
            return nativeData;
        }

    };


}

#endif //SPIRITSENSORGATEWAY_DATAMODELFIXTURE_H
