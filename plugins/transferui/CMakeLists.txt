configure_file(transferui.json.in "${CMAKE_CURRENT_BINARY_DIR}/transferui.json")

set(SRC
    transferproxymodel.h
    transferproxymodel.cpp
    transfersaction.h
    transfersaction.cpp
    transferuiplugin.h
    transferuiplugin.cpp
    transferwindow.h
    transferwindow.cpp
)

add_library(transferui MODULE ${SRC})

set_target_properties(transferui PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(transferui PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(transferui Qt5::Widgets nitroshare)

install(TARGETS transferui
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)
