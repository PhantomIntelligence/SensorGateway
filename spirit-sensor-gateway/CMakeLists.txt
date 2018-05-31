configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

############  Perdition files   ###############

# Select all the source and header files
file(GLOB COMMON_FILES "${CMAKE_CURRENT_SOURCE_DIR}/common/*")
file(GLOB BACKEND_FILES "${CMAKE_CURRENT_SOURCE_DIR}/backend/*")
file(GLOB FRONTEND_FILES "${CMAKE_CURRENT_SOURCE_DIR}/frontend/*")

add_library(${PROJECT_NAME} SHARED ${COMMON_FILES} ${BACKEND_FILES} ${FRONTEND_FILES} config.h.in)


########### SDL #######################################
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})
include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
#######################################################

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES} ${SDL2_IMAGE_LIBRARIES})

########## CONAN ######################################
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)

    message(STATUS "Linking: ${CONAN_LIBS}")
    conan_target_link_libraries(${PROJECT_NAME})
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()
#######################################################

## LIVE_SOURCE_DIR is the live test directory, name set in main CMakeLists.txt
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR}/${LIVE_SOURCE_DIR})

set_target_properties(${PROJECT_NAME} PROPERTIES
                      PUBLIC_HEADER "${HEADER_FILES};${CMAKE_CURRENT_BINARY_DIR}/config.h")

install(TARGETS ${PROJECT_NAME}
        # IMPORTANT: Add the library to the "export-set"
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}" COMPONENT dev)
