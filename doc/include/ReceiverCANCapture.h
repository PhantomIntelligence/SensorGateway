#ifndef AWL_RECEIVER_CAN_CAPTURE_H
#define AWL_RECEIVER_CAN_CAPTURE_H

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

#include "ReceiverCapture.h"

#define ALGO_QTY 4
#define GLOBAL_PARAMETERS_INDEX 0

static const int COMMAND_MESSAGE_ID = 80;
namespace awl {

// CAN Frame
//    typedef struct {
//        unsigned long id;        // Message id
//        unsigned long timestamp; // timestamp in milliseconds
//        unsigned char flags;     // [extended_id|1][RTR:1][reserver:6]
//        unsigned char len;       // Frame size (0.8)
//        unsigned char data[8]; // Databytes 0..7
//    } AWLCANMessage;

    using AWLCANMessage = SensorCommunication::AWL::CAN::Message;


/** \brief TheReceiverCANCapture class is a virtual base class that is a specialized implementation of the ReceiverCapture
   *        It implements the mechanics for acquisition of CAN data messages.
   *
   *        Derived classes that wish to implement a CAN interface must define methods for:
   *        OpenCANPort(), CloseCANPort, WriteMessage(), DoOneThreadIteration() and ReadConfigFromPropTree().
   *
   *        To parse incoming messages:
   *            In DoOneThreadIteration(), once a CAN Message has been read and Formatted into a AWLCANMessage,
   *            the derived classes can call ParseMessage() to continue the interpretation of the messages.
   *            In DoOneThreadIteration(), derived classes should also handle the disconnection / reconnection of the
   *            CAN port in a robust manner.
   *
   *        To Write outgoing messages:
   *            Write the appropriate WriteMessage() to send an AWLCANMessage to the CAN device.
   *
   *        Use the ReceiverEasySyncCapture class as a reference for deriving CAN-Based Receivers.
  * \author Jean-Yves Desch�nes
  */
    class ReceiverCANCapture : public ReceiverCapture {

    public:
        typedef std::shared_ptr<ReceiverCANCapture> Ptr;
        typedef std::shared_ptr<ReceiverCANCapture> ConstPtr;

        typedef enum eReceiverCANRate {
            canRate1Mbps = 1000,
            canRate500kbps = 500,
            canRate250kbps = 250,
            canRate125kbps = 125,
            canRate100kbps = 100,
            canRate50kbps = 50,
            canRate10kps = 10
        } eReceiverCANRate;

        /** \brief ReceiverCANCapture constructor.
             * \param[in] inReceiverID  unique receiverID
            * \param[in] inReceiverChannelQty number of channels in the receiver
            * \param[in] inCANRate CAN Communitcations baud rate of the receiver
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

        ReceiverCANCapture(int receiverID, int inReceiverChannelQty, eReceiverCANRate inCANRate, int inFrameRate,
                           ChannelMask& inChannelMask, MessageMask& inMessageMask, float inRangeOffset,
                           const RegisterSet& inRegistersFPGA, const RegisterSet& inRegistersADC,
                           const RegisterSet& inRegistersGPIO, const AlgorithmSet& inParametersAlgos);

        /** \brief ReceiverCANCapture constructor from a configuration file information.
             * \param[in] inReceiverID  unique receiverID
            * \param[in] propTree propertyTree that contains teh confoguration file information.
          */

        ReceiverCANCapture(int receiverID, boost::property_tree::ptree& propTree);


        /** \brief ReceiverCANCapture Destructor.  Insures that all threads are stopped before destruction.
          */
        virtual ~ReceiverCANCapture();

        /** \brief Start the lidar Data Projection  thread
          */
        virtual void Go();

        /** \brief Stop the lidar data projection thread
          */
        virtual void Stop();

    public:
        /** \brief Parse the CAN messages and call the appropriate processing function
             * \param[in] inMsg  CAN message contents
          */
        void ParseMessage(AWLCANMessage& inMsg);

        /** \brief Sets the playback filename at the receiver device level.
      * \param[in] inPlaybackFileName the name for the playback file.
      * \return true if success.  false on error
     */
        virtual bool SetPlaybackFileName(std::string inPlaybackFileName);


        /** \brief Sets the record filename at the receiver device level.
          * \param[in] inRecordFileName the name for the playback file.
          * \return true if success.  false on error
         */
        virtual bool SetRecordFileName(std::string inRecordFileName);

        /** \brief Starts the playback of the file specified in the last SetPlaybackFileName() call.
          * \param[in] frameRate playback frame rate. Ignored on some implementations of AWL.
          * \param[in] channelMask mask for the channels that will be played back. that an empty channelMask is equivalent to StopPlayback().
          * \return true if success.  false on error
           * \remarks status of playback is updated in the receiverStatus member.
          * \remarks File is recorded locally on SD Card.
         */
        virtual bool StartPlayback(uint8_t frameRate, ChannelMask channelMask);

        /** \brief Starts the record of a file whose name was set using the last SetRecordFileName() call.
          * \param[in] frameRate recording frame rate. Ignored on some implementations of AWL (in this case, default frame rate is used).
          * \param[in] channelMask mask for the recorded channels. that an empty channelMask is equivalent to StopRecord().
          * \return true if success.  false on error
          * \remarks status of record is updated in the receiverStatus member.
          * \remarks File is recorded locally on SD Card.
         */
        virtual bool StartRecord(uint8_t frameRate, ChannelMask channelMask);

        /** \brief Stops any current playback of a file.
          * \return true if success.  false on error
           * \remarks status of playback is updated in the receiverStatus member.
            * \remarks After playback is interrupted, the unit should return to the default acquisition mode.
        */
        virtual bool StopPlayback();


        /** \brief Stops any current recording.
          * \return true if success.  false on error
            * \remarks status of recording is updated in the receiverStatus member.
           * \remarks After recording, the unit should return to the default acquisition mode.
        */
        virtual bool StopRecord();

        /** \brief Starts the internal calibration of the system.
          * \param[in] frameQty number of frames on which calibration is calculated
          * \param[in] beta beta parameter for the calibration
          * \param[in] channelMask mask for the recorded channels. that an empty channelMask is equivalent to StopRecord().
          * \return true if success.  false on error
          * \remarks Calibration file is recorded locally on SD Card.
         */
        virtual bool StartCalibration(uint8_t frameQty, float beta, ChannelMask channelMask);


        /** \brief Issues the command to set the current algorithm in the FPGA.
          *\param[in] algorigthmID  id of the selected algorithm.
        * \return true if success.  false on error.
        */

        virtual bool SetAlgorithm(uint16_t algorithmID);

        /** \brief Sets an internal FPGA register to the value sent as argument.
          *\param[in] registerAddress Adrress of the register to change.
          *\param[in] registerValue Value to put into register.
        * \return true if success.  false on error.
        */

        virtual bool SetFPGARegister(uint16_t registerAddress, uint32_t registerValue);

        /** \brief Sets an ADC register to the value sent as argument.
          *\param[in] registerAddress Adrress of the register to change.
          *\param[in] registerValue Value to put into register.
        * \return true if success.  false on error.
        */
        virtual bool SetADCRegister(uint16_t registerAddress, uint32_t registerValue);

        /** \brief Sets an internal GPIO register to the value sent as argument.
          *\param[in] registerAddress Adrress of the register to change.
          *\param[in] registerValue Value to put into register (values accepted are 0-1).
        * \return true if success.  false on error.
        */

        virtual bool SetGPIORegister(uint16_t registerAddress, uint32_t registerValue);

        /** \brief Sets algorithm parameters to the value sent as argument.
          *\param[in] algoID id of the detection algo affected by the change.
          *\param[in] registerAddress Adrress of the parameter to change.
          *\param[in] registerValue Value to put into register (values accepted are 0-1).
        * \return true if success.  false on error.
        */

        virtual bool SetAlgoParameter(int algoID, uint16_t registerAddress, uint32_t registerValue);

        /** \brief Sets global  algorithm parameters to the value sent as argument.
          *\param[in] registerAddress Adrress of the parameter to change.
          *\param[in] registerValue Value to put into register (values accepted are 0-1).
        * \return true if success.  false on error.
        */

        virtual bool SetGlobalAlgoParameter(uint16_t registerAddress, uint32_t registerValue);

        /** \brief Changes the controls of which messages are sent from AWL to the client to reflect provided settings
        * \param[in] frameRate new frame rate for the system. A value of 0 means no change
        * \param[in] channelMask mask for the analyzed channels.
        * \param[in] messageMask mask identifies which groups of target/distance/intensity messages are transmitted over CAN.
        * \return true if success.  false on error.
        */

        virtual bool SetMessageFilters(uint8_t frameRate, ChannelMask channelMask, MessageMask messageMask);


        /** \brief Issues an asynchronous query command to get the current algorithm.
        * \return true if success.  false on error.
        */
        virtual bool QueryAlgorithm();


        /** \brief Send an asynchronous query command for an internal FPGA register.
             *\param[in] registerAddress Adrress of the register to query.
          * \return true if success.  false on error.
          * \remarks On reception of the answer to query the register address and value will be
          *          placed in the receiverStatus member and in globalSettings.
            */
        virtual bool QueryFPGARegister(uint16_t registerAddress);

        /** \brief Send an asynchronous query command for an ADC register.
             *\param[in] registerAddress Adrress of the register to query.
          * \return true if success.  false on error.
          * \remarks On reception of the answer to query the register address and value will be
          *          placed in the receiverStatus member and in globalSettings.
            */
        virtual bool QueryADCRegister(uint16_t registerAddress);

        /** \brief Send an asynchronous query command for a GPIO register.
             *\param[in] registerAddress Adrress of the register to query.
          * \return true if success.  false on error.
          * \remarks On reception of the answer to query the register address and value will be
          *          placed in the receiverStatus member and in the globalSettings.
            */
        virtual bool QueryGPIORegister(uint16_t registerAddress);

        /** \brief Send an asynchronous query command for an algorithm parameter.
              *\param[in] algoID id of the detection algo for which we want to query.
             *\param[in] registerAddress Adrress of the register to query.
          * \return true if success.  false on error.
          * \remarks On reception of the answer to query the register address and value will be
          *          placed in the receiverStatus member and in the globalSettings.
            */
        virtual bool QueryAlgoParameter(int algoID, uint16_t registerAddress);

        /** \brief Send an asynchronous query command for a global algorithm parameter.
          *\param[in] algoID id of the detection algo for which we want to query.
         *\param[in] registerAddress Adrress of the register to query.
      * \return true if success.  false on error.
      * \remarks On reception of the answer to query the register address and value will be
      *          placed in the receiverStatus member and in the globalSettings.
        */
        virtual bool QueryGlobalAlgoParameter(uint16_t registerAddress);
// Protected methods
    protected:

        /** \brief Return the lidar data rendering thread status
          * \return true if the lidar data rendering thread is stoppped.
          */
        virtual void DoThreadLoop();

        /** \brief Do one iteration of the thread loop.
          *        Acquire CAN Data until ParseMessage() can be called with a CAN Message.
          *        Try to manage automatic connection/reconnection of the CAN communications in the loop.
          */
        virtual void DoOneThreadIteration() = 0;

        /** \brief Read the sensor status message (001)
             * \param[in] inMsg  CAN message contents
          */
        void ParseSensorStatus(AWLCANMessage& inMsg);

        /** \brief Read the sensor boot status message (002)
             * \param[in] inMsg  CAN message contents
          */
        void ParseSensorBoot(AWLCANMessage& inMsg);

        /** \brief Read the distance readings from CAN messages (20-26 30-36)
             * \param[in] inMsg   CAN message contents
          */
        void ParseChannelDistance(AWLCANMessage& inMsg);


        /** \brief Read the intensity readings from CAN messages (40-46 50-56)
             * \param[in] inMsg   CAN message contents
         */
        void ParseChannelIntensity(AWLCANMessage& inMsg);

        /** \brief Read the obstacle track description message (10). If required, creste the corresponding Track object
             * \param[in] inMsg   CAN message contents
         */
        void ParseObstacleTrack(AWLCANMessage& inMsg);

        /** \brief Read the obstacle speed/velocity  message (11). If required, creste the corresponding Track object
             * \param[in] inMsg   CAN message contents
         */
        void ParseObstacleVelocity(AWLCANMessage& inMsg);

        /** \brief Read the obstacle size/intensity message (12). If required, creste the corresponding Track object
             * \param[in] inMsg   CAN message contents
         */
        void ParseObstacleSize(AWLCANMessage& inMsg);

        /** \brief Read the obstacle angular position message (31). If required, creste the corresponding Track object
             * \param[in] inMsg   CAN message contents
         */
        void ParseObstacleAngularPosition(AWLCANMessage& inMsg);


        /** \brief Parse control messages (80) and dispatch to appropriate handling method
             * \param[in] inMsg   CAN message contents
         */
        void ParseControlMessage(AWLCANMessage& inMsg);

        /** \brief Process the debug / parameter / set_parameter message (0xC0)
             * \param[in] inMsg   CAN message contents
            * \ remarks	This message is normally a master message.  It is ignored.
         */
        void ParseParameterSet(AWLCANMessage& inMsg);

        /** \brief Process the debug / parameter / Query message (0xC1)
             * \param[in] inMsg   CAN message contents
            * \ remarks	This message is normally a master message.  It is ignored.
         */
        void ParseParameterQuery(AWLCANMessage& inMsg);

        /** \brief Process the debug / parameter / response message (0xC2)
             * \param[in] inMsg   CAN message contents
         */
        void ParseParameterResponse(AWLCANMessage& inMsg);

        /** \brief Process the debug / parameter / error message (0xC3)
             * \param[in] inMsg   CAN message contents
            * \remarks  The error flag is set in the receiverStatus.
         */
        void ParseParameterError(AWLCANMessage& inMsg);


        void ParseParameterAlgoSelectResponse(AWLCANMessage& inMsg);

        void ParseParameterAlgoParameterResponse(AWLCANMessage& inMsg);

        void ParseParameterFPGARegisterResponse(AWLCANMessage& inMsg);

        void ParseParameterBiasResponse(AWLCANMessage& inMsg);

        void ParseParameterADCRegisterResponse(AWLCANMessage& inMsg);

        void ParseParameterPresetResponse(AWLCANMessage& inMsg);

        void ParseParameterGlobalParameterResponse(AWLCANMessage& inMsg);

        void ParseParameterGPIORegisterResponse(AWLCANMessage& inMsg);

        void ParseParameterDateTimeResponse(AWLCANMessage& inMsg);

        void ParseParameterRecordResponse(AWLCANMessage& inMsg);

        void ParseParameterPlaybackResponse(AWLCANMessage& inMsg);

        void ParseParameterAlgoSelectError(AWLCANMessage& inMsg);

        void ParseParameterAlgoParameterError(AWLCANMessage& inMsg);

        void ParseParameterFPGARegisterError(AWLCANMessage& inMsg);

        void ParseParameterBiasError(AWLCANMessage& inMsg);

        void ParseParameterADCRegisterError(AWLCANMessage& inMsg);

        void ParseParameterPresetError(AWLCANMessage& inMsg);

        void ParseParameterGlobalParameterError(AWLCANMessage& inMsg);

        void ParseParameterGPIORegisterError(AWLCANMessage& inMsg);

        void ParseParameterDateTimeError(AWLCANMessage& inMsg);

        void ParseParameterRecordError(AWLCANMessage& inMsg);

        void ParseParameterPlaybackError(AWLCANMessage& inMsg);

        /** \brief Open the CAN port
          * \returns true if the port is successfully opened, false otherwise.
          * \remarks Once the port is successfully opened, use the "reader" pointer to access the can data.
          *          If opening the port fails, reader is set to NULL.
          */
        virtual bool OpenCANPort()  = 0;


        /** \brief Closes the CAN port and associated objects.
          * \returns true if the port is successfully closed, false otherwise.
          */
        virtual bool CloseCANPort() = 0;

        /** \brief Synchronous write of a CAN message in the stream
           * \param[in] outString  Message to send
          * \return true iof the function was successful. false otherwise.
          */
        virtual bool WriteMessage(const AWLCANMessage& inMsg) = 0;

        /** \brief Put the current date and time to the CAN port
           * \return true iof the function was successful. false otherwise.
         */
        bool WriteCurrentDateTime();

        /** \brief Get a byte in hex format from the text response string supplied by the EasySync and convert it into
          *        a uint8_t value.  Used in parsing the EasySync response lines.
          * \param[in] inResponse  EasySync response string that corresponds to a CAN message.
          * \param[out] outByte  interpreted value of the substring.
          * \param[in] startIndex  Index of the substring that we want to parse within the string.
          * \param[in] len length of the substring to be interpreted.  Should be 1 or 2.
          *                 Default is 1. Values exceeding 2 are limited to 2 characters.
           * \return true if the function was successful (the substring is a valid hex number) . False otherwise.
         */
        bool GetDataByte(std::string& inResponse, uint8_t& outByte, int startIndex, int len = 1);

        /** \brief Parse a message id (which is an unsigned long in hex format) from the text response string supplied
          *        by the EasySync and convert it into a an unsigned_long value.
          *        Used in parsing the EasySync response lines.
          * \param[in] inResponse  EasySync response string that corresponds to a CAN message.
          * \param[out] outID  interpreded messageID extracted from the substring.
          * \param[in] startIndex  Index of the substring that we want to parse within the string.
           * \return true if the function was successful (the substring is a valid hex number) . False otherwise.
         */
        bool GetStandardID(std::string& inResponse, unsigned long& outID, int startIndex);

        /** \brief Reads the configuration proerties from the configuration file
          * \param[in] propTree the boost propertyTree created from reading the configuration file.
          * \returns Returns true otherwise.
          * \throws  Throws boost error on read of the property keys.
          */
        virtual bool ReadConfigFromPropTree(boost::property_tree::ptree& propTree) override;

        /** \brief Reads the description of registers (FPGA, ADC and GPIO) and controllable algorithm parameters
          *        from the configuration file.
          * \param[in] propTree the boost propertyTree created from reading the configuration file.
          * \returns Returns false in case of a read of the property tree, when all the register description is absent or not found.  Returns true otherwise.
          * \throws  Throws boost error on read of the property keys orther than the root key.
          */
        virtual bool ReadRegistersFromPropTree(boost::property_tree::ptree& propTree) override;

// Protected variables
    protected:
        /** \brief All dates received from the CAN devices are sent as offsets from 1900. Add the yearOffset to convert to
          *        POSIX-Compliant years. This may change depending on device versions*/
        uint16_t yearOffset;

        /** \brief All months received from the CAN devices are sent starting from 0.
          *        POSIX starts at 1. Add the monthOffset to convert to
          *        POSIX-Compliant months. This may change depending on device versions*/
        uint16_t monthOffset;


        /** \brief counter in the closeCanPort() call, used to avoid reentry iduring thread close */
        int closeCANReentryCount;

        /** \brief CAN Rate
          *
         */
        eReceiverCANRate canRate;

        void updateReceiverStatusWithSensorStatus(const uint16_t* uintDataPtr, const uint8_t* byteDataPtr,
                                                  const int16_t* intDataPtr,
                                                  int& iTemperature, unsigned int& uiVoltage);

        void updateReceiverStatusWithSensorBoot(const uint8_t* byteDataPtr);

        void updateDetectionWithChannelDistance(int channel, int detectOffset, const uint16_t* distancePtr);

        void updateDetectionWithChannelIntensity(int channel, int detectOffset, const uint16_t* intensityPtr);

        void LogAndUpdateCommandError(const char* origin, const unsigned char errorCode);
    };

} // namespace AWL

#endif
