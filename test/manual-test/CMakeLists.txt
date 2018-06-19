find_package(SpiritSensorGateway)

file(GLOB MANUAL_TEST_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*/")

foreach (file ${MANUAL_TEST_FILES})

    set(currentPath ${CMAKE_CURRENT_SOURCE_DIR}/${file})

    IF (IS_DIRECTORY ${currentPath})

        file(GLOB MANUAL_TEST_FILES ${currentPath}/*)

        set(MANUAL_TEST_NAME ${file})
        set(MANUAL_TEST manual-test-${MANUAL_TEST_NAME})
        message(STATUS "Building manual test ${MANUAL_TEST_NAME}")

        add_executable(${MANUAL_TEST} ${MANUAL_TEST_FILES})
        target_link_libraries(${MANUAL_TEST} ${SpiritSensorGateway_LIBRARIES} ${CONAN_LIBS})


        install(TARGETS ${MANUAL_TEST}
            EXPORT SpiritSensorGatewayTargets
                RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)
        message(STATUS "Done")

    ENDIF ()
endforeach ()
