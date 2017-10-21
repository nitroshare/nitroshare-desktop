configure_file(mdns.json.in "${CMAKE_CURRENT_BINARY_DIR}/mdns.json")

set(SRC
    mdns.h
    mdns.cpp
    mdnsenumerator.h
    mdnsenumerator.cpp
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

target_link_libraries(mdns Qt5::Network nitroshare qmdnsengine)

install(TARGETS mdns
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)

# Copy the QMdnsEngine library to the runtime and install directories
if(WIN32)
    include(SharedLibrary)
    copy_lib_win(qmdnsengine mdns)
endif()
