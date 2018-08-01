configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

############  SpiritSensorGateway files   ###############

# Select all the source and header files
file(GLOB APPLICATION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/application/*")
file(GLOB COMMON_FILES "${CMAKE_CURRENT_SOURCE_DIR}/common/*")
file(GLOB DOMAIN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/domain/*")
file(GLOB SERVER_COMMUNICATION_PROTOCOL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/server-communication/*")
file(GLOB SPIRIT_PROTOCOL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/message-translation/*")
file(GLOB COMMUNICATION_PROTOCOL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/sensor-communication/*")

add_library(${PROJECT_NAME} SHARED ${APPLICATION_FILES} ${COMMON_FILES} ${DOMAIN_FILES} ${SRC_FILES} ${COMMUNICATION_PROTOCOL_FILES} ${SPIRIT_PROTOCOL_FILES} ${SERVER_COMMUNICATION_PROTOCOL_FILES} config.h.in)

setup_conan_dependencies_and_link_to_target(${PROJECT_NAME})

########## uws ########################################
find_package(uws CONFIG REQUIRED)
if (uws_FOUND)
    message(STATUS "Found uWS")
    message(STATUS "INCLUDE_DIRS ${uws_INCLUDE_DIRS}")
    include_directories(${uws_INCLUDE_DIRS})
else ()
    message(FATAL "uWS is required")
endif ()
#######################################################

target_link_libraries(${PROJECT_NAME} canlib uws)

set_target_properties(${PROJECT_NAME} PROPERTIES
        PUBLIC_HEADER "${HEADER_FILES};${CMAKE_CURRENT_BINARY_DIR}/config.h")

install(TARGETS ${PROJECT_NAME}
        # IMPORTANT: Add the library to the "export-set"
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}" COMPONENT dev)

