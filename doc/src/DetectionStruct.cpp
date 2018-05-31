/* DetectionStruct.cpp */
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

#include <stdint.h>
#include <string>

#include "SensorCommunication/DetectionStruct.h"

#define _USE_MATH_DEFINES 1  // Makes sure we have access to all math constants, like M_PI

#include <math.h>

using awl::AlertCondition;
using awl::AcquisitionSequence;
using awl::Track;
using awl::Detection;
using awl::SensorFrame;
using awl::FrameID;


AlertCondition::Vector AlertCondition::globalAlertsVector;


AcquisitionSequence::AcquisitionSequence() :
        frameID(1) {
}

FrameID AcquisitionSequence::AllocateFrameID() {
    return (frameID++);
}

FrameID AcquisitionSequence::GetLastFrameID() {
    if (sensorFrames.size() <= 0) return (0);

    return (sensorFrames.back()->GetFrameID());

}


bool AcquisitionSequence::FindTrack(SensorFrame::Ptr currentFrame, TrackID trackID, Track::Ptr& outTrack) {

    Track::Vector::iterator trackIterator = currentFrame->tracks.begin();
    while (trackIterator != currentFrame->tracks.end()) {
        Track::Ptr track = *trackIterator;
        if (track->GetTrackID() == trackID) {
            outTrack = track;
            return (true);
        }

        trackIterator++;
    }

    return (false);
}

Track::Ptr AcquisitionSequence::MakeUniqueTrack(SensorFrame::Ptr currentFrame, TrackID trackID) {
    Track::Ptr track;
    bool bExists = FindTrack(currentFrame, trackID, track);
    if (!bExists) {
        track = Track::Ptr(new Track(trackID));
        currentFrame->tracks.push_back(track);
    }

    return (track);
}

bool AcquisitionSequence::FindSensorFrame(FrameID frameID, SensorFrame::Ptr& outSensorFrame) {
    for (int i = 0; i < sensorFrames.size(); i++) {
        SensorFrame::Ptr sensorFrame = sensorFrames._Get_container().at(i);
        if (sensorFrame->GetFrameID() == frameID) {
            outSensorFrame = sensorFrame;
            return (true);
        }
    }

    return (false);
}


SensorFrame::SensorFrame(int inReceiverID, FrameID inFrameID, int inChannelQty) :
        receiverID(inReceiverID),
        frameID(inFrameID),
        channelQty(inChannelQty),
        rawDetections(),
        tracks(),
        timeStamp(0) {
}

Detection::Ptr SensorFrame::MakeUniqueDetection(Detection::Vector& detectionVector, int channelID, int detectionID) {
    Detection::Ptr detection;
    bool bExists = FindDetection(detectionVector, channelID, detectionID, detection);
    if (!bExists) {
        detection = Detection::Ptr(new Detection(receiverID, channelID, detectionID));
        detectionVector.push_back(detection);
    }

    return (detection);
}

bool SensorFrame::FindDetection(Detection::Vector& detectionVector, int inChannelID, int inDetectionID,
                                Detection::Ptr& outDetection) {
    Detection::Vector::iterator detectionIterator = detectionVector.begin();
    while (detectionIterator != detectionVector.end()) {
        Detection::Ptr detection = *detectionIterator;
        if (detection->channelID == inChannelID && detection->detectionID == inDetectionID) {
            outDetection = detection;
            return (true);
        }

        detectionIterator++;
    }

    return (false);
}


Detection::Detection() :
        receiverID(0),
        channelID(0),
        detectionID(0),
        distance(0.0),
        intensity(0.0),
        velocity(0.0),
        acceleration(0.0),
        timeToCollision(NAN),
        decelerationToStop(NAN),
        probability(0.0),
        timeStamp(0),
        firstTimeStamp(0),
        relativeToSensorCart(),
        relativeToSensorSpherical(),
        relativeToVehicleCart(),
        relativeToVehicleSpherical(),
        relativeToWorldCart(),
        relativeToWorldSpherical() {
}


Detection::Detection(int inReceiverID, int inChannelID, int inDetectionID) :
        receiverID(inReceiverID),
        channelID(inChannelID),
        detectionID(inDetectionID),
        distance(0.0),
        intensity(0.0),
        velocity(0.0),
        acceleration(0.0),
        timeToCollision(NAN),
        decelerationToStop(NAN),
        probability(0.0),
        timeStamp(0),
        firstTimeStamp(0),
        relativeToSensorCart(),
        relativeToSensorSpherical(),
        relativeToVehicleCart(),
        relativeToVehicleSpherical(),
        relativeToWorldCart(),
        relativeToWorldSpherical() {

}

Detection::Detection(int inReceiverID, int inChannelID, int inDetectionID, float inDistance, float inIntensity,
                     float inVelocity,
                     float inTimeStamp, float inFirstTimeStamp, TrackID inTrackID,
                     AlertCondition::ThreatLevel inThreatLevel) :
        receiverID(inReceiverID),
        channelID(inChannelID),
        detectionID(inDetectionID),
        distance(inDistance),
        intensity(inIntensity),
        velocity(inVelocity),
        acceleration(0.0),
        timeToCollision(NAN),
        probability(0.0),
        timeStamp(inTimeStamp),
        firstTimeStamp(inFirstTimeStamp),
        trackID(inTrackID),
        threatLevel(inThreatLevel),
        relativeToSensorCart(),
        relativeToSensorSpherical(),
        relativeToVehicleCart(),
        relativeToVehicleSpherical(),
        relativeToWorldCart(),
        relativeToWorldSpherical() {
}


Track::Track(int inTrackID) :
        distance(0.0f),
        velocity(NAN),
        acceleration(NAN),
        timeToCollision(NAN),
        decelerationToStop(NAN),
        timeStamp(0.0f),
        firstTimeStamp(0.0),
        trackID(inTrackID),
        threatLevel(AlertCondition::eThreatNone),
        part1Entered(false),
        part2Entered(false),
        part3Entered(false),
        part4Entered(false) {
    channels.byteData = 0;
}


AlertCondition::AlertCondition(AlertCondition::AlertType inAlertType, int inReceiverID, ChannelMask inChannelMask,
                               float inMinRange, float inMaxRange, ThreatLevel inThreatLevel) :
        alertType(inAlertType),
        receiverID(inReceiverID),
        channelMask(inChannelMask),
        minRange(inMinRange),
        maxRange(inMaxRange),
        threatLevel(inThreatLevel) {
}

AlertCondition::AlertCondition(AlertCondition& sourceCondition) {
    *this = sourceCondition;
}

AlertCondition::ThreatLevel AlertCondition::FindDetectionThreat(std::shared_ptr <Detection> detection) {
    AlertCondition::ThreatLevel maxThreatLevel = AlertCondition::eThreatNone;

    AlertCondition::Vector::iterator alertIterator = globalAlertsVector.begin();
    while (alertIterator != globalAlertsVector.end()) {
        AlertCondition::Ptr alert = *alertIterator;
        ChannelMask channelMask;
        channelMask.byteData = 0x01 << detection->channelID;

        if (alert->receiverID == detection->receiverID && (alert->channelMask.byteData & channelMask.byteData)) {
            AlertCondition::ThreatLevel currentThreatLevel = AlertCondition::eThreatNone;
            switch (alert->alertType) {
                case eAlertDistanceWithin:
                    if (detection->relativeToSensorSpherical.rho >= alert->minRange &&
                        detection->relativeToSensorSpherical.rho <= alert->maxRange) {
                        currentThreatLevel = alert->threatLevel;
                    }
                    break;
                case eAlertDistanceOutside:
                    if (!(detection->relativeToSensorSpherical.rho >= alert->minRange &&
                          detection->relativeToSensorSpherical.rho <= alert->maxRange)) {
                        currentThreatLevel = alert->threatLevel;
                    }
                    break;
                case eAlertSpeed:
                    if (detection->velocity >= alert->minRange && detection->velocity <= alert->maxRange) {
                        currentThreatLevel = alert->threatLevel;
                    }
                    break;

                case eAlertAcceleration:
                    if (detection->acceleration >= alert->minRange && detection->acceleration <= alert->maxRange) {
                        currentThreatLevel = alert->threatLevel;
                    }
                    break;

                case eAlertDecelerationToStop:
                    if (detection->decelerationToStop >= alert->minRange &&
                        detection->decelerationToStop <= alert->maxRange) {
                        currentThreatLevel = alert->threatLevel;
                    }
                    break;

                case eAlertTTC:
                    if (detection->timeToCollision >= alert->minRange &&
                        detection->timeToCollision <= alert->maxRange) {
                        currentThreatLevel = alert->threatLevel;
                    }
                    break;

                default:
                    break;
            }

            if (currentThreatLevel > maxThreatLevel) maxThreatLevel = currentThreatLevel;
        }

        alertIterator++;
    }

    return (maxThreatLevel);

}

AlertCondition::ThreatLevel AlertCondition::FindTrackThreat(int inReceiverID, std::shared_ptr <Track> track) {
    AlertCondition::ThreatLevel maxThreatLevel = AlertCondition::eThreatNone;

    AlertCondition::Vector::iterator alertIterator = globalAlertsVector.begin();
    while (alertIterator != globalAlertsVector.end()) {
        AlertCondition::Ptr alert = *alertIterator;
        if (alert->receiverID == inReceiverID && (alert->channelMask.byteData & track->channels.byteData)) {
            AlertCondition::ThreatLevel currentThreatLevel = AlertCondition::eThreatNone;
            switch (alert->alertType) {
                case eAlertDistanceWithin:
                    if (track->distance >= alert->minRange && track->distance <= alert->maxRange) {
                        currentThreatLevel = alert->threatLevel;
                    }
                    break;

                case eAlertDistanceOutside:
                    if (!(track->distance >= alert->minRange && track->distance <= alert->maxRange)) {
                        currentThreatLevel = alert->threatLevel;
                    }
                    break;

                case eAlertSpeed:
                    if (track->velocity >= alert->minRange && track->velocity <= alert->maxRange) {
                        currentThreatLevel = alert->threatLevel;
                    }
                    break;

                case eAlertAcceleration:
                    if (track->acceleration >= alert->minRange && track->acceleration <= alert->maxRange) {
                        currentThreatLevel = alert->threatLevel;
                    }
                    break;

                case eAlertTTC:
                    if (track->timeToCollision >= alert->minRange && track->timeToCollision <= alert->maxRange) {
                        currentThreatLevel = alert->threatLevel;
                    }
                    break;

                default:
                    break;
            }

            if (currentThreatLevel > maxThreatLevel) maxThreatLevel = currentThreatLevel;
        }

        alertIterator++;
    }

    return (maxThreatLevel);

}
