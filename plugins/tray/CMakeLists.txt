set(SRC
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

target_link_libraries(tray nitroshare Qt5::Widgets)

install(TARGETS tray
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
