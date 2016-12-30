set(SRC
    broadcastenumerator.h
    broadcastenumerator.cpp
    broadcastplugin.h
    broadcastplugin.cpp
)

add_library(broadcast MODULE ${SRC})

set_target_properties(broadcast PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_link_libraries(broadcast nitroshare Qt5::Network)

install(TARGETS broadcast
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
