configure_file(about.json.in "${CMAKE_CURRENT_BINARY_DIR}/about.json")

set(SRC
    aboutdialog.h
    aboutdialog.cpp
    aboutplugin.h
    aboutplugin.cpp
    resource.qrc
    showaboutdialogaction.h
    showaboutdialogaction.cpp
)

add_library(about MODULE ${SRC})

set_target_properties(about PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(about PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(about nitroshare Qt5::Widgets)

install(TARGETS about
    LIBRARY DESTINATION "${INSTALL_PLUGIN_PATH}"
)
