# Add Google tests files
# Note that googlemock target already builds googletest

set(GOOGLE_TEST_DIR "./lib/googletest" CACHE PATH "Path to the Google test framework.")
add_subdirectory(${GOOGLE_TEST_DIR} ${CMAKE_BINARY_DIR}/lib/googletest)
set_property(TARGET gtest APPEND_STRING PROPERTY COMPILE_FLAGS " -w")

include_directories(SYSTEM ${GOOGLE_TEST_DIR}/googlemock/include)

# Add SpiritSensorGateway files
# Select all the test and source files
file(GLOB TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test-suite/*")

# Include Mocks
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/mock")

# Include unfiled of higher hierarchy files
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/test-suite")

# Add all the test and mock files to the executable
add_executable(runtests ${TEST_FILES} test-suite/KvaserCanProtocolStrategyTest.cpp test-suite/MessageInterpretationStrategyTest.cpp)

find_package(SpiritSensorGateway)
target_link_libraries(runtests gtest gmock_main ${SpiritSensorGateway_LIBRARIES} ${CONAN_LIBS})

install(TARGETS runtests
        # IMPORTANT: Add the runtests executable to the "export-set"
        EXPORT SecureStreamingTargets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)

add_test(runtests runtests)
