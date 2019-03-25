/**
	Copyright 2014-2019 Phantom Intelligence Inc.

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

#ifndef SENSORGATEWAY_MOCKFUNCTIONMOCK_HPP
#define SENSORGATEWAY_MOCKFUNCTIONMOCK_HPP

#include <cassert>
#include <type_traits>
#include "test/utilities/data-model/DataModelFixture.h"

namespace Mock {

    struct VoidType {
        using type = void;
    };

    /**
     * Allows to more easily define the behavior of a mock function member (definition for only one parameter)
     * @tparam N StringLiteral that names the function
     * @tparam R Return type of the function
     * @tparam P Parameter type of the function
     */
    template<typename N, class R, typename P>
    class Function final {
    protected:

        using Name = N;
        using ReturnType = R;
        using ParameterType = P;
        using ParametersReceivedWhenInvoked = std::list<ParameterType>;


        // TODO : Improve this by changing `InvokeExpectation = std::pair<ParameterType, ReturnType>` to `FunctionMapping = std::tuple<...>` with more possible options (e.g. errorToThrow, callbackToInvoke, etc...)
        // TODO : Once ^ is done, add stuff like: `template<typename Mapping> using getReturnValue = std::get<ReturnType>(Mapping);`... or something like that :)
        using InvokeExpectation = std::tuple<ParameterType, ReturnType>;
        using InvokeExpectations = std::list<InvokeExpectation>;

        static constexpr bool const FUNCTION_REQUIRES_NO_PARAMETERS = std::is_same<ParameterType, VoidType>::value;
        static constexpr bool const FUNCTION_REQUIRES_PARAMETERS = !FUNCTION_REQUIRES_NO_PARAMETERS;
        static constexpr bool const FUNCTION_HAS_A_VOID_RETURN_TYPE = std::is_same<ReturnType, VoidType>::value;
        static constexpr bool const FUNCTION_HAS_A_NON_VOID_RETURN_TYPE = !FUNCTION_HAS_A_VOID_RETURN_TYPE;

    public:

        explicit Function() :
                mapping(false),
                functionInvoked(false) {
        };

        ~Function() noexcept = default;

        auto invoke() -> ReturnType {
            assertFunctionRequiresNoParameters();
            static_assert(!std::is_same<ReturnType, VoidType>::value,
                          "MockFunction --> this function has a non void return type. `invokeVoidReturn` should be used instead.");
            VoidType voidValue;
            ReturnType returnValue = useMappingIfSet(voidValue, &invokeExpectations);
            invokeWithParameters(std::move(voidValue));
            return returnValue;
        }

        auto invoke(ParameterType&& parameter) -> ReturnType {
            assertFunctionRequiresParameters();
            static_assert(!std::is_same<ReturnType, VoidType>::value,
                          "MockFunction --> this function has a non void return type. `invokeVoidReturn` should be used instead.");
            ReturnType returnValue = useMappingIfSet(parameter, &invokeExpectations);
            invokeWithParameters(std::move(parameter));
            return returnValue;
        }

        void invokeVoidReturn() {
            assertFunctionRequiresNoParameters();
            static_assert(std::is_same<ReturnType, VoidType>::value,
                          "MockFunction --> this function has a void return type. `invoke` should be used instead.");
            VoidType voidValue;
            invokeWithParameters(std::move(voidValue));
        }

        void invokeVoidReturn(ParameterType&& parameter) {
            assertFunctionRequiresParameters();
            static_assert(std::is_same<ReturnType, VoidType>::value,
                          "MockFunction --> this function has a void return type. `invoke` should be used instead.");
            invokeWithParameters(std::forward<ParameterType>(parameter));
        }

        /**
         * Start to create a call expectation
         */
        void expectCall() {
            assertFunctionRequiresNoParameters();
            assertFunctionHasAVoidReturnTypeForExpectation();
            VoidType voidValue;
            startInvocationExpectation(voidValue);
            completeInvocationExpectation(voidValue);
        }

        /**
         * Start to create a call expectation
         */
        void expectCall(ParameterType&& parameterValue) {
            assertFunctionRequiresParameters();
            assertFunctionHasAVoidReturnTypeForExpectation();
            startInvocationExpectation(parameterValue);
            VoidType voidValue;
            completeInvocationExpectation(voidValue);
        }

        /**
         * Start to create a call expectation, based on the fact that no parameter is received
         */
        void onCall() {
            assertFunctionRequiresNoParameters();
            assertFunctionHasANonVoidReturnTypeForExpectation();
            VoidType voidValue;
            startInvocationExpectation(voidValue);
        }

        /**
         * Start to create a call expectation, based on the reception of a parameter
         * @param parameterValue the expected parameter
         */
        void onCall(ParameterType&& parameterValue) {
            assertFunctionRequiresParameters();
            assertFunctionHasANonVoidReturnTypeForExpectation();
            startInvocationExpectation(parameterValue);
        }

        void returnThis(ReturnType const& returnValue) {
            static_assert(!std::is_same<ReturnType, VoidType>::value,
                          "MockFunction --> this function has a void return type. This function cannot be called.");
            completeInvocationExpectation(returnValue);
        }

        // TODO : Add timeout and handle cases where fail to facilitate testing and avoid infinite test
        // Allow test thread to sleep until call done
        void waitUntilInvocation() {
            if (!hasBeenInvoked()) {
                invocationAcknowledgement.get_future().wait();
            }
        }

        bool hasBeenInvoked() const noexcept {
            return functionInvoked.load();
        }

        bool hasBeenInvokedWith(ParameterType const& parameterValue) const noexcept {
            LockGuard guard(parameterInvocationMutex);
            auto end = std::cend(parametersReceivedWhenInvoked);
            auto result = std::find(std::cbegin(parametersReceivedWhenInvoked), end, parameterValue);
            return result != end;
        }

    private:

        static constexpr void assertFunctionRequiresNoParameters() noexcept {
            static_assert(FUNCTION_REQUIRES_NO_PARAMETERS,
                          "MockFunction --> this function expects an argument. Try to add a parameter to this call.");
        }

        static constexpr void assertFunctionRequiresParameters() noexcept {
            static_assert(FUNCTION_REQUIRES_PARAMETERS,
                          "MockFunction --> this function does not expect any argument. Try to remove any parameters from this call");
        }

        static constexpr void assertFunctionHasANonVoidReturnTypeForInvocation() noexcept {
            static_assert(FUNCTION_HAS_A_NON_VOID_RETURN_TYPE,
                          "MockFunction --> this function has a void return type. `invokeVoidReturn` should be used instead.");
        }

        static constexpr void assertFunctionHasAVoidReturnTypeForInvocation() noexcept {
            static_assert(FUNCTION_HAS_A_VOID_RETURN_TYPE,
                          "MockFunction --> this function has a non void return type. `invoke` should be used instead.");
        }

        static constexpr void assertFunctionHasANonVoidReturnTypeForExpectation() noexcept {
            static_assert(FUNCTION_HAS_A_NON_VOID_RETURN_TYPE,
                          "MockFunction --> this function has a void return type. `expectCall` should be used instead.");
        }

        static constexpr void assertFunctionHasAVoidReturnTypeForExpectation() noexcept {
            static_assert(FUNCTION_HAS_A_VOID_RETURN_TYPE,
                          "MockFunction --> this function has a non void return type. `onCall` should be used instead.");
        }

        // Customization of mock function behavior
        struct ExpectationMapComparator {
            ParameterType const parameterValue;

            explicit ExpectationMapComparator(ParameterType const& parameterValue) : parameterValue(parameterValue) {
            }

            bool const operator()(InvokeExpectation const& invokeExpectation) const {
                return getParameterValue(&invokeExpectation) == parameterValue;
            }
        };

        ReturnType
        useMappingIfSet(ParameterType const& parameterValue, InvokeExpectations const* mapToUse) const {
            ReturnType returnValue;
            auto iterator = std::find_if(mapToUse->begin(), mapToUse->end(), ExpectationMapComparator(parameterValue));
            if (iterator != mapToUse->end()) {
                InvokeExpectation expectation = *iterator;
                returnValue = getReturnValue(&expectation);
            }
            return returnValue;
        }


        void startInvocationExpectation(ParameterType const& parameterValue) {
            assert(!mapping.load());
            mapping.store(true);
            temporaryParameterValueMapping = parameterValue;
        }

        void completeInvocationExpectation(ReturnType const& returnValue) {
            assert(mapping.load());
            mapping.store(false);
            invokeExpectations.remove_if(
                    [&](InvokeExpectation expectation) {
                        return getParameterValue(&expectation) == temporaryParameterValueMapping;
                    }
            );
            invokeExpectations.emplace_back(temporaryParameterValueMapping, returnValue);
        }

        AtomicFlag mapping;
        InvokeExpectations invokeExpectations;
        ParameterType temporaryParameterValueMapping;

        template<typename E>
        static constexpr auto getParameterValue(E* invokeExpectation) noexcept {
            return std::get<ParameterType>(*invokeExpectation);
        }

        template<typename E>
        static constexpr auto getReturnValue(E* invokeExpectation) noexcept {
            return std::get<ReturnType>(*invokeExpectation);
        }

        // Runtime check
        void invokeWithParameters(ParameterType&& parameter) {
            LockGuard guard(parameterInvocationMutex);
            parametersReceivedWhenInvoked.push_back(parameter);
            acknowledgeInvocation(); // TODO :add an acknowledgement for a specific parameter
        }

        // Confirmation of expected usage
        void acknowledgeInvocation() {
            LockGuard guard(acknowledgementMutex);
            if (!hasBeenInvoked()) {
                functionInvoked.store(true);
                invocationAcknowledgement.set_value(true);
            }
        }

        AtomicFlag functionInvoked;

        Mutex acknowledgementMutex;
        mutable BooleanPromise invocationAcknowledgement;

        mutable Mutex parameterInvocationMutex;
        ParametersReceivedWhenInvoked parametersReceivedWhenInvoked;

    };
}

#endif //SENSORGATEWAY_MOCKFUNCTIONMOCK_HPP
