/**
	Copyright 2014-2018 Phantom Intelligence Inc.
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


#ifndef SPIRITSENSORGATEWAY_AWLMESSAGETRANSLATORTEST_CPP
#define SPIRITSENSORGATEWAY_AWLMESSAGETRANSLATORTEST_CPP


#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "spirit-sensor-gateway/protocol-translation/AWLMessageToSpiritMessageTranslationStrategy.h"

using DataFlow::Track;
using ProtocolTranslation::AWLMessageToSpiritMessageTranslationStrategy;


class AWLMessageToSpiritMessageTranslationStrategyTest : public ::testing::Test {
protected:
    AWLMessage createAWLMessageWithID(uint16_t id) const;

    Track createTrack(TrackID trackID, ConfidenceLevel confidenceLevel, Intensity intensity, Acceleration acceleration,
                      Distance distance, Speed speed);

    Pixel createPixel(PixelID pixelID, std::vector<Track> tracks);

    Frame createFrame(FrameID frameID, SystemID systemID, std::vector<Pixel> pixels);
};

using FrameSink = DataFlow::DataSink<Frame>;

class FrameSinkMock : public FrameSink {

protected:

    using FrameSink::DATA;

public:

    FrameSinkMock(uint8_t numberOfDataToConsumeGoal) :
            actualNumberOfDataConsumed(0),
            numberOfDataToConsumeGoal(numberOfDataToConsumeGoal) {

    }

    void consume(DATA&& data) override {
        ++actualNumberOfDataConsumed;
        consumedData.push_back(data);
    }

    bool hasBeenCalledExpectedNumberOfTimes() const {
        return actualNumberOfDataConsumed.load() == numberOfDataToConsumeGoal;
    };
    

    std::vector<Frame> getConsumedData() const noexcept {
        return consumedData;
    }

private:

    AtomicCounter actualNumberOfDataConsumed;
    AtomicCounter numberOfDataToConsumeGoal;

    std::vector<Frame> consumedData;
};

using FrameProcessingScheduler = DataFlow::DataProcessingScheduler<Frame, FrameSinkMock, 1>;

TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,
       given_someFrameDoneAWLMessage_when_translatingTheAwlMessage_then_setsTheCorrectFrameIdAndSystemId) {

    FrameSinkMock frameSinkMock(1);
    FrameProcessingScheduler scheduler(&frameSinkMock);
    AWLMessageToSpiritMessageTranslationStrategy awlMessageTranslator;
    awlMessageTranslator.linkConsumer(&scheduler);

    AWLMessage awlMessage = AWLMessage::returnDefaultData();

    awlMessage.id = FRAME_DONE;
    awlMessage.data[0] = 18;
    awlMessage.data[1] = 16;
    awlMessage.data[2] = 0;
    awlMessage.data[3] = 16;

    awlMessageTranslator.translateBasicMessage(std::move(awlMessage));
    auto expectedFrameID = 4114;
    auto expectedSystemID = 4096;

    scheduler.terminateAndJoin();

    auto actualFrame =  frameSinkMock.getConsumedData().at(0);
    ASSERT_EQ(expectedFrameID, actualFrame.getFrameID());
    ASSERT_EQ(expectedSystemID, actualFrame.getSystemID());

}

TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,
       given_someDetectionTrackAWLMessage_when_translatingTheAwlMessage_then_setsTheCorrectAttributesOfTheTrack) {

    auto detectionTrackAwlMessage = createAWLMessageWithID(DETECTION_TRACK);
    auto endOfFrameAwlMessage = createAWLMessageWithID(FRAME_DONE);
    auto expectedPixelID = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[3],
                                                              detectionTrackAwlMessage.data[4]);
    auto expectedTrackID = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[0],
                                                              detectionTrackAwlMessage.data[1]);
    auto expectedTrackConfidenceLevel = detectionTrackAwlMessage.data[5];
    auto expectedTrackIntensity = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[6],
            detectionTrackAwlMessage.data[7]);


    auto expectedProducedFrame = createFrame(8208, 304, {createPixel(expectedPixelID, {createTrack(expectedTrackID,
                                                                                                   expectedTrackConfidenceLevel,
                                                                                                   expectedTrackIntensity,
                                                                                                   Defaults::Track::DEFAULT_ACCELERATION_VALUE,
                                                                                                   Defaults::Track::DEFAULT_DISTANCE_VALUE,
                                                                                                   Defaults::Track::DEFAULT_SPEED_VALUE)})});

    FrameSinkMock frameSinkMock(1);
    FrameProcessingScheduler scheduler(&frameSinkMock);
    AWLMessageToSpiritMessageTranslationStrategy awlMessageTranslator;
    awlMessageTranslator.linkConsumer(&scheduler);

    awlMessageTranslator.translateBasicMessage(std::move(detectionTrackAwlMessage));
    awlMessageTranslator.translateBasicMessage(std::move(endOfFrameAwlMessage));

    scheduler.terminateAndJoin();


    awlMessageTranslator.translateBasicMessage(std::move(detectionTrackAwlMessage));
    awlMessageTranslator.translateBasicMessage(std::move(endOfFrameAwlMessage));

    auto actualProducedFrame = frameSinkMock.getConsumedData().at(0);
    auto pixel = actualProducedFrame.fetchPixelByID(expectedPixelID);
    auto track = *(pixel->fetchTrackByID(expectedTrackID));


    ASSERT_EQ(expectedTrackID, track.getID());
    ASSERT_EQ(expectedTrackConfidenceLevel, track.getConfidenceLevel());
    ASSERT_EQ(expectedTrackIntensity, track.getIntensity());
    ASSERT_EQ(Defaults::Track::DEFAULT_ACCELERATION_VALUE, track.getAcceleration());
    ASSERT_EQ(Defaults::Track::DEFAULT_DISTANCE_VALUE, track.getDistance());
    ASSERT_EQ(Defaults::Track::DEFAULT_SPEED_VALUE, track.getSpeed());
}

TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,
       given_someVelocityTrackAWLMessage_when_translatingTheAWLMessage_then_setsTheCorrectAttributesOfTheTrack) {

    auto detectionTrackAwlMessage = createAWLMessageWithID(DETECTION_TRACK);
    auto detectionVelocityAwlMessage = createAWLMessageWithID(DETECTION_VELOCITY);
    auto endOfFrameAwlMessage = createAWLMessageWithID(FRAME_DONE);
    auto expectedPixelID = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[3],
                                                              detectionTrackAwlMessage.data[4]);
    auto expectedTrackID = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[0],
                                                              detectionTrackAwlMessage.data[1]);
    auto expectedTrackConfidenceLevel = detectionTrackAwlMessage.data[5];
    auto expectedTrackIntensity = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[6],
                                                                     detectionTrackAwlMessage.data[7]);
    auto expectedTrackDistance = convertTwoBytesToUnsignedBigEndian(detectionVelocityAwlMessage.data[2],
                                                                    detectionVelocityAwlMessage.data[3]);
    Speed expectedTrackSpeed = convertTwoBytesToSignedBigEndian(detectionTrackAwlMessage.data[4],
                                                                detectionTrackAwlMessage.data[5]);
    Acceleration expectedTrackAcceleration = convertTwoBytesToSignedBigEndian(detectionTrackAwlMessage.data[6],
                                                                              detectionTrackAwlMessage.data[7]);

    auto expectedProducedFrame = createFrame(8208, 304, {createPixel(expectedPixelID, {createTrack(expectedTrackID,
                                                                                                   expectedTrackConfidenceLevel,
                                                                                                   expectedTrackIntensity,
                                                                                                   expectedTrackAcceleration,
                                                                                                   expectedTrackDistance,
                                                                                                   expectedTrackSpeed)})});

    FrameSinkMock frameSinkMock(1);
    FrameProcessingScheduler scheduler(&frameSinkMock);
    AWLMessageToSpiritMessageTranslationStrategy awlMessageTranslator;
    awlMessageTranslator.linkConsumer(&scheduler);

    awlMessageTranslator.translateBasicMessage(std::move(detectionTrackAwlMessage));
    awlMessageTranslator.translateBasicMessage(std::move(detectionVelocityAwlMessage));
    awlMessageTranslator.translateBasicMessage(std::move(endOfFrameAwlMessage));

    scheduler.terminateAndJoin();

    auto actualProducedFrame = frameSinkMock.getConsumedData().at(0);
    auto pixel = actualProducedFrame.fetchPixelByID(expectedPixelID);
    auto track = *(pixel->fetchTrackByID(expectedTrackID));

    ASSERT_EQ(expectedTrackID, track.getID());
    ASSERT_EQ(expectedTrackConfidenceLevel, track.getConfidenceLevel());
    ASSERT_EQ(expectedTrackIntensity, track.getIntensity());
    ASSERT_EQ(expectedTrackDistance, track.getDistance());
    ASSERT_EQ(expectedTrackSpeed, track.getSpeed());
    ASSERT_EQ(expectedTrackAcceleration, track.getAcceleration());
}


TEST_F(AWLMessageToSpiritMessageTranslationStrategyTest,
       given_someFrameDoneAwlMessage_when_translatingTheAwlMessage_then_callsTheProduceMethodOfDataSource) {

    auto detectionTrackAwlMessage = createAWLMessageWithID(DETECTION_TRACK);
    auto detectionVelocityAwlMessage = createAWLMessageWithID(DETECTION_VELOCITY);
    auto endOfFrameAwlMessage = createAWLMessageWithID(FRAME_DONE);
    auto expectedPixelID = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[3],
                                                              detectionTrackAwlMessage.data[4]);
    auto expectedTrackID = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[0],
                                                              detectionTrackAwlMessage.data[1]);
    auto expectedTrackConfidenceLevel = detectionTrackAwlMessage.data[5];
    auto expectedTrackIntensity = convertTwoBytesToUnsignedBigEndian(detectionTrackAwlMessage.data[6],
                                                                     detectionTrackAwlMessage.data[7]);
    auto expectedTrackDistance = convertTwoBytesToUnsignedBigEndian(detectionVelocityAwlMessage.data[2],
                                                                    detectionVelocityAwlMessage.data[3]);
    Speed expectedTrackSpeed = convertTwoBytesToSignedBigEndian(detectionTrackAwlMessage.data[4],
                                                                detectionTrackAwlMessage.data[5]);
    Acceleration expectedTrackAcceleration = convertTwoBytesToSignedBigEndian(detectionTrackAwlMessage.data[6],
                                                                              detectionTrackAwlMessage.data[7]);

    auto expectedProducedFrame = createFrame(8208, 304, {createPixel(expectedPixelID, {createTrack(expectedTrackID,
                                                                                           expectedTrackConfidenceLevel,
                                                                                           expectedTrackIntensity,
                                                                                           expectedTrackAcceleration,
                                                                                           expectedTrackDistance,
                                                                                           expectedTrackSpeed)})});

    FrameSinkMock frameSinkMock(1);
    FrameProcessingScheduler scheduler(&frameSinkMock);
    AWLMessageToSpiritMessageTranslationStrategy awlMessageTranslator;
    awlMessageTranslator.linkConsumer(&scheduler);

    awlMessageTranslator.translateBasicMessage(std::move(detectionTrackAwlMessage));
    awlMessageTranslator.translateBasicMessage(std::move(detectionVelocityAwlMessage));
    awlMessageTranslator.translateBasicMessage(std::move(endOfFrameAwlMessage));

    scheduler.terminateAndJoin();

    auto actualProducedFrame = frameSinkMock.getConsumedData().at(0);


    ASSERT_EQ(frameSinkMock.hasBeenCalledExpectedNumberOfTimes(),1);


}


AWLMessage AWLMessageToSpiritMessageTranslationStrategyTest::createAWLMessageWithID(uint16_t id) const {

    AWLMessage awlMessage = AWLMessage::returnDefaultData();
    awlMessage.id = id;
    awlMessage.data[0] = 16;
    awlMessage.data[1] = 32;
    awlMessage.data[2] = 48;
    awlMessage.data[3] = 1;
    awlMessage.data[4] = 0;
    awlMessage.data[5] = 96;
    awlMessage.data[6] = 112;
    awlMessage.data[7] = 128;
    return awlMessage;
}

Track AWLMessageToSpiritMessageTranslationStrategyTest::createTrack(TrackID trackID, ConfidenceLevel confidenceLevel,
                                                                    Intensity intensity, Acceleration acceleration,
                                                                    Distance distance, Speed speed) {
    Track track = Track(trackID, confidenceLevel, intensity);
    track.setAcceleration(acceleration);
    track.setDistance(distance);
    track.setSpeed(speed);
    return track;
}

Pixel AWLMessageToSpiritMessageTranslationStrategyTest::createPixel(PixelID pixelID, std::vector<Track> tracks) {
    Pixel pixel = Pixel(pixelID);
    for (auto track : tracks) {
        pixel.addTrack(track);
    }
    return pixel;
}

Frame AWLMessageToSpiritMessageTranslationStrategyTest::createFrame(FrameID frameID, SystemID systemID,
                                                                    std::vector<Pixel> pixels) {
    Frame frame = Frame();
    frame.setFrameID(frameID);
    frame.setSystemID(systemID);
    for (auto pixel : pixels) {
        frame.addPixel(pixel);
    }
    return frame;
}

#endif //SPIRITSENSORGATEWAY_AWLMESSAGETRANSLATORTEST_CPP
