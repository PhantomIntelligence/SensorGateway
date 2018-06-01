
# Add SpiritSensorGateway files
# Select all the test and source files
file(GLOB LIVE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*")

# Add all the test and mock files to the executable
add_executable(${LIVE_NAME} ${LIVE_FILES})

find_package(SpiritSensorGateway)
target_link_libraries(${LIVE_NAME} ${SpiritSensorGateway_LIBRARIES} ${CONAN_LIBS})

install(TARGETS ${LIVE_NAME}
        # IMPORTANT: Add the RunTests executable to the "export-set"
        EXPORT SpiritSensorGatewayTestTargets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin)

add_test(runtests runtests)
