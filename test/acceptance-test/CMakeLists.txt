# Add SensorGateway acceptance test files
file(GLOB TRANSLATION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/message-translation/*")

add_executable(acceptance-tests ${TRANSLATION_FILES} ${UTILITIES_FILES} ${MOCK_FILES} ${STUB_FILES} )

find_package(SensorGateway)
target_link_libraries(acceptance-tests gtest gmock_main ${SensorGateway_LIBRARIES} ${CONAN_LIBS})

install(TARGETS acceptance-tests
        # IMPORTANT: Add the runtests executable to the "export-set"
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)
