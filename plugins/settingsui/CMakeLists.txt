configure_file(settingsui.json.in "${CMAKE_CURRENT_BINARY_DIR}/settingsui.json")

set(SRC
    settingsaction.h
    settingsaction.cpp
    settingsuiplugin.h
    settingsuiplugin.cpp
)

add_library(settingsui MODULE ${SRC})

set_target_properties(settingsui PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(settingsui PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(settingsui nitroshare Qt5::Widgets)

install(TARGETS settingsui
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
