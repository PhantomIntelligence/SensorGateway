
set(RUNTEST test-${PROJECT_NAME})

# Select all the test and source files
file(GLOB TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test-suite/*")
file(GLOB UTILITIES_FILES "${CMAKE_CURRENT_SOURCE_DIR}/utilities/files/*")
file(GLOB MOCK_FILES "${CMAKE_CURRENT_SOURCE_DIR}/utilities/mock/*")
file(GLOB STUB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/utilities/stub/*")
file(GLOB DATA_MODEL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/utilities/data-model/*")
file(GLOB APPLICATION_TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test-suite/application/*")
file(GLOB COMMON_TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test-suite/common/*")
file(GLOB DOMAIN_TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test-suite/domain/*")
file(GLOB TRANSLATOR_TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test-suite/message-translation/*")
file(GLOB SENSOR_COMM_TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test-suite/sensor-communication/*")
file(GLOB SERVER_COMM_TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/test-suite/server-communication/*")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/test-suite")

# Add all the test and mock files to the executable
find_package(SensorGateway)
add_executable(${RUNTEST} ${TEST_FILES} ${UTILITIES_FILES} ${MOCK_FILES} ${STUB_FILES} ${DATA_MODEL_FILES} ${APPLICATION_TEST_FILES} ${COMMON_TEST_FILES} ${DOMAIN_TEST_FILES} ${SENSOR_COMM_TEST_FILES} ${TRANSLATOR_TEST_FILES} ${SERVER_COMM_TEST_FILES})

target_link_libraries(${RUNTEST} gtest gmock_main ${SensorGateway_LIBRARIES} ${CONAN_LIBS})

fetch_googletest(
        ${PROJECT_SOURCE_DIR}/cmake-module
        ${PROJECT_BINARY_DIR}/lib/googletest
)

install(TARGETS ${RUNTEST}
        # IMPORTANT: Add the runtests executable to the "export-set"
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)

# Include acceptance-test
add_subdirectory(acceptance-test)

# Includes the manual test
add_subdirectory(manual-test)

add_test(${RUNTEST} ${RUNTEST})
