set(HEADERS
    include/qmdnsengine/abstractserver.h
    include/qmdnsengine/bitmap.h
    include/qmdnsengine/browser.h
    include/qmdnsengine/cache.h
    include/qmdnsengine/dns.h
    include/qmdnsengine/hostname.h
    include/qmdnsengine/mdns.h
    include/qmdnsengine/message.h
    include/qmdnsengine/prober.h
    include/qmdnsengine/provider.h
    include/qmdnsengine/qmdnsengine_export.h
    include/qmdnsengine/query.h
    include/qmdnsengine/record.h
    include/qmdnsengine/resolver.h
    include/qmdnsengine/server.h
    include/qmdnsengine/service.h
)

set(SRC
    src/abstractserver.cpp
    src/bitmap.cpp
    src/browser.cpp
    src/cache.cpp
    src/dns.cpp
    src/hostname.cpp
    src/mdns.cpp
    src/message.cpp
    src/prober.cpp
    src/provider.cpp
    src/query.cpp
    src/record.cpp
    src/resolver.cpp
    src/server.cpp
    src/service.cpp
)

if(WIN32)
    configure_file(resource.rc.in "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
    set(SRC ${SRC} "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
endif()

add_library(qmdnsengine SHARED ${HEADERS} ${SRC})

set_target_properties(qmdnsengine PROPERTIES
    CXX_STANDARD  11
    DEFINE_SYMBOL QT_NO_SIGNALS_SLOTS_KEYWORDS
    DEFINE_SYMBOL QMDNSENGINE_LIBRARY
    PUBLIC_HEADER "${HEADERS}"
    VERSION       ${PROJECT_VERSION}
    SOVERSION     ${PROJECT_VERSION_MAJOR}
)

target_include_directories(qmdnsengine PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>"
)

target_link_libraries(qmdnsengine Qt5::Network)

install(TARGETS qmdnsengine
    EXPORT        qmdnsengine-export
    RUNTIME       DESTINATION "${BIN_INSTALL_DIR}"
    LIBRARY       DESTINATION "${LIB_INSTALL_DIR}"
    ARCHIVE       DESTINATION "${LIB_INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INCLUDE_INSTALL_DIR}/qmdnsengine"
)

install(EXPORT qmdnsengine-export
    FILE        qmdnsengineConfig.cmake
    DESTINATION "${LIB_INSTALL_DIR}/cmake/qmdnsengine"
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/qmdnsengineConfigVersion.cmake"
    VERSION       ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qmdnsengineConfigVersion.cmake"
    DESTINATION "${LIB_INSTALL_DIR}/cmake/qmdnsengine"
)
