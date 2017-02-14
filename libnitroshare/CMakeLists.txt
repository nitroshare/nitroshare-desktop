configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/nitroshare/config.h")

file(GLOB HEADERS include/nitroshare/*.h)
set(HEADERS "${HEADERS}" "${CMAKE_CURRENT_BINARY_DIR}/nitroshare/config.h")

set(SRC
    src/action/action.cpp
    src/action/actionregistry.cpp
    src/application/application.cpp
    src/bundle/bundle.cpp
    src/core/object.cpp
    src/discovery/device.cpp
    src/discovery/deviceenumerator.cpp
    src/discovery/devicemodel.cpp
    src/handler/handlerregistry.cpp
    src/plugin/plugin.cpp
    src/plugin/pluginregistry.cpp
    src/transfer/packet.cpp
    src/transfer/transfer.cpp
    src/transfer/transfermodel.cpp
    src/util/logger.cpp
    src/util/settings.cpp
    src/util/signalnotifier.cpp
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
    RUNTIME       DESTINATION "${INSTALL_BIN_PATH}"
    LIBRARY       DESTINATION "${INSTALL_LIB_PATH}"
    PUBLIC_HEADER DESTINATION include/nitroshare
)

install(EXPORT nitroshare-export
    DESTINATION lib/cmake/nitroshare
)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
