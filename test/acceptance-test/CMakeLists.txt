
set(ACCEPTANCE_TEST acceptance-test-${PROJECT_NAME})

file(GLOB TRANSLATION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/data-translation/*")

add_executable(${ACCEPTANCE_TEST}
        ${TRANSLATION_FILES}
        ${UTILITIES_FILES}
        ${MOCK_FILES}
        ${STUB_FILES}
        )

find_package(SensorGateway)
target_link_libraries(${ACCEPTANCE_TEST} gtest gmock_main ${SensorGateway_LIBRARIES} ${CONAN_LIBS})

install(TARGETS ${ACCEPTANCE_TEST}
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)
