configure_file(filesystem.json.in "${CMAKE_CURRENT_BINARY_DIR}/filesystem.json")

set(SRC
    file.h
    file.cpp
    filehandler.h
    filehandler.cpp
    filesystemplugin.h
    filesystemplugin.cpp
    sendfilesaction.h
    sendfilesaction.cpp
)

add_library(filesystem MODULE ${SRC})

set_target_properties(filesystem PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(filesystem PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(filesystem nitroshare)

install(TARGETS filesystem
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
