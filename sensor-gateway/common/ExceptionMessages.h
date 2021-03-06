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

#ifndef SENSORGATEWAY_EXCEPTIONMESSAGES_H
#define SENSORGATEWAY_EXCEPTIONMESSAGES_H

namespace ExceptionMessage {

    static auto ABSTRACT_POINTER_ARRAY_ILLEGAL_CONSUMPTION_EMPTY = "Illegal consumption of pointer: the ConstantSizedAbstractPointerArray is empty.";
    static auto ABSTRACT_POINTER_ARRAY_ILLEGAL_STORE_FULL = "Illegal store of pointer: the ConstantSizedAbstractPointerArray is full.";

    static auto CONSTANT_SIZED_POINTER_LIST_ILLEGAL_REMOVAL_OF_POINTER = "Illegal removal of pointer: the ConstantSizedPointerList does not contain the pointer.";

    static auto RING_BUFFER_ILLEGAL_CONSUMPTION_ON_WRITER_LOCATION_MESSAGE = "Illegal consumption, execution should not reach this point. The calling entity should not be allowed to proceed to this call. Data might have been lost";

    static auto DATA_PROCESSING_SCHEDULER_ILLEGAL_LINKING_SCHEDULER_HAS_BEEN_STOPPED = "Illegal linking attempt between DataProcessingScheduler and DataSourceBuffer: The DataProcessingScheduler has received the terminate order, no new DataSourceBuffer can be linked.";
    static auto DATA_PROCESSING_SCHEDULER_ILLEGAL_LINKING_OF_ALREADY_LINKED_BUFFER_MESSAGE = "Illegal linking attempt between DataProcessingScheduler and DataSourceBuffer: The DataSourceBuffer has already been linked.";
    static auto DATA_PROCESSING_SCHEDULER_ILLEGAL_NUMBER_OF_INPUT_BUFFER_MESSAGE = "Illegal linking attempt between DataProcessingScheduler and DataSourceBuffer: The maximum number of DataSourceBuffer has already been reached for this DataProcessingScheduler.";
    static auto DATA_PROCESSING_SCHEDULER_ILLEGAL_ACTIVATION_MESSAGE = "Illegal DataProcessingScheduler activation attempt from an unlinked DataSourceBuffer";
    static auto DATA_PROCESSING_SCHEDULER_ILLEGAL_DEACTIVATION_MESSAGE = "Illegal DataProcessingScheduler deactivation attempt from an unlinked DataSourceBuffer";

    static auto PIXEL_TRACK_ARRAY_ILLEGAL_STORE_FULL = "Illegal storing of track: the pixel's track array is full";
    static auto SERVICE_TIMESTAMPS_ILLEGAL_FULL_CAPACITY = "Illegal storing of timestamp: the time points array is already full";

}

#endif //SENSORGATEWAY_EXCEPTIONMESSAGES_H
