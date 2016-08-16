configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")

file(GLOB HEADERS include/nitroshare/*.h)
set(HEADERS "${HEADERS}" "${CMAKE_CURRENT_BINARY_DIR}/config.h")

set(SRC
    src/discovery/device.cpp
    src/discovery/deviceenumerator.cpp
    src/discovery/devicemodel.cpp
)

add_library(nitroshare SHARED ${HEADERS} ${SRC})

set_target_properties(nitroshare PROPERTIES
    CXX_STANDARD  11
    DEFINE_SYMBOL QT_NO_SIGNALS_SLOTS_KEYWORDS
    DEFINE_SYMBOL NITROSHARE_LIBRARY
    PUBLIC_HEADER "${HEADERS}"
    VERSION       ${VERSION}
    SOVERSION     ${VERSION_MAJOR}
)

target_include_directories(nitroshare PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(nitroshare Qt5::Network)

install(TARGETS nitroshare
    EXPORT nitroshare-export
    RUNTIME       DESTINATION bin
    LIBRARY       DESTINATION lib
    PUBLIC_HEADER DESTINATION include/nitroshare
)

install(EXPORT nitroshare-export
    DESTINATION lib/cmake/nitroshare
)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
