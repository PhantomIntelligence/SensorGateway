configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h" @ONLY)

############  SensorGateway files   ###############

# Select all the source and header files
file(GLOB APPLICATION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/application/*")
file(GLOB COMMON_FILES "${CMAKE_CURRENT_SOURCE_DIR}/common/*")
file(GLOB DATA_STRUCTURE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/common/data-structure/*")
file(GLOB SENSOR_STRUCTURE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/common/data-structure/sensor/*")
file(GLOB SPIRIT_STRUCTURE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/common/data-structure/spirit/*")
file(GLOB DOMAIN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/domain/*")
file(GLOB SERVER_COMMUNICATION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/server-communication/*")
file(GLOB TRANSLATION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/data-translation/*")
file(GLOB SENSOR_COMMUNICATION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/sensor-communication/*")

add_library(${PROJECT_NAME} SHARED ${APPLICATION_FILES} ${COMMON_FILES} ${DATA_STRUCTURE_FILES} ${SPIRIT_STRUCTURE_FILES} ${SENSOR_STRUCTURE_FILES} ${DOMAIN_FILES} ${SRC_FILES} ${SERVER_COMMUNICATION_FILES} ${TRANSLATION_FILES} ${SENSOR_COMMUNICATION_FILES} config.h.in)

setup_conan_dependencies_and_link_to_target(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME} canlib)

set_target_properties(${PROJECT_NAME} PROPERTIES
        PUBLIC_HEADER "${HEADER_FILES};${CMAKE_CURRENT_BINARY_DIR}/config.h")

install(TARGETS ${PROJECT_NAME}
        # IMPORTANT: Add the library to the "export-set"
        EXPORT ${PROJECT_NAME}Targets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
        LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT shlib
        PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/${PROJECT_NAME}" COMPONENT dev)

