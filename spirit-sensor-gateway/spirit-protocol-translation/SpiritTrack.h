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
#ifndef SPIRITSENSORGATEWAY_SPIRITTRACK_H
#define SPIRITSENSORGATEWAY_SPIRITTRACK_H

#include <cstdint>

class SpiritTrack {

    public:

        explicit SpiritTrack(uint16_t id,uint8_t confidenceLevel, uint16_t intensity);

        explicit SpiritTrack(uint16_t id,uint16_t distance, uint16_t acceleration);

        void setDistance(uint16_t distance);

        void setSpeed(uint16_t speed);

        void setAcceleration(uint16_t acceleration);



    private:
        uint16_t id;

        uint16_t distance;

        uint16_t speed;

        uint16_t acceleration;

        uint16_t intensity;

        uint8_t confidenceLevel;



};


#endif //SPIRITSENSORGATEWAY_SPIRITTRACK_H
