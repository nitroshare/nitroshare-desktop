configure_file(notifications.json.in "${CMAKE_CURRENT_BINARY_DIR}/notifications.json")

set(SRC
    notifier.h
    notifier.cpp
    notificationsplugin.h
    notificationsplugin.cpp
)

add_library(notifications MODULE ${SRC})

set_target_properties(notifications PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(notifications PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(notifications nitroshare Qt5::Widgets)

install(TARGETS notifications
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
