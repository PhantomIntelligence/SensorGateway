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


#ifndef SPIRITSENSORGATEWAY_HIGHINTEGRITYTHREAD_H
#define SPIRITSENSORGATEWAY_HIGHINTEGRITYTHREAD_H

#include <thread>
#include <cstdint>

/**
 * @brief High Integrity C++ default compliant code is contained in this namespace. It has been directly obtained from the HIC++ version 4
 */
namespace HighIntegrity {

    /**
     * @brief Enum used in this template to determine if the thread should run by itself and will be expected to rejoin a thread later (JOIN) or if it simply will be executed and no be expected back (DETACH)
     */
    enum ThreadExecutionType : int32_t {
        DETACH,
        JOIN,
    };

    /**
     * @brief Delete safe implementation of std::threads. Only this implementation shall be used in the project
     * @template ThreadExec the enum type (see enum ThreadExec)
     */
    template<ThreadExecutionType TYPE>
    class HighIntegrityThread {
    public:
        template<class F, class ...ARGS>
        HighIntegrityThread(F&& f, ARGS&& ...args)
                : thread(std::forward<F>(f), std::forward<ARGS>(args)...) {
        }

        HighIntegrityThread(HighIntegrityThread const&) = delete;

        HighIntegrityThread(HighIntegrityThread&&) = default;

        ~HighIntegrityThread() {
            exitSafely();
        }

        inline void exitSafely() {
            if (thread.joinable()) {
                joinOrDetach();
            }
        }

        HighIntegrityThread& operator=(const HighIntegrityThread&) = delete;

        HighIntegrityThread& operator=(HighIntegrityThread&& other) {
            if (this != &other) {
                exitSafely();
            }
            swap(other);
            return *this;
        }

        inline void join() { thread.join(); }

        void swap(HighIntegrityThread& other) noexcept { std::swap(thread, other.thread); }

    private:
        inline void joinOrDetach();

        std::thread thread;

    };

    /**
     * @brief DETACH specialisation for HIThreads
     */
    template<>
    inline void HighIntegrityThread<ThreadExecutionType::DETACH>::joinOrDetach() {
        thread.detach();
    }

    /**
     * @brief JOIN specialisation for HIThreads
     */
    template<>
    inline void HighIntegrityThread<ThreadExecutionType::JOIN>::joinOrDetach() {
        thread.join();
    }

}

#endif //SPIRITSENSORGATEWAY_HIGHINTEGRITYTHREAD_H
