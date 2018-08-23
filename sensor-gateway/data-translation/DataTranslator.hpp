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
#ifndef SENSORGATEWAY_DATATRANSLATOR_H
#define SENSORGATEWAY_DATATRANSLATOR_H

#include "sensor-gateway/sensor-communication/SensorCommunicator.hpp"
#include "DataTranslationStrategy.hpp"

namespace SensorAccessLinkElement {

    template<class I, class O>
    class DataTranslator : public DataFlow::DataSink<I> {

    protected:
        typedef I INPUT;
        typedef O OUTPUT;

    public:
        explicit DataTranslator(DataTranslation::DataTranslationStrategy<INPUT, OUTPUT>* dataTranslationStrategy) :
                dataTranslationStrategy(dataTranslationStrategy) {};

        ~DataTranslator() noexcept = default;

        DataTranslator(DataTranslator const& other) = delete;

        DataTranslator(DataTranslator&& other) noexcept = delete;

        DataTranslator& operator=(DataTranslator const& other)& = delete;

        DataTranslator& operator=(DataTranslator&& other)& noexcept = delete;

        void consume(I&& inputMessage) override {
            dataTranslationStrategy->translateMessage(std::forward<I>(inputMessage));
        };

    private:
        DataTranslation::DataTranslationStrategy<INPUT, OUTPUT>* dataTranslationStrategy;

    };
}

#endif //SENSORGATEWAY_DATATRANSLATOR_H
