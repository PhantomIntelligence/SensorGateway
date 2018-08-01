macro(setup_conan_dependencies_and_link_to_target _project_name)
    message(STATUS)
    message(STATUS "Downloading conan dependencies")
    execute_process(
            COMMAND "./dev-script/conanUpdateDependencies"
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

    if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
        # Include generated cmake files
        include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)

        conan_basic_setup(TARGETS)

        message(STATUS "Linking the following libs from conan: ${CONAN_LIBS}")
        conan_target_link_libraries(${_project_name})
        message(STATUS "Done with conan")
        message(STATUS)
    else ()
        message(FATAL_ERROR "Could not run Conan. \n Make sure 'conan' is installed")
    endif()
endmacro()
