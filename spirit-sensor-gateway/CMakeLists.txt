configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

############  SpiritSensorGateway files   ###############

# Select all the source and header files
file(GLOB COMMON_FILES "${CMAKE_CURRENT_SOURCE_DIR}/common/*")
file(GLOB DOMAIN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/domain/*")
file(GLOB COMMUNICATION_PROTOCOL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/sensor-communication/*")

add_library(${PROJECT_NAME} SHARED ${COMMON_FILES} ${DOMAIN_FILES} ${COMMUNICATION_PROTOCOL_FILES} config.h.in sensor-communication/SensorCommunicator.h)

########## CONAN ######################################
if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup(TARGETS)

    message(STATUS "Linking the following libs from conan: ${CONAN_LIBS}")
    conan_target_link_libraries(${PROJECT_NAME})
else()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
endif()
#######################################################

target_link_libraries(${PROJECT_NAME} canlib)

set_target_properties(${PROJECT_NAME} PROPERTIES
                      PUBLIC_HEADER "${HEADER_FILES};${CMAKE_CURRENT_BINARY_DIR}/config.h")

install(TARGETS ${PROJECT_NAME}
        # IMPORTANT: Add the library to the "export-set"
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}" COMPONENT dev)
