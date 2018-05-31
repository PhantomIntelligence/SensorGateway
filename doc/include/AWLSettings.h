#ifndef _AWLSettings__H
#define _AWLSettings__H

/*
	Copyright 2014, 2015 Phantom Intelligence Inc.

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

#include "DetectionStruct.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#define VelocityToKmH(velocity) (velocity * 3.6)

namespace awl {

    typedef struct ChannelConfig {
        int channelIndex;
        float fovWidth;
        float fovHeight;
        float maxRange;
    }
            ChannelConfig;

    typedef std::vector<ChannelConfig> ChannelConfigVector;


    typedef enum {
        eVelocityUnitsMS = 0,
        eVelocityUnitsKMH = 1
    }
            VelocityUnits;

    typedef struct ReceiverSettings {
        // Channel configuration
        ChannelConfigVector channelsConfig;

        // Receiver
        std::string sReceiverType;

        float displayedRangeMin;
        float displayedRangeMax;

        float lineWrapAround; // For sensors that use the "line wraparound trick".
        int channelsPerLine;
    }
            ReceiverSettings;

    typedef std::vector<ReceiverSettings> ReceiverSettingsVector;

    class AWLSettings {
    public:
        static AWLSettings* InitSettings();

        static AWLSettings* GetGlobalSettings();

        // Constructor
        AWLSettings();

        bool ReadSettings();

        boost::property_tree::ptree& GetPropTree() { return (propTree); };


        // Get/Set methods

        static void GetPosition(boost::property_tree::ptree& node, float& forward, float& left, float& up);

        static void GetOrientation(boost::property_tree::ptree& node, float& pitch, float& yaw, float& roll);

        static void Get2DPoint(boost::property_tree::ptree& node, float& x, float& y);

        static void
        GetGeometry(boost::property_tree::ptree& geometryNode, float& forward, float& left, float& up, float& pitch,
                    float& yaw, float& roll);

        static void GetColor(boost::property_tree::ptree& colorNodeNode, uint8_t& red, uint8_t& green, uint8_t& blue);

        static void PutPosition(boost::property_tree::ptree& node, float forward, float left, float up);

        static void PutOrientation(boost::property_tree::ptree& node, float pitch, float yaw, float roll);

        static void Put2DPoint(boost::property_tree::ptree& node, float x, float y);

        static void
        PutGeometry(boost::property_tree::ptree& geometryNode, float forward, float left, float up, float pitch,
                    float yaw, float roll);

        static void PutColor(boost::property_tree::ptree& colorNode, uint8_t red, uint8_t green, uint8_t blue);

        /** \brief Stores the current receiver calibration settings
            * \return true if storage processe dwithout error. False in case of a storage error.
          */
        bool AWLSettings::StoreReceiverCalibration();

    public:
        // Receiver configuration
        ReceiverSettingsVector receiverSettings;

        // Dynamic testing
        float brakingDeceleration;
        float travelSpeed;

        // Debug
        bool bWriteDebugFile;
        bool bWriteLogFile;


    protected:
        std::string sFileName;
        static AWLSettings* globalSettings;
        // Property tree contains all the information from the configuration file
        boost::property_tree::ptree propTree;
    };
} // namespace AWL

#endif
