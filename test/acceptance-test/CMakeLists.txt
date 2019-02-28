
set(ACCEPTANCE_TEST acceptance-test-${PROJECT_NAME})

file(GLOB TRANSLATION_ACCEPT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/data-translation/*")
file(GLOB REQUEST_HANDLING_ACCEPT_FILES "${CMAKE_CURRENT_SOURCE_DIR}/request-handling/*")

add_executable(${ACCEPTANCE_TEST}
        ${ASSERTION_FILES}
        ${DATA_MODEL_FILES}
        ${TRANSLATION_ACCEPT_FILES}
        ${REQUEST_HANDLING_ACCEPT_FILES}
        ${UTILITIES_FILES}
        ${MOCK_FILES}
        ${STUB_FILES}
        )

find_package(SensorGateway)
target_link_libraries(${ACCEPTANCE_TEST} gtest gmock_main ${SensorGateway_LIBRARIES} ${CONAN_LIBS})

set_property(TARGET ${ACCEPTANCE_TEST} PROPERTY CXX_STANDARD 14)

install(TARGETS ${ACCEPTANCE_TEST}
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)
