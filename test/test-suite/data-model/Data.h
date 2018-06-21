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


#ifndef SPIRITSENSORGATEWAY_SENSORDATA_H
#define SPIRITSENSORGATEWAY_SENSORDATA_H

#include <cstdint>
#include <cassert>
#include "ProcessedData.h"

/**
 * @brief The Data Representation (DR) classes contained in this namespace form an basic implementation guide for DR classes in Spectre.
 * @note The DR classes member function signatures are important, as they insure compliance with the High Integrity C++ Coding Standard Version 4.0.
 * @see http://www.codingstandard.com/
 */
namespace ExampleDataModel {

    /**
     * @brief Example of a complement that could be used to create a data structure
     * @note As with all Data Representation used in Spectre, the construction of a DR requires a Complement.
     * @see the ExampleDataModel::Data::returnDefaultComplement() method.
     */
    struct Complement {
        const Sample convolutedDetection;
    };


    /**
     * @brief The main Data Representation (DR) example of Spectre. It is stateless, as all DR used in Spectre should be.
     * @note This particular class inherits another DR.
     * It is useful to build DR this way when building data from other data (see NativeData and ProcessedData) for several reasons:
     *  - it preserves the statelessness of the data structure when creating new results from the data already in a DR class;
     *  - it allows to chose when and where to split the different procedures and manipulations to better control memory and cpu used by Spectre.
     * @note Inheritance of previous DR class is not mandatory, as some DR could represent user command to send to the sensor or be used for debugging and logging purposes.
     */
    class Data final : public ProcessedData {
    public:

        /**
         * @brief Default constructor of the Data example Data Representation (DR) class.
         * @param processedData the base data held in the class from which this DR directly inherits. Is used to simply transfer the data while preserving the integrity of the source.
         * @param complement the new data created for this DR that complements the base data it is derived from.
         * @note As for all DR, its constructor shall only initialise its components. No operation or data manipulation should be done in DR classes.
         */
        explicit Data(ProcessedData const& processedData, Complement const& complement);

        /**
         * @brief Default destructor of the Data example Data Representation (DR) class.
         * @note Since DR are stateless, their destructors should be *= default*.
         */
        ~Data() noexcept = default;

        /**
         * @brief Copy constructor for the Data example Data Representation (DR) class.
         * @note It is important all DR of Spectre implement a copy constructor, as it is used in many instances of Spectre's classes.
         * @param other the Data DR instance from which the data is copied to construct this instance.
         */
        Data(Data const& other);

        /**
         * @brief Move constructor for the Data example Data Representation (DR) class.
         * @note It is important all DR of Spectre implement a move constructor, as it is used in many instances of Spectre's classes.
         * @param other the Data DR instance from which the data is moved into this instance.
         * @attention the *other* param should be used in any way after having been used in a move constructor
         */
        Data(Data&& other) noexcept;

        /**
         * @brief Copy assignment operator for the Data example Data Representation (DR) class.
         * @note It is important all DR of Spectre implement a copy assignment operator, as it is used in many instances of Spectre's classes.
         * @param other the Data DR instance copy-assigned to this instance.
         * @return A handle to this instance.
         */
        Data& operator=(Data other)& {
            swap(*this, other);
            return *this;
        }

        /**
         * @brief Move assignment operator for the Data example Data Representation (DR) class.
         * @note It is important all DR of Spectre implement a move assignment operator, as it is used in many instances of Spectre's classes.
         * @param other the DR instance move-assigned to this instance.
         * @return A handle to this instance.
         * @attention the *other* param should be used in any way after having been used in a move assignation.
         */
        Data& operator=(Data&& other)& noexcept {
            Data temporary(std::move(other));
            swap(*this, temporary);
            return *this;
        }

        /**
         * @brief Non throwing swap used in the provides the Strong Exception Guarantee for the different assignment and construction operations it is used in.
         * @param current the instance on the receiving hand of the swap.
         * @param other the instance from which the data will be swapped from.
         */
        void swap(Data& current, Data& other) noexcept;

        /**
         * @brief Example for a getter signature for a Data Representation (DR) class.
         * @return a copy of the member data.
         * @attention all getters for all DR must return a const& to the member and be const functions
         */
        Sample const& getConvolutedDetection() const;

        /**
         * @brief Equality operator for a Data Representation (DR) class.
         * @param other the instance to which the current DR instance is compared.
         * @return true if both instances contain the same data; false otherwise.
         */
        Boolean operator==(Data const& other) const;

        /**
         * @brief Inequality operator for a Data Representation (DR) class.
         * @param other the instance to which the current DR instance is compared.
         * @return false if both instances contain the same data; true otherwise.
         */
        Boolean operator!=(Data const& other) const;

        /**
         * @brief A mandatory function for all Data Representation (DR) class.
         * @return an empty, default-constructed DR instance.
         * @see the example below this class for the formation of the Default DR instance.
         */
        Data static const& returnDefaultData() noexcept;

    private:
        /**
         * @brief Example of the signature for a possible data contained by a Data Representation (DR) class.
         */
        Sample convolutedDetection;
    };

}


/**
 * @brief Default and/or empty values to avoid nullptr and allow empty buffer construction.
 * @note Every Data Representation (DR) class should have two elements defined in this namespace, i.e. a Data and a Complement
 * @see ExampleDataModel::Complement, ExampleDataModel::Data
 */
namespace Defaults {
    using ExampleDataModel::Complement;
    using ExampleDataModel::Data;

    const Complement DEFAULT_COMPLEMENT = Complement{EMPTY_SAMPLE};
    const Data DEFAULT_DATA = Data(DEFAULT_PROCESSED_DATA, DEFAULT_COMPLEMENT);
}

#endif //SPIRITSENSORGATEWAY_SENSORDATA_H
