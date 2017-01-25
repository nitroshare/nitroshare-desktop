set(SRC
    uiplugin.h
    uiplugin.cpp
)

add_library(ui MODULE ${SRC})

set_target_properties(ui PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_link_libraries(ui nitroshare Qt5::Widgets)

install(TARGETS ui
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
