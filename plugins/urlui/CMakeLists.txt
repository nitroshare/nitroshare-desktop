configure_file(urlui.json.in "${CMAKE_CURRENT_BINARY_DIR}/urlui.json")

set(SRC
    urluiplugin.h
    urluiplugin.cpp
)

add_library(urlui MODULE ${SRC})

set_target_properties(urlui PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(urlui PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(urlui nitroshare Qt5::Widgets)

install(TARGETS urlui
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
