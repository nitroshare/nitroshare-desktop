set(SRC
    filesystemplugin.h
    filesystemplugin.cpp
)

add_library(filesystem MODULE ${SRC})

set_target_properties(filesystem PROPERTIES
    CXX_STANDARD 11
    VERSION   ${VERSION}
    SOVERSION ${VERSION_MAJOR}
)

target_link_libraries(filesystem nitroshare)

install(TARGETS filesystem
    DESTINATION plugins
)
