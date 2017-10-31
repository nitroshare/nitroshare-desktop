configure_file(sharebox.json.in "${CMAKE_CURRENT_BINARY_DIR}/sharebox.json")

set(SRC
    shareboxplugin.h
    shareboxplugin.cpp
)

add_library(sharebox MODULE ${SRC})

set_target_properties(sharebox PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(sharebox PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(sharebox nitroshare Qt5::Widgets)

install(TARGETS sharebox
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
