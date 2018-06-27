#ifndef AWL_RECEIVER_POSTPROCESSOR_H
#define AWL_RECEIVER_POSTPROCESSOR_H

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

#include "ReceiverCapture.h"


using namespace std;

namespace awl {

    /** \brief   The ReceiverPostProcessor is used to read the detection and track information acquired from the
      *          Receivers and return "enhanced" detection data.
      *	\remarks The enhanced detection data is built with information that the receivers cannot provide due to
      *		   implementation limitations, suck as threat level or supplemental geometry information (distance from
      *        other objects dependent on placement).
     */

    class ReceiverPostProcessor {
    public:
        ReceiverPostProcessor() noexcept = default;

        // Build processsed detections from the current track set.
        // Only detections from tracks that are complete (no errors are kept)
        // Detections from tracks where distance is oustide the range are not kept.
        // Return true if all tracks have been intepreted correctly.
        // Return false if we have found incomplete tracks or invalid channel info.
        bool GetEnhancedDetectionsFromFrame(ReceiverCapture::Ptr receiver, FrameID inFrameID,
                                            Publisher::SubscriberID inSubscriberID, Detection::Vector& detectionBuffer);

    protected:
        // Complete the track info that was not processed by the AWL Module.
        // Return true if all tracks have been intepreted correctly.
        // Return false if we have found incomplete tracks or invalid channel info.
        bool CompleteTrackInfo(SensorFrame::Ptr currentFrame);

        // Rebuild processsed detections from the current track set.
        // Only detections from tracks that are complete (no errors are kept)
        // Detections from tracks where distance is oustide the range are not kept.
        // Return true if all tracks have been intepreted correctly.
        // Return false if we have found incomplete tracks or invalid channel info.
        bool BuildEnhancedDetectionsFromTracks(SensorFrame::Ptr currentFrame, Detection::Vector& outDetections);

        /** \brief Predict the time to collision (distance = 0) between sensor and obstacle,
      *        given current distance and speed, assuming constant deceleration.
       * \param[in] currentDistance to obstacle, in meters
       * \param[in] relativeSpeed relative speed between sensor and obstacle, in meters/sec
      * \return Predicted time to collision, in seconds.
      * \remarks This corresponds to TTC1 measure defined in:
      *          Yizhen Zhang, Erik K. Antonsson and Karl Grote: A New Threat Assessment Measure for Collision Avoidance Systems
      */

        float ReceiverPostProcessor::PredictTimeToCollisionConstant(float currentDistance, float relativeSpeed);

        /** \brief Predict the relative distance of an obstacle after a certain time delay,
          *        given current distance speed and  deceleration.
           * \param[in] currentDistance to obstacle, in meters
           * \param[in] relativeSpeed relative speed between sensor and obstacle, in meters/sec
           * \param[in] currentAcceleration current acceleration between sensor and obstacle, in meters/sec2
          * \param[in] timeDelay timeDelay (in sec) for which updated position is calculated
          * \return Predicted relative distance of the obstacle, , in m/s2.
          * \remarks  arguments use acceleration, not deceleration! Positive acceleration means object is moving away.
          */

        float ReceiverPostProcessor::PredictDistance(float currentDistance, float relativeSpeed, float acceleration,
                                                     float time);

        /** \brief Calculate the required accceleration to get to zero speed at the specified distance,
          *        given currentSpeed and current deceleration.
           * \param[in] currentDistance to obstacle, in meters
           * \param[in] relativeSpeed relative speed between sensor and obstacle, in meters/sec
           * \param[in] currentDeceleration current deceleration between sensor and obstacle, in meters/sec2
          * \return Current required acceleration, in m/s2. Should be a negative value for objects movig towards sensor.
          * \remarks  arguments use acceleration, not deceleration! Positive acceleration means object is moving away.
          */
        float CalculateAccelerationToStop(float currentDistance, float relativeSpeed, float currentAcceleration);

        /** \brief Calculate the predicted time to collision to an obstacle to get to zero speed at the specified distance,
          *        given currentSpeed and current deceleration, projected in deltaTime seconds.
           * \param[in] distance to obstacle, in meters
           * \param[in] speed relative speed between sensor and obstacle, in meters/sec
           * \param[in] acceleration current acceleration between sensor and obstacle, in meters/sec2
           * \param[in] deltaTime time at which the prediction is claculated for (in seconds from current time)
          * \return Time to collision predicted at (currentTime + deltaTime)
          * \remarks  arguments use acceleration, not deceleration! Positive acceleration means object is moving away.
          */
        float PredictTTC(float distance, float speed, float acceleration, float deltaTime);


    };

} // namespace awl
#endif // AWL_RECEIVER_POSTPROCESSOR_H
