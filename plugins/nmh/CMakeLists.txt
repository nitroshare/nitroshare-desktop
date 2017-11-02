configure_file(nmh.json.in "${CMAKE_CURRENT_BINARY_DIR}/nmh.json")

set(SRC
    nmhplugin.h
    nmhplugin.cpp
)

add_library(nmh MODULE ${SRC})

set_target_properties(nmh PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(nmh PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(nmh nitroshare Qt5::Core)

install(TARGETS nmh
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
