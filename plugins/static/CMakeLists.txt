configure_file(static.json.in "${CMAKE_CURRENT_BINARY_DIR}/static.json")

set(SRC
    staticplugin.h
    staticplugin.cpp
)

add_library(static MODULE ${SRC})

set_target_properties(static PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(static PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(static nitroshare Qt5::Network)

install(TARGETS static
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
