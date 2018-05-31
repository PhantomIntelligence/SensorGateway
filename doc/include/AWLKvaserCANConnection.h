/**
	Copyright 2014-2016 Phantom Intelligence Inc.

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

#ifndef SPECTRE_AWLKVASERCANCONNECTION_H
#define SPECTRE_AWLKVASERCANCONNECTION_H

#include <canlib.h> // Kvaser CAN library
#include "SensorConnection.hpp"


namespace SensorCommunication {

    using SensorCommunication::AWL::CAN::Message;
    using std::string;

    class AWLKvaserCANConnection final : public SensorConnection<Message> {

        using KvaserCanStatus = canStatus;
        using KvaserConnectionHandle = int;
        using KvaserCanChannelId = int;
        using KvaserCanBitRate = long;


        const KvaserConnectionHandle INVALID_CONNECTION_HANDLE;
        const Millisecond DELAY_BEFORE_RECONNECTION_ATTEMPT;

    public:

        AWLKvaserCANConnection();

        ~AWLKvaserCANConnection();

        AWLKvaserCANConnection(AWLKvaserCANConnection const& other) = delete;

        AWLKvaserCANConnection(AWLKvaserCANConnection&& other) noexcept = delete;

        AWLKvaserCANConnection& operator=(AWLKvaserCANConnection other)& = delete;

        AWLKvaserCANConnection& operator=(AWLKvaserCANConnection&& other)& noexcept = delete;

        void openConnection() override;

        void closeConnection() override;

        void transmitMessage(Message const& message) override;

        auto receiveMessage() -> Message;

    private:
        Boolean isConnected() const noexcept;

        Boolean isConnectionValid() const noexcept;

        void logActionIfAnErrorOccurred(string attemptedAction, KvaserCanStatus kvaserCanStatus) const noexcept;

        void throwingExitIfTheActionFailed(string attemptedAction, KvaserCanStatus kvaserCanStatus);

        Boolean anErrorOccurred(KvaserCanStatus status) const noexcept;

        void updateReconnectionTimePointToNowPlusDelay() noexcept;

        void attemptReconnection();

        void attemptReconnectionAndThrowIfAnErrorOccurred(const KvaserCanStatus& status);

        Mutex connectionMutex;
        AtomicFlag connected;
        KvaserConnectionHandle kvaserConnectionHandle;
        KvaserCanChannelId kvaserCanChannelId;
        KvaserCanBitRate kvaserCanBitRate;

        TimePoint timePointToReachBeforeReconnectionAttempt;

        HighResolutionClock highResolutionClock;


    };
}

#endif //SPECTRE_AWLKVASERCANCONNECTION_H
