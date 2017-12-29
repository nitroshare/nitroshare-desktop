configure_file(dev.json.in "${CMAKE_CURRENT_BINARY_DIR}/dev.json")

set(SRC
    actionswidget.h
    actionswidget.cpp
    devplugin.h
    devplugin.cpp
    devwindow.h
    devwindow.cpp
    showdevwindowaction.h
    showdevwindowaction.cpp
)

add_library(dev MODULE ${SRC})

set_target_properties(dev PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(dev PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(dev nitroshare Qt5::Widgets)

install(TARGETS dev
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
