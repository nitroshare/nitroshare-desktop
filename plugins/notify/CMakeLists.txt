configure_file(notify.json.in "${CMAKE_CURRENT_BINARY_DIR}/notify.json")

set(SRC
    notifyplugin.h
    notifyplugin.cpp
    showlibnotifynotificationaction.h
    showlibnotifynotificationaction.cpp
)

# Use "notify-plugin" to avoid a name clash with libnotify

add_library(notify-plugin MODULE ${SRC})

set_target_properties(notify-plugin PROPERTIES
    CXX_STANDARD             11
    OUTPUT_NAME              notify
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(notify-plugin PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}"
    "${libnotify_INCLUDE_DIRS}"
)
target_link_libraries(notify-plugin nitroshare "${libnotify_LIBRARIES}")

install(TARGETS notify-plugin
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
