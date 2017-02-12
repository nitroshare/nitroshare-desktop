# Determine the relative path from the bin directory to the plugins
file(RELATIVE_PATH PLUGIN_PATH
    "/${INSTALL_BIN_PATH}"
    "/${INSTALL_PLUGIN_PATH}"
)

configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")

set(SRC
    src/init.cpp
)

add_library(nitroshare-common STATIC ${HEADERS} ${SRC})

set_target_properties(nitroshare-common PROPERTIES
    CXX_STANDARD  11
)

target_include_directories(nitroshare PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
)

target_link_libraries(nitroshare-common nitroshare Qt5::Core)
