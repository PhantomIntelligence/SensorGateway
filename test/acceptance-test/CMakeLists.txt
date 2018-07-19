# Add SpiritSensorGateway acceptance test files
file(GLOB TRANSLTAION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/message-translation/*")
file(GLOB SERVER_COMM_FILES "${CMAKE_CURRENT_SOURCE_DIR}/server-communication/*")

add_executable(acceptance-tests ${TRANSLATION_FILES} ${SERVER_COMM_FILES} ${UTILITIES_FILES} ${STUB_FILES})

find_package(SpiritSensorGateway)
target_link_libraries(acceptance-tests gtest gmock_main ${SpiritSensorGateway_LIBRARIES} ${CONAN_LIBS} uws)

install(TARGETS acceptance-tests
        # IMPORTANT: Add the runtests executable to the "export-set"
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)
