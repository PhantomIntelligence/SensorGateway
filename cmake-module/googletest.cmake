## FROM https://github.com/google/googletest/blob/master/googletest/README.md#using-cmake

macro(fetch_googletest _download_module_path _download_root)
    message(STATUS "Building googletest")
    message(STATUS "GoogleTest download directory: ${_download_root}")
    set(GOOGLETEST_DOWNLOAD_ROOT ${_download_root})
    configure_file(
            ${_download_module_path}/googletest-download.cmake.in
            ${_download_root}/CMakeLists.txt
            @ONLY
    )
    unset(GOOGLETEST_DOWNLOAD_ROOT)

    execute_process(
            COMMAND
            "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
            WORKING_DIRECTORY
            ${_download_root}
    )
    execute_process(
            COMMAND
            "${CMAKE_COMMAND}" --build .
            WORKING_DIRECTORY
            ${_download_root}
    )

    # adds the targers: gtest, gtest_main, gmock, gmock_main
    add_subdirectory(
            ${_download_root}/googletest-src
            ${_download_root}/googletest-build
    )

    set_property(TARGET gtest APPEND_STRING PROPERTY COMPILE_FLAGS " -w")
    message(STATUS "Done with googletest")
endmacro()
