configure_file(wizard.json.in "${CMAKE_CURRENT_BINARY_DIR}/wizard.json")

set(SRC
    wizardplugin.h
    wizardplugin.cpp
)

add_library(wizard MODULE ${SRC})

set_target_properties(wizard PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(wizard PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(wizard nitroshare Qt5::Widgets)

install(TARGETS wizard
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
