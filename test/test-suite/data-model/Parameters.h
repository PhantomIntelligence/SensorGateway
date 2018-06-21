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


#ifndef SPIRITSENSORGATEWAY_PARAMETERS_H
#define SPIRITSENSORGATEWAY_PARAMETERS_H

#include "DataModelTypeDefinition.h"

/**
 * @brief The Data Representation (DR) classes contained in this namespace form an basic implementation guide for DR classes in Spectre.
 * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
 * @see Data.h for more details
 */
namespace ExampleDataModel {

/**
 * @brief Information stored on the physical sensor
 * @attention: These info should stay about the same through all of the program's execution. They shall only be changed
 *       by changing the sensor's parameters.
 * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
 * @see Data.h for more details
 */
    class Parameters {

    public:

        /**
         * @brief Default constructor of the Parameters example Data Representation (DR) class.
         * @param FWHM Full Width at Half Maximum (in nanoseconds)
         * @param samplingTime Time interval used to collect a sample (in nanoseconds)
         * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
         * @see Data.h for more details
         */
        Parameters(uData16 const& FWHM, uData16 const& samplingTime);

        /**
         * @brief Default destructor of the Data example Data Representation (DR) class.
         * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
         * @see Data.h for more details
         */
        ~Parameters() noexcept = default;

        /**
         * @brief Copy constructor for the Parameters example Data Representation (DR) class.
         * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
         * @see Data.h for more details
         */
        Parameters(Parameters const& other);

        /**
         * @brief Move constructor for the Parameters example Data Representation (DR) class.
         * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
         * @see Data.h for more details
         */
        Parameters(Parameters&& other) noexcept;

        /**
         * @brief Copy assignment operator for the Parameters example Data Representation (DR) class.
         * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
         * @see Data.h for more details
         */
        Parameters& operator=(Parameters other)& {
            swap(*this, other);
            return *this;
        }

        /**
         * @brief Move assignment operator for the Parameters example Data Representation (DR) class.
         * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
         * @see Data.h for more details
         */
        Parameters& operator=(Parameters&& other)& noexcept {
            Parameters temporary(std::move(other));
            swap(*this, temporary);
            return *this;
        }

        /**
         * @brief Non throwing swap used in the provides the Strong Exception Guarantee for the different assignment and construction operations it is used in.
         * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
         * @see Data.h for more details
         */
        void swap(Parameters& current, Parameters& other) noexcept;

        /**
         * @brief Example for a getter signature for the FWHM of the Parameters example Data Representation (DR) class.
         * @return Full Width at Half Maximum (in nanoseconds)
         * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
         * @see Data.h for more details
         */
        uData16 const& getFWHM() const;

        /**
         * @brief Example for a getter signature for the Sampling Time of the Parameters example Data Representation (DR) class.
         * @return Time interval used to collect a sample (in nanoseconds)
         * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
         * @see Data.h for more details
         */
        uData16 const& getSamplingTime() const;

        /**
         * @brief Equality operator for a Data Representation (DR) class.
         * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
         * @see Data.h for more details
         */
        Boolean operator==(Parameters const& other) const;

        /**
         * @brief Inequality operator for a Data Representation (DR) class.
         * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
         * @see Data.h for more details
         */
        Boolean operator!=(Parameters const& other) const;

        /**
         * @brief A mandatory function for all Data Representation (DR) class.
         * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
         * @see Data.h for more details
         */
        Parameters static const returnDefaultData() noexcept;

    private:
        /**
         *
         * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
         * @see Data.h for more details
         */
        uData16 FWHM;

        /**
         *
         * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
         * @see Data.h for more details
         */
        uData16 samplingTime;

    };
}

/**
 *
 * @note The HIC++ requirements details are documented and well explained in the Data.h file. Please refer to it for any DR implementation explanation.
 * @see Data.h for more details
 */
namespace Defaults {
    using ExampleDataModel::Parameters;
    Parameters const DEFAULT_PARAMETERS = Parameters(DEFAULT_FWHM, DEFAULT_SAMPLING_TIME);
}

#endif //SPIRITSENSORGATEWAY_PARAMETERS_H
