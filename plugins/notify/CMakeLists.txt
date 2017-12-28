configure_file(notify.json.in "${CMAKE_CURRENT_BINARY_DIR}/notify.json")

set(SRC
    notifyplugin.h
    notifyplugin.cpp
)

add_library(notify MODULE ${SRC})

set_target_properties(notify PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(notify PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(notify nitroshare Qt5::Widgets)

install(TARGETS notify
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
