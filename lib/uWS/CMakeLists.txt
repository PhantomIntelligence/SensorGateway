cmake_minimum_required(VERSION 2.8)

set(uWS_EXTRA_LIBS "")

option(ENABLE_LIBUV "Enable libUV backend"  ON)

if(ENABLE_LIBUV)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_LIBUV")
    find_package(LibUV REQUIRED)
    include_directories(${LibUV_INCLUDE_DIRS})
    set(uWS_EXTRA_LIBS ${uWS_EXTRA_LIBS} ${LIBUV_LIBRARIES})
endif(ENABLE_LIBUV)


if(APPLE)
    add_definitions(-stdlib=libc++ -mmacosx-version-min=10.7 -undefined dynamic_lookup)
endif(APPLE)

##  PTHREAD
find_package(Threads REQUIRED)

## OpenSSL
find_package(PkgConfig REQUIRED)
pkg_search_module(OPENSSL REQUIRED openssl)

if( OPENSSL_FOUND )
    include_directories(${OPENSSL_INCLUDE_DIR})
    message(STATUS "Using OpenSSL ${OPENSSL_VERSION}")
else()
    # Error; with REQUIRED, pkg_search_module() will throw an error by it's own
    message(FATAL "OpeSSL no found")
endif()

## ZLIB
find_package(ZLIB REQUIRED)

if( ZLIB_FOUND )
    include_directories(${ZLIB_INCLUDE_DIRS})
    message(STATUS "Using Zlib ${ZLIB_VERSION_STRING}")
else()
    # Error; with REQUIRED, pkg_search_module() will throw an error by it's own
    message(FATAL "ZLib no found")
endif()

## Source files
set(uWS_TARGET uWS)
file(GLOB uWS_LIB_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src")

file(GLOB uWS_HEADERS ${uWS_LIB_FILES}/*.h)
file(GLOB uWS_SRCS ${uWS_LIB_FILES}/*.cpp)

add_library(${uWS_TARGET} SHARED ${uWS_SRCS} ${uWS_HEADERS})

set_target_properties(${uWS_TARGET} PROPERTIES INTERFACE_POSITION_INDEPENDENT_CODE ON
        PUBLIC_HEADER "${uWS_HEADERS}")
include(GNUInstallDirs)
install(TARGETS ${uWS_TARGET}
        EXPORT ${uWS_TARGET}Targets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${uWS_TARGET} COMPONENT dev)

set(uWS_TARGET_LINKS "${uWS_TARGET} ${uWS_EXTRA_LIBS} ${OPENSSL_LIBRARIES} ${ZLIB_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS}")
