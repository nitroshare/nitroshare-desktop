configure_file(autostart.json.in "${CMAKE_CURRENT_BINARY_DIR}/autostart.json")

set(SRC
    autostarthelper.h
    autostarthelper.cpp
    autostartplugin.h
    autostartplugin.cpp
)

add_library(autostart MODULE ${SRC})

set_target_properties(autostart PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(autostart PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(autostart nitroshare Qt5::Core)

install(TARGETS autostart
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
