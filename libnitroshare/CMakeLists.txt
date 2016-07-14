# libnitroshare - core application functionality
# Copyright 2016 - Nathan Osman

configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")

set(SRC
    src/discovery/device.cpp
)

add_library(nitroshare SHARED ${SRC})

set_target_properties(nitroshare PROPERTIES
    CXX_STANDARD  11
    DEFINE_SYMBOL QT_NO_SIGNALS_SLOTS_KEYWORDS
    DEFINE_SYMBOL NITROSHARE_LIBRARY
    VERSION       ${VERSION}
    SOVERSION     ${VERSION_MAJOR}
)

target_include_directories(nitroshare PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}/include>"
)

target_link_libraries(nitroshare Qt5::Network)
