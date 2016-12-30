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
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_link_libraries(lan nitroshare Qt5::Network)

install(TARGETS lan
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
