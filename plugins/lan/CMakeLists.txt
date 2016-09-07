set(SRC
    lanplugin.h
    lanplugin.cpp
    lantransport.h
    lantransport.cpp
    lantransportserver.h
    lantransportserver.cpp
)

add_library(lan MODULE ${SRC})

set_target_properties(lan PROPERTIES
    CXX_STANDARD 11
    VERSION      ${VERSION}
    SOVERSION    ${VERSION_MAJOR}
)

target_link_libraries(lan nitroshare Qt5::Network)

install(TARGETS lan
    DESTINATION plugins
)
