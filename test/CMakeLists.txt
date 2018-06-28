# Add Google tests files
# Note that googlemock target already builds googletest

set(GOOGLE_TEST_DIR "./lib/googletest" CACHE PATH "Path to the Google test framework.")
add_subdirectory(${GOOGLE_TEST_DIR} ${CMAKE_BINARY_DIR}/lib/googletest)
set_property(TARGET gtest APPEND_STRING PROPERTY COMPILE_FLAGS " -w")

include_directories(SYSTEM ${GOOGLE_TEST_DIR}/googlemock/include)

# Add SpiritSensorGateway files
# Select all the test and source files
file(GLOB TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test-suite/*")
file(GLOB TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/mock/*")
file(GLOB RING_BUFFER_TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test-suite/common/*")
file(GLOB DATA_MODEL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test-suite/data-model/*")

# Include Mocks
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/mock")

# Include unfiled of higher hierarchy files
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/test-suite")

# Include manual-test
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/test/manual-test")

# Add all the test and mock files to the executable
add_executable(runtests ${TEST_FILES} ${MOCK_FILES} ${RING_BUFFER_TEST_FILES} ${DATA_MODEL_FILES})



find_package(SpiritSensorGateway)
target_link_libraries(runtests gtest gmock_main ${SpiritSensorGateway_LIBRARIES} ${CONAN_LIBS} ${uWS_TARGET_LINKS})

install(TARGETS runtests
        # IMPORTANT: Add the runtests executable to the "export-set"
        EXPORT SecureStreamingTargets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)


# Includes the manual test
add_subdirectory(manual-test)

add_test(runtests runtests)
