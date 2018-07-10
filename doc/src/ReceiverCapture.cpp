/* ReceiverCapture.cpp */
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

#ifndef Q_MOC_RUN

#include <boost/property_tree/ptree.hpp>

#endif

#include "SensorCommunication/ReceiverCapture.h"

using awl::ReceiverCapture;

const int ReceiverCapture::maximumSensorFrames(100);
// Sensor transitions going from left to right...

const std::string sDefaultReceiverType = "Generic";
const int defaultFrameRate(50);
const uint8_t defaultChannelMaskValue = 127;
const float defaultSignalToNoiseFloor = -10.0;


ReceiverCapture::ReceiverCapture(int receiverID, int inReceiverChannelQty,
                                 int inFrameRate, ChannelMask& inChannelMask, MessageMask& inMessageMask,
                                 float inRangeOffset,
                                 const RegisterSet& inRegistersFPGA, const RegisterSet& inRegistersADC,
                                 const RegisterSet& inRegistersGPIO, const AlgorithmSet& inParametersAlgos) :
        receiverID(receiverID),
        receiverChannelQty(inReceiverChannelQty),
        acquisitionSequence(new AcquisitionSequence()),
        frameID(0),
        currentFrame(new SensorFrame(receiverID, 0, inReceiverChannelQty)),
        measurementOffset(inRangeOffset),
        bFrameInvalidated(false),
        registersFPGA(inRegistersFPGA),
        registersADC(inRegistersADC),
        registersGPIO(inRegistersGPIO),
        parametersAlgos(inParametersAlgos),
        sReceiverType(sDefaultReceiverType),
        targetHintDistance(0.0),
        targetHintAngle(0.0) {
    // Initialize default status values
    InitStatus();

    receiverStatus.frameRate = static_cast<uData8>(inFrameRate);
    receiverStatus.currentAlgo = 0;
    receiverStatus.currentAlgoPendingUpdates = 0;

    // Update settings from application
    receiverStatus.frameRate = static_cast<uData8>(inFrameRate);
    receiverStatus.channelMask = inChannelMask;
    receiverStatus.messageMask = inMessageMask;

    // Reflect the settings in hardware
    SetMessageFilters();
}

ReceiverCapture::ReceiverCapture(int receiverID, boost::property_tree::ptree& propTree) :
        receiverID(receiverID),
        acquisitionSequence(new AcquisitionSequence()),
        frameID(0),
        bFrameInvalidated(false),
        sReceiverType(sDefaultReceiverType),
        targetHintDistance(0.0),
        targetHintAngle(0.0) {
    // Read the configuration from the configuration file

    receiverStatus.signalToNoiseFloor = defaultSignalToNoiseFloor;

    ReadConfigFromPropTree(propTree);
    ReadRegistersFromPropTree(propTree);

    // Initialize default status values
    InitStatus();

    // make sure that the communication is reset.
    receiverStatus.currentAlgoPendingUpdates = 0;

    // Create a temporary SensorFrame object for storage of the current data
    currentFrame = SensorFrame::Ptr(new SensorFrame(receiverID, 0, receiverChannelQty));

    // Reflect the settings in hardware
    SetMessageFilters();
}

ReceiverCapture::~ReceiverCapture() {
    EndDistanceLog();
}

void ReceiverCapture::InitStatus() {
    receiverStatus.statusHasBeenUpdated = false;
    receiverStatus.systemTemperature = 0.0;
    receiverStatus.coreVoltage = 0;
    receiverStatus.hardwareError.byteData = 0;
    receiverStatus.receiverError.byteData = 0;
    receiverStatus.status.byteData = 0;
    receiverStatus.version.major = 0;
    receiverStatus.version.minor = 0;
    receiverStatus.bootChecksumError.byteData = 0;
    receiverStatus.bootSelfTest.byteData = 0;

    receiverStatus.inPlaybackMode = false;
    receiverStatus.inRecordMode = false;
    receiverStatus.playbackFileName = "";
    receiverStatus.recordFileName = "";

    receiverStatus.lastCommandError = 0;
    receiverStatus.fpgaRegisterAddressRead = 0;
    receiverStatus.fpgaRegisterValueRead = 0;
    receiverStatus.adcRegisterAddressRead = 0;
    receiverStatus.adcRegisterValueRead = 0;
    receiverStatus.gpioRegisterAddressRead = 0;
    receiverStatus.gpioRegisterValueRead = 0;

    receiverStatus.signalToNoiseFloor = defaultSignalToNoiseFloor;
}


int ReceiverCapture::GetFrameQty() {
    return static_cast<int>(acquisitionSequence->sensorFrames.size());
}


bool ReceiverCapture::CopyReceiverFrame(FrameID inFrameID, SensorFrame::Ptr& outSensorFrame,
                                        Publisher::SubscriberID inSubscriberID) {
    if (!LockNews(inSubscriberID)) return (false);

    SensorFrame::Ptr sensorFrame;
    bool bFound = acquisitionSequence->FindSensorFrame(inFrameID, sensorFrame);
    if (bFound) {
        *outSensorFrame = *sensorFrame;
    }

    UnlockNews(inSubscriberID);
    return (bFound);
}

bool ReceiverCapture::CopyReceiverRawDetections(FrameID inFrameID, Detection::Vector& outDetections,
                                                Publisher::SubscriberID inSubscriberID) {
    if (!LockNews(inSubscriberID)) return (false);

    SensorFrame::Ptr sensorFrame;
    bool bFound = acquisitionSequence->FindSensorFrame(inFrameID, sensorFrame);
    if (bFound) {
        outDetections.clear();
        outDetections = sensorFrame->rawDetections;
    }

    UnlockNews(inSubscriberID);
    return (bFound);
}


bool ReceiverCapture::CopyReceiverStatusData(ReceiverStatus& outStatus, Publisher::SubscriberID inSubscriberID) {
    std::lock_guard guard(GetMutex());

    outStatus = receiverStatus;
    receiverStatus.statusHasBeenUpdated = false;
    return (true);
}

FrameID ReceiverCapture::GetFrameID(int inFrameIndex) {
    std::lock_guard guard(GetMutex());
    FrameID frameID;

    if (inFrameIndex > acquisitionSequence->sensorFrames.size() - 1) {
        frameID = 0xFFFFFFFF;
    } else {
        SensorFrame::Ptr sensorFrame = acquisitionSequence->sensorFrames._Get_container().at(inFrameIndex);
        frameID = sensorFrame->frameID;
    }

    return (frameID);
}

void ReceiverCapture::SetMeasurementOffset(double inMeasurementOffset) {
    measurementOffset = inMeasurementOffset;
}

void ReceiverCapture::GetMeasurementOffset(double& outMeasurementOffset) {
    outMeasurementOffset = measurementOffset;
}

bool ReceiverCapture::SetPlaybackFileName(std::string inPlaybackFileName) {
    receiverStatus.playbackFileName = inPlaybackFileName;
    return (true);
}

bool ReceiverCapture::SetRecordFileName(std::string inRecordFileName) {
    receiverStatus.recordFileName = inRecordFileName;
    return (true);
}

bool ReceiverCapture::StartPlayback(uint8_t frameRate, ChannelMask channelMask) {
    receiverStatus.inPlaybackMode = true;
    return (true);
}

bool ReceiverCapture::StartRecord(uint8_t frameRate, ChannelMask channelMask) {
    receiverStatus.inRecordMode = true;
    return (true);
}

bool ReceiverCapture::StopPlayback() {
    receiverStatus.inPlaybackMode = false;
    return (true);
}

bool ReceiverCapture::StopRecord() {
    receiverStatus.inRecordMode = false;
    return (true);
}


bool ReceiverCapture::SetMessageFilters() {
    return (SetMessageFilters(receiverStatus.frameRate, receiverStatus.channelMask, receiverStatus.messageMask));
}

double ReceiverCapture::GetElapsed() {
//    boost::posix_time::ptime nowTime(boost::posix_time::microsec_clock::local_time());
//    boost::posix_time::time_duration msdiff = nowTime - startTime;
//    return (msdiff.total_microseconds() / 1000.0);
    chrono::duration duration = highResolutionClock.now() - startTime;
    return (duration.count() / 1000.0); // MAKE SURE IT IS IN MICROSECONDS
}

void ReceiverCapture::InvalidateFrame() {
    bFrameInvalidated.store(true);
}

bool ReceiverCapture::isFrameValid() {
    return !bFrameInvalidated.load();
}

void ReceiverCapture::ProcessCompletedFrame() {
    std::lock_guard guard(GetMutex());

    // timestamp the currentFrame
    double elapsed = GetElapsed();
    currentFrame->timeStamp = elapsed;

    TimestampTracks(currentFrame);
    TimestampDetections(currentFrame);
    logTrack();
    logDistances();

    FrameID completedFrameID = currentFrame->GetFrameID();
    std::atomic_bool hasNewFrameToPublish(false);
    if (isFrameValid()) {
        addFrameToSequence();
        controlSequenceSize();
        hasNewFrameToPublish.store(true);
    }

    // Create a new current frame.
    FrameID frameID = acquisitionSequence->AllocateFrameID();
    currentFrame = SensorFrame::Ptr(new SensorFrame(receiverID, frameID, receiverChannelQty));
    InvalidateFrame();

    //---------------------------------------------------

    if (hasNewFrameToPublish) {
        PutNews(completedFrameID);
        DebugFilePrintf(debugFile, "FrameID- %lu", completedFrameID);
    } else {
        DebugFilePrintf(debugFile, "FrameIDUnpublished- %lu", completedFrameID);
    }

}

void ReceiverCapture::controlSequenceSize() const {
    // Make sure we do not keep too many of those frames around.
    // Remove the older frame if we exceed the data-flow capacity
    if (acquisitionSequence->sensorFrames.size() > maximumSensorFrames) {
        acquisitionSequence->sensorFrames.pop();
    }
}

void ReceiverCapture::addFrameToSequence() const { acquisitionSequence->sensorFrames.push(currentFrame); }

void ReceiverCapture::logDistances() {
    if (receiverStatus.messageMask.bitFieldData.distance_1_4 ||
        receiverStatus.messageMask.bitFieldData.distance_5_8 ||
        receiverStatus.messageMask.bitFieldData.intensity_1_4 ||
        receiverStatus.messageMask.bitFieldData.intensity_5_8) {
        LogDistances(logFile, currentFrame);
    }
}

void ReceiverCapture::logTrack() {
    if (receiverStatus.messageMask.bitFieldData.obstacle) {
        LogTracks(logFile, currentFrame);
    }
}


void ReceiverCapture::TimestampTracks(SensorFrame::Ptr sourceFrame) {
    for (auto track: sourceFrame->tracks) {
        track->firstTimeStamp = currentFrame->timeStamp;
        track->timeStamp = currentFrame->timeStamp;
    }
}

void ReceiverCapture::TimestampDetections(SensorFrame::Ptr sourceFrame) {
    for (auto rawDetection: sourceFrame->rawDetections) {
        rawDetection->firstTimeStamp = currentFrame->timeStamp;
        rawDetection->timeStamp = currentFrame->timeStamp;
    }
}

int ReceiverCapture::FindRegisterByAddress(const RegisterSet& inRegisterSet, uint16_t inAddress) {
    for (int i = 0; i < inRegisterSet.size(); i++) {
        if (inRegisterSet.at(i).address == inAddress) {
            return (i);
        }
    }

    return (-1);
}

AlgorithmParameter* ReceiverCapture::FindAlgoParamByAddress(int inAlgoID, uint16_t inAddress) {

    for (int i = 0; i < parametersAlgos.algorithms[inAlgoID].parameters.size(); i++) {
        if (parametersAlgos.algorithms[inAlgoID].parameters[i].address == inAddress) {
            return (&parametersAlgos.algorithms[inAlgoID].parameters[i]);
        }
    }
    return nullptr;
}

bool ReceiverCapture::BeginDistanceLog() {
    if (!logFile.is_open()) {
        OpenLogFile(logFile, "DistanceLog.dat", true);
    }

    LogFilePrintf(logFile, "Start distance log");
    // Title Line
    LogFilePrintf(logFile,
                  "Track Description:;Track;trackID;_;__;___;Expected;expectDistance;expectAngle;Val;distance;intensity;velocity;acceleration;ttc;decelerationToStop;probability;ThreatLevel;Ch.0;Ch.1;Ch.2;Ch.3;Ch.4;Ch.5;Ch.6;");
    LogFilePrintf(logFile,
                  "Distance Description:;Dist;_;__;Channel;DetectionID;Expected;expectDistance;expectAngle;Val;distance;intensity;velocity;acceleration;ttc;decelerationToStop;probability;ThreatLevel");

    return (true);
}


bool ReceiverCapture::EndDistanceLog() {
    if (logFile.is_open())
        CloseLogFile(logFile);
    return (false);
}


void ReceiverCapture::LogTracks(ofstream& logFile, SensorFrame::Ptr sourceFrame) {
    // Update the coaslesced tracks
    Track::Vector::iterator trackIterator = sourceFrame->tracks.begin();

    while (trackIterator != sourceFrame->tracks.end()) {
        Track::Ptr track = *trackIterator;
        if (track->IsComplete()) {
            //Date;Comment (empty);"TrackID", "Track"/"Dist";TrackID;"Channel";....Val;distance;intensity,speed;acceleration;probability;timeToCollision);
            LogFilePrintf(logFile,
                          " ;Track;%d; ; ; ;Expected;%.2f;%.1f;Val;%.2f;%.1f;%.1f;%.1f;%.3f;%.1f;%.0f;%d;%d;%d;%d;%d;%d;%d;%d;",
                          track->trackID,
                          targetHintDistance,
                          targetHintAngle,
                          track->distance,
                          track->intensity,
                          track->velocity,
                          track->acceleration,
                          track->timeToCollision,
                          track->decelerationToStop,
                          track->probability,
                          track->threatLevel,
                          (track->channels.bitFieldData.channel0) ? 0 : 0,
                          (track->channels.bitFieldData.channel1) ? 1 : 0,
                          (track->channels.bitFieldData.channel2) ? 2 : 0,
                          (track->channels.bitFieldData.channel3) ? 3 : 0,
                          (track->channels.bitFieldData.channel4) ? 4 : 0,
                          (track->channels.bitFieldData.channel5) ? 5 : 0,
                          (track->channels.bitFieldData.channel6) ? 6 : 0);

        }  // if (track...

        trackIterator++;
    } // while (trackIterator...
}

void ReceiverCapture::LogDistances(ofstream& logFile, SensorFrame::Ptr sourceFrame) {
    Detection::Vector::iterator detectionIterator = sourceFrame->rawDetections.begin();
    while (detectionIterator != sourceFrame->rawDetections.end()) {
        Detection::Ptr detection = *detectionIterator;
        LogFilePrintf(logFile, " ;Dist;;Channel;%d;%d;Expected;%.2f;%.1f;Val;%.2f;%.1f;%.2f;%.1f;%.3f;%.1f;%.0f;%d",
                      detection->channelID,
                      detection->detectionID,
                      targetHintDistance,
                      targetHintAngle,
                      detection->distance,
                      detection->intensity,
                      detection->velocity,
                      detection->acceleration,
                      detection->timeToCollision,
                      detection->decelerationToStop,
                      detection->probability,
                      detection->threatLevel);

        detectionIterator++;
    }
}

// Configuration file related functions


bool ReceiverCapture::ReadConfigFromPropTree(boost::property_tree::ptree& propTree) {
    char receiverKeyString[32];
    sprintf(receiverKeyString, "config.receivers.receiver%d", receiverID);
    std::string receiverKey = receiverKeyString;

    boost::property_tree::ptree& receiverNode = propTree.get_child(receiverKey);

    sReceiverType = receiverNode.get<std::string>("receiverType");
    measurementOffset = receiverNode.get<float>("rangeOffset");
    receiverChannelQty = receiverNode.get<int>("channelQty");
    receiverStatus.frameRate = receiverNode.get<uint8_t>("frameRate");    // Default frame rate is 100Hz

    receiverStatus.channelMask.byteData = receiverNode.get<uint8_t>("channelMask");

    receiverStatus.messageMask.byteData = 0;
    if (receiverNode.get<bool>("msgEnableObstacle")) receiverStatus.messageMask.bitFieldData.obstacle = 1;
    if (receiverNode.get<bool>("msgEnableDistance_1_4")) receiverStatus.messageMask.bitFieldData.distance_1_4 = 1;
    if (receiverNode.get<bool>("msgEnableDistance_5_8")) receiverStatus.messageMask.bitFieldData.distance_5_8 = 1;
    if (receiverNode.get<bool>("msgEnableIntensity_1_4")) receiverStatus.messageMask.bitFieldData.intensity_1_4 = 1;
    if (receiverNode.get<bool>("msgEnableIntensity_5_8")) receiverStatus.messageMask.bitFieldData.intensity_5_8 = 1;

    return (true);
}

