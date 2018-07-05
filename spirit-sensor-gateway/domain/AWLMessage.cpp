#include "AWLMessage.h"

DataFlow::AWLMessage::AWLMessage(int64_t id, uint64_t timestamp, uint32_t length, AWL::DataArray data) :
        id(id), timestamp(timestamp), length(length), data(data) {
}

DataFlow::AWLMessage const DataFlow::AWLMessage::returnDefaultData() noexcept {
    return Defaults::DEFAULT_AWLMESSAGE;
}
