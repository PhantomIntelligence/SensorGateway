/* AWLSettings .cpp */
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


#include <SensorCommunication/AWLSettings.h>

using awl::AWLSettings;

const std::string sDefaultSettingsFileName("AWLSimpleDemoSettings.xml");

AWLSettings* AWLSettings::globalSettings = NULL;

AWLSettings::AWLSettings() :
        sFileName(sDefaultSettingsFileName) {
}

AWLSettings* AWLSettings::InitSettings() {
    globalSettings = new AWLSettings();
    return (globalSettings);
}

AWLSettings* AWLSettings::GetGlobalSettings() {
    return (globalSettings);
}


bool AWLSettings::ReadSettings() {
    // Create an empty property tree object
    using boost::property_tree::ptree;
    propTree.clear();

    // Load the XML file into the property tree. If reading fails
    // (cannot open file, parse error), an exception is thrown.
    read_xml(sFileName, propTree);

    int receiverQty = propTree.get<int>("config.receivers.receiverQty");
    receiverSettings.resize(receiverQty);
    for (int receiverIndex = 0; receiverIndex < receiverQty; receiverIndex++) {
        char receiverKeyString[32];
        sprintf(receiverKeyString, "config.receivers.receiver%d", receiverIndex);
        std::string receiverKey = receiverKeyString;

        boost::property_tree::ptree& receiverNode = propTree.get_child(receiverKey);

        ReceiverSettings* receiverPtr = &receiverSettings[receiverIndex];
        receiverPtr->sReceiverType = receiverNode.get<std::string>("receiverType");
        // Display
        receiverPtr->displayedRangeMin = receiverNode.get<float>("displayedRangeMin");
        receiverPtr->displayedRangeMax = receiverNode.get<float>("displayedRangeMax");
        receiverPtr->lineWrapAround = receiverNode.get<float>("lineWrapAround", 32767.0);
        receiverPtr->channelsPerLine = receiverNode.get<int>("channelsPerLine", 1);



        // All channel info for the receiver
        int channelQty = receiverNode.get<int>("channelQty");
        receiverPtr->channelsConfig.resize(channelQty);

        for (int channelIndex = 0; channelIndex < channelQty; channelIndex++) {
            char channelKeyString[32];
            sprintf(channelKeyString, "channel%d", channelIndex);
            std::string channelKey = channelKeyString;

            boost::property_tree::ptree& channelNode = receiverNode.get_child(channelKey);

            ChannelConfig* channelConfigPtr = &receiverPtr->channelsConfig[channelIndex];
            channelConfigPtr->channelIndex = channelIndex;
            Get2DPoint(channelNode.get_child("fov"), channelConfigPtr->fovWidth, channelConfigPtr->fovHeight);
            float roll;
            channelConfigPtr->maxRange = channelNode.get<float>("maxRange");
        } // for (int channelIndex = 0;

    } // for (int receiverIndex = 0;



    // Debug and log file control
    bWriteDebugFile = propTree.get<bool>("config.debug.enableDebugFile");
    bWriteLogFile = propTree.get<bool>("config.debug.enableLogFile");

    brakingDeceleration = propTree.get<float>("config.dynamicTesting.brakingDeceleration");
    travelSpeed = propTree.get<float>("config.dynamicTesting.travelSpeed");

    return (true);
}

bool AWLSettings::StoreReceiverCalibration() {
    return (true);
}


void AWLSettings::GetPosition(boost::property_tree::ptree& node, float& forward, float& left, float& up) {
    forward = node.get<float>("forward");
    left = node.get<float>("left");
    up = node.get<float>("up");
}

void AWLSettings::GetOrientation(boost::property_tree::ptree& node, float& pitch, float& yaw, float& roll) {
    pitch = node.get<float>("pitch");
    yaw = node.get<float>("yaw");
    roll = node.get<float>("roll");
}

void AWLSettings::Get2DPoint(boost::property_tree::ptree& node, float& x, float& y) {
    x = node.get<float>("x");
    y = node.get<float>("y");
}

void AWLSettings::GetGeometry(boost::property_tree::ptree& geometryNode, float& forward, float& left, float& up,
                              float& pitch, float& yaw, float& roll) {
    boost::property_tree::ptree& positionNode = geometryNode.get_child("position");
    boost::property_tree::ptree& orientationNode = geometryNode.get_child("orientation");
    GetPosition(positionNode, forward, left, up);
    GetOrientation(orientationNode, pitch, yaw, roll);
}

void AWLSettings::GetColor(boost::property_tree::ptree& colorNode, uint8_t& red, uint8_t& green, uint8_t& blue) {
    red = colorNode.get<uint8_t>("red");
    green = colorNode.get<uint8_t>("green");
    blue = colorNode.get<uint8_t>("blue");
}


void AWLSettings::PutPosition(boost::property_tree::ptree& node, float forward, float left, float up) {
    node.put<float>("forward", forward);
    node.put<float>("left", left);
    node.put<float>("up", up);
}

void AWLSettings::PutOrientation(boost::property_tree::ptree& node, float pitch, float yaw, float roll) {
    node.put<float>("pitch", pitch);
    node.put<float>("yaw", yaw);
    node.put<float>("roll", roll);
}

void AWLSettings::Put2DPoint(boost::property_tree::ptree& node, float x, float y) {
    node.put<float>("x", x);
    node.put<float>("y", y);
}

void
AWLSettings::PutGeometry(boost::property_tree::ptree& geometryNode, float forward, float left, float up, float pitch,
                         float yaw, float roll) {
    boost::property_tree::ptree& positionNode = geometryNode.get_child("position");
    boost::property_tree::ptree& orientationNode = geometryNode.get_child("orientation");
    PutPosition(positionNode, forward, left, up);
    PutOrientation(orientationNode, pitch, yaw, roll);
}

void AWLSettings::PutColor(boost::property_tree::ptree& colorNode, uint8_t red, uint8_t green, uint8_t blue) {
    colorNode.get<uint8_t>("red");
    colorNode.get<uint8_t>("green");
    colorNode.get<uint8_t>("blue");
}


