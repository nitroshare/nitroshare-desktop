configure_file(device.json.in "${CMAKE_CURRENT_BINARY_DIR}/device.json")

set(SRC
    deviceplugin.h
    deviceplugin.cpp
    devicesaction.h
    devicesaction.cpp
)

add_library(device MODULE ${SRC})

set_target_properties(device PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(device PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(device nitroshare Qt5::Core)

install(TARGETS device
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
