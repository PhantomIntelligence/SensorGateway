macro(setup_conan_dependencies_and_link_to_target _project_name)
    message(STATUS)
    message(STATUS "Downloading conan dependencies")
    message(STATUS)

    execute_process(
            COMMAND "./dev-script/conanUpdateDependencies" ${SENSOR_GATEWAY_CMAKE_MACRO_SOURCE_DIR}/sensor-gateway ${SENSOR_GATEWAY_CMAKE_MACRO_BINARY_DIR}
            OUTPUT_VARIABLE CONAN_SCRIPT_OUTPUT
            ERROR_VARIABLE CONAN_SCRIPT_OUTPUT
            WORKING_DIRECTORY ${SENSOR_GATEWAY_CMAKE_MACRO_SOURCE_DIR}
    )
    message(${CONAN_SCRIPT_OUTPUT})
    if (EXISTS ${SENSOR_GATEWAY_CMAKE_MACRO_BINARY_DIR}/conanbuildinfo.cmake)
        # Include generated cmake files
        include(${SENSOR_GATEWAY_CMAKE_MACRO_BINARY_DIR}/conanbuildinfo.cmake)

        conan_basic_setup(TARGETS)

        message(STATUS "Linking the following libs from conan: ${CONAN_LIBS}")
        conan_target_link_libraries(${_project_name})
        message(STATUS)
        message(STATUS "Done with conan")
    else ()
        message(FATAL_ERROR "Could not run conan. \n Make sure 'conan' is installed")
    endif ()
endmacro()
