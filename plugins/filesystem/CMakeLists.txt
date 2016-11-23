set(SRC
    file.h
    file.cpp
    filehandler.h
    filehandler.cpp
    filesystemplugin.h
    filesystemplugin.cpp
    sendaction.h
    sendaction.cpp
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
