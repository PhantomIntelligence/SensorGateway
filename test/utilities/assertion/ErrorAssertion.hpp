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

#ifndef SENSORGATEWAY_ERRORASSERTION_HPP
#define SENSORGATEWAY_ERRORASSERTION_HPP

#include <gtest/gtest.h>
#include "test/utilities/mock/ArbitraryDataSinkMock.hpp"

namespace {
    using Error = ErrorHandling::SensorAccessLinkError;
    using ErrorSinkMock = Mock::ArbitraryDataSinkMock<Error>;
    using ErrorProcessingScheduler = DataFlow::DataProcessingScheduler<Error, ErrorSinkMock, 1>;

   Error formatStrategyErrorWithCorrectOrigin(Error const& strategyIssuedError, std::string const& origin) noexcept {
        Error formattedError(origin + ErrorHandling::Message::SEPARATOR + strategyIssuedError.getOrigin(),
                             strategyIssuedError.getCategory(),
                             strategyIssuedError.getSeverity(),
                             strategyIssuedError.getErrorCode(),
                             strategyIssuedError.getMessage());
        return formattedError;
    }

}

namespace Assert {
    static ::testing::AssertionResult errorHasBeenThrown(ErrorSinkMock* sink, Error const& expectedError) noexcept {

        auto producedErrors = sink->getConsumedData();
        bool expectedErrorPublished = true;
        auto lastParsedError = ErrorHandling::SensorAccessLinkError::returnDefaultData();
        for (auto t = 0; t < producedErrors.size() && expectedErrorPublished; ++t) {
            lastParsedError = producedErrors.front();
            expectedErrorPublished = lastParsedError == expectedError;
            producedErrors.pop_front();
        }
        if (expectedErrorPublished) {
            return ::testing::AssertionSuccess();
        } else {
            return ::testing::AssertionFailure()
                    << "\nExpected : \n"
                    << expectedError.fetchDetailedMessage()
                    << "\nGot : \n"
                    << lastParsedError.fetchDetailedMessage()
                    << "\n";
        }
    }
}

#endif //SENSORGATEWAY_ERRORASSERTION_HPP
