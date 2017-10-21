configure_file(deviceui.json.in "${CMAKE_CURRENT_BINARY_DIR}/deviceui.json")

set(SRC
    browseaction.h
    browseaction.cpp
    devicedialog.h
    devicedialog.cpp
    deviceproxymodel.h
    deviceproxymodel.cpp
    deviceuiplugin.h
    deviceuiplugin.cpp
)

add_library(deviceui MODULE ${SRC})

set_target_properties(deviceui PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_link_libraries(deviceui Qt5::Widgets nitroshare)

install(TARGETS deviceui
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
