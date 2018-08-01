macro(setup_conan_dependencies_and_link_to_target _project_name)
    message(STATUS)
    message(STATUS "Downloading conan dependencies")
    execute_process(
            COMMAND ls -lAh
            WORKING_DIRECTORY ${CMAKE_MACRO_SOURCE_DIR})
    execute_process(
            COMMAND ls -lAh
            WORKING_DIRECTORY ${CMAKE_MACRO_BINARY_DIR})

    execute_process(
            COMMAND "./dev-script/conanUpdateDependencies" spirit-sensor-gateway ${CMAKE_MACRO_BINARY_DIR}
            OUTPUT_VARIABLE CONAN_SCRIPT_OUTPUT
            ERROR_VARIABLE CONAN_SCRIPT_OUTPUT
            WORKING_DIRECTORY ${CMAKE_MACRO_SOURCE_DIR}
    )
    message(${CONAN_SCRIPT_OUTPUT})
    if (EXISTS ${CMAKE_MACRO_BINARY_DIR}/conanbuildinfo.cmake)
        # Include generated cmake files
        include(${CMAKE_MACRO_BINARY_DIR}/conanbuildinfo.cmake)

        conan_basic_setup(TARGETS)

        message(STATUS "Linking the following libs from conan: ${CONAN_LIBS}")
        conan_target_link_libraries(${_project_name})
        message(STATUS "Done with conan")
        message(STATUS)
    else ()
        message(FATAL_ERROR "Could not run conan. \n Make sure 'conan' is installed")
    endif ()
endmacro()
