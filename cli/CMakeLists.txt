# Determine the relative path from the bin directory to the plugins
file(RELATIVE_PATH PLUGIN_PATH
    "/${INSTALL_BIN_PATH}"
    "/${INSTALL_PLUGIN_PATH}"
)

configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")

set(SRC
    main.cpp
    signal.cpp
)

add_executable(nitroshare-cli ${SRC})

set_target_properties(nitroshare-cli PROPERTIES
    CXX_STANDARD 11
)

target_include_directories(nitroshare-cli PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(nitroshare-cli nitroshare Qt5::Core)

install(TARGETS nitroshare-cli
    DESTINATION "${INSTALL_BIN_PATH}"
)
