/* ReceiverSimulatorCapture.cpp */
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


#include "SensorCommunication/ReceiverSimulatorCapture.h"
#ifndef Q_MOC_RUN

#include <boost/property_tree/ptree.hpp>

#endif



using awl::ReceiverSimulatorCapture;

ReceiverSimulatorCapture::ReceiverSimulatorCapture(int receiverID, int inReceiverChannelQty,
                                                   int inFrameRate, ChannelMask& inChannelMask,
                                                   MessageMask& inMessageMask, float inRangeOffset,
                                                   const RegisterSet& inRegistersFPGA,
                                                   const RegisterSet& inRegistersADC,
                                                   const RegisterSet& inRegistersGPIO,
                                                   const AlgorithmSet& inParametersAlgos) :
        ReceiverCapture(receiverID, inReceiverChannelQty, inFrameRate, inChannelMask, inMessageMask, inRangeOffset,
                        inRegistersFPGA, inRegistersADC, inRegistersGPIO, inParametersAlgos) {
}


ReceiverSimulatorCapture::ReceiverSimulatorCapture(int receiverID, boost::property_tree::ptree& propTree) :
        ReceiverCapture(receiverID, propTree) {
    // Read the configuration from the configuration file
    ReadConfigFromPropTree(propTree);

}

ReceiverSimulatorCapture::~ReceiverSimulatorCapture() {
    CloseDebugFile(debugFile);
    EndDistanceLog();
}

int threadCount = 0;
static float trackDistance = 0.0;

void ReceiverSimulatorCapture::DoOneThreadIteration() {
    if (!WasStopped()) {
        // Simulate some tracks for debug purposes
        double elapsed = GetElapsed();

        Track::Ptr track = acquisitionSequence->MakeUniqueTrack(currentFrame, 1);

        track->firstTimeStamp = currentFrame->timeStamp;
        track->timeStamp = currentFrame->timeStamp;
        trackDistance += 0.0001;
        if (trackDistance > 30.0) trackDistance = 0.0;
        track->distance = trackDistance;
        track->intensity = -3.00;
        track->channels.byteData = 0X7F;

        track->velocity = 30.0;
        track->acceleration = 0;
        track->threatLevel = AlertCondition::eThreatLow;
        track->part1Entered = true;
        track->part2Entered = true;
        track->part3Entered = true;
        track->part4Entered = true;

        track->probability = 99;
        track->timeStamp = elapsed;
        track->firstTimeStamp = elapsed;


        track = acquisitionSequence->MakeUniqueTrack(currentFrame, 2);

        track->firstTimeStamp = currentFrame->timeStamp;
        track->timeStamp = currentFrame->timeStamp;
        track->distance = 25.0;
        track->intensity = -0.1;
        track->channels.byteData = 0X7F;

        track->velocity = 3;
        track->acceleration = 0;
        track->threatLevel = AlertCondition::eThreatLow;
        track->part1Entered = true;
        track->part2Entered = true;
        track->part3Entered = true;
        track->part4Entered = true;

        track->probability = 99;
        track->timeStamp = elapsed;
        track->firstTimeStamp = elapsed;

        track = acquisitionSequence->MakeUniqueTrack(currentFrame, 3);
        track->firstTimeStamp = currentFrame->timeStamp;

        track->timeStamp = currentFrame->timeStamp;
        track->distance = 40;
        track->intensity = 1.00;
        track->channels.byteData = 0X7F;

        track->velocity = 3;
        track->acceleration = 0;
        track->threatLevel = AlertCondition::eThreatLow;
        track->part1Entered = true;
        track->part2Entered = true;
        track->part3Entered = true;
        track->part4Entered = true;

        track->probability = 99;
        track->timeStamp = elapsed;
        track->firstTimeStamp = elapsed;

        track = acquisitionSequence->MakeUniqueTrack(currentFrame, 4);

        track->firstTimeStamp = currentFrame->timeStamp;
        track->timeStamp = currentFrame->timeStamp;
        track->distance = 155;
        track->intensity = 22.00;
        track->channels.byteData = 0X7F;

        track->velocity = 3;
        track->acceleration = 0;
        track->threatLevel = AlertCondition::eThreatLow;
        track->part1Entered = true;
        track->part2Entered = true;
        track->part3Entered = true;
        track->part4Entered = true;

        track->probability = 99;
        track->timeStamp = elapsed;
        track->firstTimeStamp = elapsed;

        threadCount = ++threadCount % 7;
        ProcessCompletedFrame();
    } // if  (!WasStoppped)
}


bool ReceiverSimulatorCapture::ReadConfigFromPropTree(boost::property_tree::ptree& propTree) {
    ReceiverCapture::ReadConfigFromPropTree(propTree);
    receiverStatus.signalToNoiseFloor = 18.0;
    return (true);
}

