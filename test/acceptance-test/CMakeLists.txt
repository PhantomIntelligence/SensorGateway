# Add SpiritSensorGateway acceptance test files
file(GLOB TEST_FILES "${CMAKE_CURRENT_SOURCE_DIR}/spirit-protocol-translation/*")

add_executable(acceptance-tests ${TEST_FILES} ${UTILITIES_FILES})

find_package(SpiritSensorGateway)
target_link_libraries(acceptance-tests gtest gmock_main ${SpiritSensorGateway_LIBRARIES} ${CONAN_LIBS} uws)

install(TARGETS acceptance-tests
        # IMPORTANT: Add the runtests executable to the "export-set"
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)
