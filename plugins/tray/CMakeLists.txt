configure_file(trayplugin.json.in "${CMAKE_CURRENT_BINARY_DIR}/trayplugin.json")

set(SRC
    resource.qrc
    trayplugin.h
    trayplugin.cpp
)

add_library(tray MODULE ${SRC})

set_target_properties(tray PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(tray PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(tray nitroshare Qt5::Widgets)

install(TARGETS tray
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
