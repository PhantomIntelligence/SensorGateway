#ifndef AWL_RECEIVER_CAPTURE_H
#define AWL_RECEIVER_CAPTURE_H

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

#include "Publisher.h"

#ifndef Q_MOC_RUN

#include <boost/property_tree/ptree.hpp>

#endif


namespace {
    const int maxReceiverFrames = 100;
}

namespace awl {

    using ReceiverStatus = SensorCommunication::AWL::SensorStatus;
    using SensorCommunication::AWL::ChannelMask;
    using SensorCommunication::AWL::MessageMask;
/** \brief RegisterSetting is an internal representation of an AWL register for an internal device
*/

    typedef struct RegisterSetting {
        /** \brief String used to describe the index.
          *        This string corresponds to the index identifier in the device, typically used in the user manual.
         * \remarks Can be alphanumeric or numeric or hex string (ex: "R0", "0x1A", "09")..
        */
        std::string sIndex;

        /** \brief String used to describe the register function.
        */
        std::string sDescription;

        /** \brief Physical address of the register.
         * \remarks On some devices, the physical address may be different than the register index. (Ex:multibyte registers).
        */
        uint16_t address;

        /** \brief Current value of the register.
        */
        uint32_t value;

        /** \brief Count of the messages demanding a change in the value, that have not been acknowledged by the
          *        AWL moldule. A non-zero count indicates that the value may not have been received and reflected in the module yet.
        */
        int pendingUpdates;
    }
            RegisterSetting;

/** \brief A RegisterSet is a container of related RegisterSettings
*    \remarks For example,  a typical AWL receiver provides controls for FPGA registers, ADC registers and GPIO registers.
*             Each of these devices is represented in a RegisterSet.
*/
    typedef std::vector<RegisterSetting> RegisterSet;


/** \brief The AlgoParamType describles the type of Algorithm parameters, which are either  are either float or int values.
*/
    typedef enum {
        eAlgoParamInt = 0,  // Parameter is an  integer
        eAlgoParamFloat = 1 // Parameter is a floating point value
    }
            AlgoParamType;

/** \brief The AlgorithmParameter describes a parameter value for the internal AWL algorithms.
*/
    typedef struct AlgorithmParameter {
        /** \brief String used to describe the parameter index.
         * \remarks Should be numeric - formatted for display purposes.
        */
        std::string sIndex;

        /** \brief Identifier of the parameter for the algorithm.
         * \remarks Parameteres are proprietary and are not further described in documentation.
        */
        uint16_t address;

        /** \brief Short description string opf the parameter.
         * \remarks Parameteres are proprietary and are not further described in documentation.
        */
        std::string sDescription;

        /** \brief Indication of the storage type of the parameter. Can be  eAlgoParamInt or eAlgoParamFloat.
        */
        AlgoParamType paramType;

        /** \brief Storage area for parameters of whose type is eAlgoParamInt.
        */
        uint32_t intValue;

        /** \brief Storage area for parameters of whose type is eAlgoParamFloat.
        */
        float floatValue;

        /** \brief Count of the messages demanding a change in the value, that have not been acknowledged by the
          *        AWL moldule. A non-zero count indicates that the value may not have been received and reflected in the module yet.
        */
        int pendingUpdates;
    }
            AlgorithmParameter;

/** \brief An AlgorithmParameterVector is a container of related AlgorithmParameters
*/

    typedef std::vector<AlgorithmParameter> AlgorithmParameterVector;

/** \brief An AlgorithmDescription describes a detection Algorithm and all of its controllable parameters.
*/
    typedef struct AlgorithmDescription {
        /** \brief Algorithm Name descriptive string. Used for display and documentation purposes only.
        */
        std::string sAlgoName;

        /** \brief Internal identifier of the algorithm.
        */
        uint16_t algoID;

        /** \brief Description of all controllable parameters for that algorithm.
        */
        AlgorithmParameterVector parameters;
    }
            AlgorithmDescription;

/** \brief An AlgorithmSet is a container describing all the available detection algorithm and their associated parameters.
*    \remarks For experimental purposes, the AWL modules may host different detection algorithms, which can be selected through
*             the communications protocol.
*             The performance of each algorithm can be "tweaked" by altering the operation paramneters.
*             Beware, as units may "store" the parameter changes, which will become the default operating parameters
*             at boot time.  Those changes may not be reflected in the configuration files.
*/
    typedef struct AlgorithmSet {
        /** \brief Default displayed algo in the user interface.
        */
        int defaultAlgo;

        /** \brief Container of all the Algorithm descriptions.
        */
        std::vector<AlgorithmDescription> algorithms;
    }
            AlgorithmSet;

/** \brief ReceiverCapture class is an abstract class for all classes used to acquire data from physical LIDAR units.
  *        The ReceiverCapture acquires LIDAR sensor data in SensorFrames.
  *        It buffers up a few frames to facilitate processing afterwards.
  *        The ReceiverCapture also manages a "local" copy of the status indicators of the LIDAR units.
  *        The ReceiverCapture also stores a "local"  copy of all Registers and Algorithm parameters of the LIDAR unit
  *        that can be controlled via its communications protocols.
  *        Finally, the ReceiverCapture manages optional "logging" of the track and distance data into a local log file on the PC
  *
  *        Children classes will implement actual physical units based on their specific communications protocols and
  *        software extensions.
  *
  * \author Jean-Yves Desch�nes
  */
    class ReceiverCapture : public Publisher {
    public:

        static const int maximumSensorFrames;  // Maximum number of frames kept in frame data-flow
        typedef std::shared_ptr<ReceiverCapture> Ptr;
        typedef std::shared_ptr<ReceiverCapture> ConstPtr;
        typedef std::vector<ReceiverCapture::Ptr> List;
        typedef ReceiverCapture::List* ListPtr;

    public:
        /** \brief ReceiverCapture constructor from user supplied values.
             * \param[in] inReceiverID  unique receiverID
            * \param[in] inReceiverChannelQty number of channels in the receiver
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

        ReceiverCapture(int receiverID, int inReceiverChannelQty,
                        int inFrameRate, ChannelMask& inChannelMask, MessageMask& inMessageMask, float inRangeOffset,
                        const RegisterSet& inRegistersFPGA, const RegisterSet& inRegistersADC,
                        const RegisterSet& inRegistersGPIO,
                        const AlgorithmSet& inParametersAlgos);

        /** \brief ReceiverCapture constructor from a configuration file information.
             * \param[in] inReceiverID  unique receiverID
            * \param[in] propTree propertyTree that contains teh confoguration file information.
          */

        ReceiverCapture(int receiverID, boost::property_tree::ptree& propTree);

        /** \brief ReceiverCapture Destructor.  Insures that all threads are stopped before destruction.
          */
        virtual ~ReceiverCapture();

        /** \brief Start the lidar Data Projection  thread
          */
        virtual void Go();

        /** \brief Return the receiver ID.
          * \return receiverID used for other system structures.
          */
        int GetReceiverID() { return receiverID; };

        /** \brief Return the number of frames acquired
          * \return int indicating the number of frames.
          */
        int GetFrameQty();

        /** \brief Return the number of receiver channels used for video projection
          * \return int indicating the number of channels.
          */
        virtual int GetChannelQty() { return receiverChannelQty; };


        /** \brief copy the frame identified by frameID to to a local copy (thread-safe)
         * \param[in] inFrameID frame identificator of the requiested frame
           \param[out] outSensorFrame ChannelFram structure to which the data is copied.
           \param[in] inSubscriberID subscriber info used to manage the update information and thread locking.
         * \return True if channel data is copied successfully. False if frame corresponding to inFrameID or channel data not found
         */
        bool ReceiverCapture::CopyReceiverFrame(FrameID inFrameID, SensorFrame::Ptr& outSensorFrame,
                                                Publisher::SubscriberID inSubscriberID);

        /** \brief copy the raw detection data identified with a frameID to to a local copy (thread-safe)
         * \param[in] inFrameID frame identificator of the requiested frame
           \param[out] outChannelFrame ChannelFram structure to which the data is copied.
           \param[in] inSubscriberID subscriber info used to manage the update information and thread locking.
         * \return True if channel data is copied successfully. False if frame corresponding to inFrameID or channel data not found
         */
        virtual bool CopyReceiverRawDetections(FrameID inFrameID, Detection::Vector& outDetections,
                                               Publisher::SubscriberID inSubscriberID);

        /** \brief copy the channel status informationidentified with a frameID to to a local copy (thread-safe)
         * \param[in] inFrameID frame identificator of the requiested frame
         * \param[in] inChannelID index of the required channel
           \param[out] outChannelFrame ChannelFram structure to which the data is copied.
           \param[in] inSubscriberID subscriber info used to manage the update information and thread locking.
         * \return True if channel data is copied successfully. False if frame corresponding to inFrameID or channel data not found
         */
        virtual bool CopyReceiverStatusData(ReceiverStatus& outStatus, Publisher::SubscriberID inSubscriberID);

        /** \brief Return the current frame identification number for informational purposes
         * \return Current frame identification number.
            \remark Note that the frameID corresponds to the "incomplete" frame currently being assembled.
                    For the last complete frame, useGetLastFrameID();
         */
        virtual FrameID GetFrameID() { return (frameID); };

        /** \brief Return the  frame identification number for the frame located at inFrameIndex
          * \param[in] inFrameIndex index of the requiested frame
        * \return Current frame identification number.
            \remark Note that the frameID corresponds to the "incomplete" frame currently being assembled.
                    For the last complete frame, useGetLastFrameID();
         */
        virtual FrameID GetFrameID(int inFrameIndex);

        /** \brief Return theframe identification number of the last complete frame assembled
         * \return Last complete frame identification number.
            */
        virtual FrameID GetLastFrameID() { return (acquisitionSequence->GetLastFrameID()); };

        /** \brief Return the time elapsed, in milliseconds, since the start of thread.
            \return time in milliseconds since tthe start of thread.
         */
        double GetElapsed();

        /** \brief Add an offet to the distance measurements.  This is different from the sensor depth, which is distance from bumper.
          * \param[in] inMeasurementOffset measurementOffset introduced by detection algorithm
          * \remark measurement offset is an offset in distance from sensor caused by the nature of algorithm used.
          */
        void SetMeasurementOffset(double inMeasurementOffset);

        /** \brief Get the measurement offset  in meters.
          * \param[out] outMeasurementOffset measurementOffset introduced by detection algorithm.
          */
        void GetMeasurementOffset(double& outMeasurementOffset);

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

        /** \brief Starts the internal calibration of the system.
          * \param[in] frameQty number of frames on which calibration is calculated
          * \param[in] beta beta parameter for the calibration
          * \param[in] channelMask mask for the recorded channels. that an empty channelMask is equivalent to StopRecord().
          * \return true if success.  false on error
          * \remarks Calibration file is recorded locally on SD Card.
         */
        virtual bool StartCalibration(uint8_t frameQty, float beta, ChannelMask channelMask) = 0;


        /** \brief Stops any current recording.
          * \return true if success.  false on error
            * \remarks status of recording is updated in the receiverStatus member.
           * \remarks After recording, the unit should return to the default acquisition mode.
        */
        virtual bool StopRecord();

        /** \brief Starts the logging of distance data in a local log file.
          * \return true if success.  false on error
         */
        virtual bool BeginDistanceLog();

        /** \brief Ends the logging of distance data in a local log file.
          * \return true if success.  false on error
         */
        virtual bool EndDistanceLog();


        /** \brief Issues the command to set the current algorithm in the FPGA.
          *\param[in] algorigthmID  ID of the selected algorithm.
        * \return true if success.  false on error.
        */

        virtual bool SetAlgorithm(uint16_t algorithmID) = 0;

        /** \brief Sets an internal FPGA register to the value sent as argument.
          *\param[in] registerAddress Adrress of the register to change.
          *\param[in] registerValue Value to put into register.
        * \return true if success.  false on error.
        */

        virtual bool SetFPGARegister(uint16_t registerAddress, uint32_t registerValue) = 0;

        /** \brief Sets an ADC register to the value sent as argument.
          *\param[in] registerAddress Adrress of the register to change.
          *\param[in] registerValue Value to put into register.
        * \return true if success.  false on error.
        */
        virtual bool SetADCRegister(uint16_t registerAddress, uint32_t registerValue) = 0;

        /** \brief Sets an internal GPIO register to the value sent as argument.
          *\param[in] registerAddress Adrress of the register to change.
          *\param[in] registerValue Value to put into register (values accepted are 0-1).
        * \return true if success.  false on error.
        */

        virtual bool SetGPIORegister(uint16_t registerAddress, uint32_t registerValue) = 0;

        /** \brief Sets algorithm parameters to the value sent as argument.
          *\param[in] algoID ID of the detection algo affected by the change.
          *\param[in] registerAddress Adrress of the parameter to change.
          *\param[in] registerValue Value to put into register (values accepted are 0-1).
        * \return true if success.  false on error.
        */

        virtual bool SetAlgoParameter(int algoID, uint16_t registerAddress, uint32_t registerValue) = 0;

        /** \brief Sets global  algorithm parameters to the value sent as argument.
          *\param[in] registerAddress Adrress of the parameter to change.
          *\param[in] registerValue Value to put into register (values accepted are 0-1).
        * \return true if success.  false on error.
        */

        virtual bool SetGlobalAlgoParameter(uint16_t registerAddress, uint32_t registerValue) = 0;

        /** \brief Changes the controls of which messages are sent from AWL to the client to reflect the current global setting.
        * \return true if success.  false on error.
        * \remarks uses settings variables to call  SetMessageFilters(frameRate, channelMask, messageMask)
        */

        virtual bool SetMessageFilters();

        /** \brief Changes the controls of which messages are sent from AWL to the client to reflect provided settings
        * \param[in] frameRate new frame rate for the system. A value of 0 means no change
        * \param[in] channelMask mask for the analyzed channels.
        * \param[in] messageMask mask identifies which groups of target/distance/intensity messages are transmitted over CAN.
        * \return true if success.  false on error.
        */
        virtual bool SetMessageFilters(uint8_t frameRate, ChannelMask channelMask, MessageMask messageMask) = 0;


        /** \  an asynchronous query command to get the current algorithm.
        * \return true if success.  false on error.
        */
        virtual bool QueryAlgorithm() = 0;

        /** \brief Send an asynchronous query command for an internal FPGA register.
             *\param[in] registerAddress Adrress of the register to query.
          * \return true if success.  false on error.
          * \remarks On reception of the answer to query the register address and value will be
          *          placed in the FPGA registerSet.
            */
        virtual bool QueryFPGARegister(uint16_t registerAddress) = 0;

        /** \brief Send an asynchronous query command for an ADC register.
             *\param[in] registerAddress Adrress of the register to query.
          * \return true if success.  false on error.
          * \remarks On reception of the answer to query the register address and value will be
          *          placed in the ADC registerSet.
            */
        virtual bool QueryADCRegister(uint16_t registerAddress) = 0;

        /** \brief Send an asynchronous query command for a GPIO register.
             *\param[in] registerAddress Adrress of the register to query.
          * \return true if success.  false on error.
          * \remarks On reception of the answer to query the register address and value will be
          *          placed in the GPIO registerSet.
            */
        virtual bool QueryGPIORegister(uint16_t registerAddress) = 0;

        /** \brief Send an asynchronous query command for an algorithm parameter.
              *\param[in] algoID ID of the detection algo for which we want to query.
             *\param[in] registerAddress Adrress of the register to query.
          * \return true if success.  false on error.
          * \remarks On reception of the answer to query the register address and value will be
          *          placed in the algorithmParameters registerSet.
            */
        virtual bool QueryAlgoParameter(int algoID, uint16_t registerAddress) = 0;

        /** \brief Send an asynchronous query command for a global algorithm parameter.
          *\param[in] algoID ID of the detection algo for which we want to query.
         *\param[in] registerAddress Adrress of the register to query.
      * \return true if success.  false on error.
      * \remarks On reception of the answer to query the register address and value will be
      *          placed in the algorithmParameters registerSet.
        */
        virtual bool QueryGlobalAlgoParameter(uint16_t registerAddress) = 0;

    public:

        /** \brief Unique receiver ID. Corresponds to the index of receiver in receiverArray
        */
        int receiverID;

        /** \brief String indentifying the unique receiver type.
        */
        std::string sReceiverType;

        /** \brief Number of receiver channels on the sensor
          */
        int receiverChannelQty;

        /** \brief Current frame ID being built.
            \remark Note that the frameID corresponds to the "incomplete" frame currently being assembled.
                    For the last complete frame, use acquisitionSequence->GetLastFrameID();
          */
        std::atomic_uint32_t frameID;

        /** \brief Structure holding the frame data accumulation */
        AcquisitionSequence::Ptr acquisitionSequence;

        /** \brief Receiver status information
          */
        ReceiverStatus receiverStatus;

        /** \brief FPGA Registers description
         *  \remarks The FPGA register set should be initialized with a representation of the FPGA registers of the receiver model used.
         *           Usually, these are extracted from a config file.  If the register set is not initialized, some of the communication
         *           functions related to FPGA parameters will not transmit or interpret FPGA specific messages from the Receiver.
         *           However, they should not fail and can be called safely.
          */
        RegisterSet registersFPGA;

        /** \brief ADC Registers description
         *  \remarks The ADC register set should be initialized with a representation of the ADC registers of the receiver model used.
         *           Usually, these are extracted from a config file.  If the register set is not initialized, some of the communication
         *           functions related to ADC parameters will not transmit or interpret ADC specific messages from the Receiver.
         *           However, they should not fail and can be called safely.
          */
        RegisterSet registersADC;

        /** \brief GPIO Registers description
         *  \remarks The GPIO register set should be initialized with a representation of the GPIO registers of the receiver model used.
         *           Usually, these are extracted from a config file.  If the register set is not initialized, some of the communication
         *           functions related to GPIO parameters will not transmit or interpret GPIO specific messages from the Receiver.
         *           However, they should not fail and can be called safely.
          */
        RegisterSet registersGPIO;

        /** \brief Algorithm parameters  description
         *  \remarks Algorithms index start at 1. Algorithm 0 (GLOBAL_PARAMETERS_INDEX) is global parameters.
         *  \remarks The Algorithm set should be initialized with a representation of the algorithms parameters of the receiver model used.
         *           Usually, these are extracted from a config file.  If the algorithm set is not initialized, some of the communication
         *           functions related to algorithm parameters will not transmit or interpret algo messages from the Receiver.
         *           However, they should not fail and can be called safely.
         */
        AlgorithmSet parametersAlgos;


        /** \brief Reference distance used when logging data.
        */
        float targetHintDistance;

        /** \brief Reference angle used when logging data.
        */
        float targetHintAngle;

    protected:
        /** \brief Return the lidar data rendering thread status
          * \return true if the lidar data rendering thread is stoppped.
          */
        virtual void DoThreadLoop();


        /** \brief Mark the  current frame as invalid after a parse error of the CAN contents.
                   This will prevent the frame from being stored and processed.
          */
        void InvalidateFrame();

        /** \brief Tells if the frame is valid or not
          */
        bool isFrameValid();

        /** \brief Once all distances have been acquired in the current frame,
          *        push that frame into the frame buffer.
          *         Make sure the frameBuffer does not exceed the maximum number of frames
          * 		by removing the oldest frames, if necessary.
          *         Then, create a new "current frame" for processing.
          *         This should be canned only once, when all frame messages are received.
          *         Currently, it is invoked on reception of message 36 (las distance from last channel)
          */
        virtual void ProcessCompletedFrame();

        /** \brief Timestamp all the Tracks of the sourceFrame with the timeStamp of the sourceFrame      */
        void TimestampTracks(SensorFrame::Ptr sourceFrame);

        /** \brief Timestamp all the detections of the sourceFrame with the timeStamp of the sourceFrameFrame      */
        void TimestampDetections(SensorFrame::Ptr sourceFrame);

        /** \brief Write all the tracks in the sourceFrameframe to the log file
           * \param[in] logFile		file to which the log is injected
          * \param[in] sourceFrame  frame that contains the tracks to log
          */
        virtual void LogTracks(ofstream& logFile, SensorFrame::Ptr sourceFrame);

        /** \brief Write all the distances in the sourceFrame to the log file
           * \param[in] logFile		file to which the log is injected
          * \param[in] sourceFrame  frame that contains the tracks to log
          */
        virtual void LogDistances(ofstream& logFile, SensorFrame::Ptr sourceFrame);

        /** \brief Do one iteration of the thread loop.
          */
        virtual void DoOneThreadIteration();

        /** \brief Initialize status variables.
          */
        virtual void InitStatus();

        /** \brief Return the index of the RegisterSetting for the object that
                   has the address specified.
        * \param[in] inRegisterSet the registerSet that we want to search into
        * \param[in] inAddress the register address that we are searching for.
        * \return "index" of the found object in the list (this is NOT the sIndex field). -1 if no registers match that address.

          */
        int FindRegisterByAddress(const RegisterSet& inRegisterSet, uint16_t inAddress);


        /** \brief Returns pointer to the Algorithm parameter for the parameter that
                   has the address specified
          * \param[in] algoID an algorithm for which we want the parameter description.
         * \param[in] inAddress the parameter address
        * \return pointer to the found parameter in the list. NULL if no parameters match that address.

          */
        AlgorithmParameter* FindAlgoParamByAddress(int inAlgoID, uint16_t inAddress);

        /** \brief Reads the configuration proerties from the configuration file
          * \param[in] propTree the boost propertyTree created from reading the configuration file.
          * \returns Returns true otherwise.
          * \throws  Throws boost error on read of the property keys.
          */
        virtual bool ReadConfigFromPropTree(boost::property_tree::ptree& propTree);

        /** \brief Reads the description of registers (FPGA, ADC and GPIO) and controllable algorithm parameters
          *        from the configuration file.
          * \param[in] propTree the boost propertyTree created from reading the configuration file.
          * \returns Returns false in case of a read of the property tree, when all the register description is absent or not found.  Returns true otherwise.
          * \throws  Throws boost error on read of the property keys orther than the root key.
          */
        virtual bool ReadRegistersFromPropTree(boost::property_tree::ptree& propTree) = 0;

    protected:

        /** \brief Marker indicating that current frame as invalid after a parse error of the CAN contents.
                   This will prevent the frame from being stored and processed.
                   Reset after call of ProcessCompletedFrame();
         */
        std::atomic_bool bFrameInvalidated;

        /** \brief Time at the start of thread
         * \remark Initialized on object creation and evertytime the Go() method is called.
        */
        chrono::time_point startTime;
        chrono::high_resolution_clock highResolutionClock;

        /** \brief  measurement offset from sensor, introduced by detection algorithm */
        double measurementOffset;

        /** \brief  Pointer to the current frame information during frame acquisition */
        SensorFrame::Ptr currentFrame;

        /** \brief  debug file. */
        ofstream debugFile;

        /** \brief  Log file. */
        ofstream logFile;

        // generated in refactor, needs cleanup
        void logTrack();

        void logDistances();

        void addFrameToSequence() const;

        void controlSequenceSize() const;
    };


} // namespace AWL

#endif
