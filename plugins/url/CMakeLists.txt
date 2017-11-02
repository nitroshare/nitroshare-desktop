configure_file(url.json.in "${CMAKE_CURRENT_BINARY_DIR}/url.json")

set(SRC
    sendurlaction.h
    sendurlaction.cpp
    url.h
    url.cpp
    urlhandler.h
    urlhandler.cpp
    urlplugin.h
    urlplugin.cpp
)

add_library(url MODULE ${SRC})

set_target_properties(url PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(url PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(url nitroshare Qt5::Widgets)

install(TARGETS url
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
