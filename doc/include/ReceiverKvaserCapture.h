#ifndef AWL_RECEIVER_KVASER_CAPTURE_H
#define AWL_RECEIVER_KVASER_CAPTURE_H

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

#include <canstat.h>  // Kvaser CAN library
#include <TimeAndThreadTestUtil.h>

#include "ReceiverCANCapture.h"

using namespace std;

namespace awl {

/** \brief TheReceiverCANCapture class is a specialized implementation of the ReceiverCapture
   *        used to acquire data from AWL LIDAR modules	through the Kvaser CAN bus interface.
  * \author Jean-Yves Desch�nes
  */
    class ReceiverKvaserCapture : public ReceiverCANCapture {

// Public types
    public:
        typedef boost::shared_ptr <ReceiverKvaserCapture> Ptr;
        typedef boost::shared_ptr <ReceiverKvaserCapture> ConstPtr;
        typedef int KvaserHandle;

// public Methods
    public:
        /** \brief ReceiverKvaserCapture constructor.
             * \param[in] inReceiverID  unique receiverID
            * \param[in] inReceiverChannelQty number of channels in the receiver
            * \param[in] inKvaserChannel CAN Channel id at the Kvaser level
            * \param[in] inCANRate bit rate of the Receiver Unit (in kBPS).  See ReceiverCANCapture for allowable values (default should be 1MBps).
            * \param[in] inFrameRate frameRate of the receiver
            * \param[in] inChannelMask  channelMask indicating which channels are activated in the receiver
            * \param[in] inMessageMask mask of the messages that are enabled in the communications protocol
            * \param[in] inRangeOffset rangeOffset that corresponds to a calibration error in the sensor.
            *                          Will automatically be added to any range received.
            * \param[in] inRegistersFPGA default description of the FPGA registers
            * \param[in] inRegistersADC default description of the ADC registers
            * \param[in] inRegistersGPIO default description of the GPIO registers
            * \param[in] inParametersAlgos default description if the algorithm parameters
          */

        ReceiverKvaserCapture(int receiverID, int inReceiverChannelQty, const int inKvaserChannel,
                              ReceiverCANCapture::eReceiverCANRate inCANRate,
                              int inFrameRate, ChannelMask& inChannelMask, MessageMask& inMessageMask,
                              float inRangeOffset,
                              const RegisterSet& inRegistersFPGA, const RegisterSet& inRegistersADC,
                              const RegisterSet& inRegistersGPIO, const AlgorithmSet& inParametersAlgos);

        /** \brief ReceiverKvaserCapture constructor from a configuration file information.
             * \param[in] inReceiverID  unique receiverID
            * \param[in] propTree propertyTree that contains teh confoguration file information.
          */

        ReceiverKvaserCapture(int receiverID, boost::property_tree::ptree& propTree);


        /** \brief ReceiverKvaserCapture Destructor.  Insures that all threads are stopped before destruction.
          */
        virtual ~ReceiverKvaserCapture();

    protected:

        /** \brief Do one iteration of the thread loop.
          */
        virtual void DoOneThreadIteration();

        /** \brief Open the CAN port
          * \returns true if the port is successfully opened, false otherwise.
          * \remarks Once the port is successfully opened, use the "reader" pointer to access the can data.
          *          If opening the port fails, reader is set to NULL.
          */
        virtual bool OpenCANPort();


        /** \brief Closes the CAN port and associated objects.
          * \returns true if the port is successfully closed, false otherwise.
          */
        virtual bool CloseCANPort();


        /** \brief Synchronous write of a CAN message in the stream
           * \param[in] outString  Message to send
          * \return true if the function was successful. false otherwise.
          */
        virtual bool WriteMessage(const AWLCANMessage& inMsg);


        /** \brief Print an error status message to the debug file
           * \param[in] id  Message to format
          * \param[in] canStatus type of error code.
          * \return true iof the function was successful. false otherwise.
          */
        void logIfAnErrorOccurred(char* id, canStatus status);

        /** \brief Reads the configuration proerties from the configuration file
      * \param[in] propTree the boost propertyTree created from reading the configuration file.
      * \returns Returns true otherwise.
      * \throws  Throws boost error on read of the property keys.
      */
        virtual bool ReadConfigFromPropTree(boost::property_tree::ptree& propTree);

        /** \brief Converts the ReceiverCANCapture canRate into a KVaser-specific rate code, stored in the canBitRateCode member.
          */
        void ConvertKvaserCANBitRateCode();

// Protected variables
    protected:
        /** \brief Kvaser CAN Channel */
        int canChannelID;

        /** \brief Kvaser CAN BitRate code, as defined in canlib.h. Default is canBITRATE_1M*/
        long canBitRateCode;

        /** \brief Kvaser handle to the communication port */
        KvaserHandle kvaserHandle;

        /** \brief Time-out without an input message after which we try to reconnect the serial port. */
        chrono::time_point reconnectTime;

        /** \brief counter in the closeCanPort() call, used to avoid reentry during thread close */
        int closeCANReentryCount;

        void updateReconnectTimeAfterDelay();

        void attemptReconnection();
    };

} // namespace AWL

#endif //AWL_RECEIVER_KVASERC_CAPTURE_H
