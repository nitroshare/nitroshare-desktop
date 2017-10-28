configure_file(splash.json.in "${CMAKE_CURRENT_BINARY_DIR}/splash.json")

set(SRC
    splashdialog.h
    splashdialog.cpp
    splashplugin.h
    splashplugin.cpp
)

add_library(splash MODULE ${SRC})

set_target_properties(splash PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(splash PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(splash nitroshare Qt5::Widgets)

install(TARGETS splash
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
