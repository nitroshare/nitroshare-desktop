configure_file(mdns.json.in "${CMAKE_CURRENT_BINARY_DIR}/mdns.json")

set(SRC
    mdnsplugin.h
    mdnsplugin.cpp
)

add_library(mdns MODULE ${SRC})

set_target_properties(mdns PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(mdns PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(mdns nitroshare Qt5::Network)

install(TARGETS mdns
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
