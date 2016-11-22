set(SRC
    indicatorplugin.h
    indicatorplugin.cpp
)

add_library(indicator MODULE ${SRC})

set_target_properties(indicator PROPERTIES
    CXX_STANDARD 11
    VERSION   ${VERSION}
    SOVERSION ${VERSION_MAJOR}
)

target_link_libraries(indicator nitroshare appindicator)

install(TARGETS indicator
    DESTINATION plugins
)
