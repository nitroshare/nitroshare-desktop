# Determine the relative path from the bin directory to the plugins
file(RELATIVE_PATH PLUGIN_PATH
    "/${INSTALL_BIN_PATH}"
    "/${INSTALL_PLUGIN_PATH}"
)

configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")

set(SRC
    main.cpp
)

add_executable(nitroshare-daemon ${SRC})

set_target_properties(nitroshare-daemon PROPERTIES
    CXX_STANDARD 11
    OUTPUT_NAME nitroshare
)

target_include_directories(nitroshare-daemon PUBLIC
    "${CMAKE_CURRENT_BINARY_DIR}"
)

target_link_libraries(nitroshare-daemon nitroshare Qt5::Core)

install(TARGETS nitroshare-daemon
    DESTINATION "${INSTALL_BIN_PATH}"
)
