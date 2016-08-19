set(SRC
    apiplugin.h
    apiplugin.cpp
)

add_library(api MODULE ${SRC})

set_target_properties(api PROPERTIES
    CXX_STANDARD 11
    VERSION   ${VERSION}
    SOVERSION ${VERSION_MAJOR}
)

target_link_libraries(api nitroshare QHttpEngine)

install(TARGETS api
    DESTINATION plugins
)
