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


#ifndef SPIRITSENSORGATEWAY_EXCEPTIONMESSAGES_H
#define SPIRITSENSORGATEWAY_EXCEPTIONMESSAGES_H

#include <iostream>
#include "TypeDefinition.h"

namespace ExceptionMessage {

    static auto ABSTRACT_POINTER_ARRAY_ILLEGAL_CONSUMPTION_EMPTY = "Illegal consumption of pointer: the ConstantSizedAbstractPointerArray is empty.";
    static auto ABSTRACT_POINTER_ARRAY_ILLEGAL_STORE_FULL = "Illegal store of pointer: the ConstantSizedAbstractPointerArray is full.";

    static auto CONSTANT_SIZED_POINTER_LIST_ILLEGAL_REMOVAL_POINTER_NOT_CONTAINED = "Illegal removal of pointer: the ConstantSizedPointerList is does not contain the pointer.";

    static auto RING_BUFFER_CONSUMPTION_ON_WRITER_MESSAGE = "Illegal consumption, execution should not reach this point. The calling entity should not be allowed to proceed to this call";

    static auto WORK_SCHEDULER_ILLEGAL_LINKING_SCHEDULER_HAS_BEEN_STOPPED = "Illegal linking attempt of WorkScheduler and InputBuffer: The WorkScheduler has received the terminate order, no new InputBuffer can be linked.";
    static auto WORK_SCHEDULER_ILLEGAL_LINKING_OF_ALREADY_LINKED_BUFFER_MESSAGE = "Illegal linking attempt of WorkScheduler and InputBuffer: The InputBuffer has already been linked.";
    static auto WORK_SCHEDULER_ILLEGAL_NUMBER_OF_INPUT_BUFFER_MESSAGE = "Illegal linking attempt of WorkScheduler and InputBuffer: The maximum number of InputBuffer has already been reached for this WorkScheduler.";
    static auto WORK_SCHEDULER_ILLEGAL_ACTIVATION_MESSAGE = "Illegal WorkScheduler activation attempt from an unlinked InputBuffer";
    static auto WORK_SCHEDULER_ILLEGAL_DEACTIVATION_MESSAGE = "Illegal WorkScheduler deactivation attempt from an unlinked InputBuffer";
}



#endif //SPIRITSENSORGATEWAY_EXCEPTIONMESSAGES_H
