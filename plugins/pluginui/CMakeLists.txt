configure_file(pluginui.json.in "${CMAKE_CURRENT_BINARY_DIR}/pluginui.json")

set(SRC
    managepluginsaction.h
    managepluginsaction.cpp
    plugindialog.h
    plugindialog.cpp
    pluginproxymodel.h
    pluginproxymodel.cpp
    pluginuiplugin.h
    pluginuiplugin.cpp
)

add_library(pluginui MODULE ${SRC})

set_target_properties(pluginui PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(pluginui PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(pluginui Qt5::Widgets nitroshare)

install(TARGETS pluginui
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
