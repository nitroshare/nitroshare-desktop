set(SRC
    apihandler.h
    apihandler.cpp
    apiplugin.h
    apiplugin.cpp
    apiserver.h
    apiserver.cpp
)

add_library(api MODULE ${SRC})

set_target_properties(api PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_link_libraries(api nitroshare QHttpEngine)

install(TARGETS api
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
