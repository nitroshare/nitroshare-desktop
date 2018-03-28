configure_file(qhttpengine_export.h.in "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine_export.h")

set(HEADERS
    include/qhttpengine/basicauthmiddleware.h
    include/qhttpengine/filesystemhandler.h
    include/qhttpengine/handler.h
    include/qhttpengine/ibytearray.h
    include/qhttpengine/localauthmiddleware.h
    include/qhttpengine/localfile.h
    include/qhttpengine/middleware.h
    include/qhttpengine/parser.h
    include/qhttpengine/proxyhandler.h
    include/qhttpengine/qiodevicecopier.h
    include/qhttpengine/qobjecthandler.h
    include/qhttpengine/range.h
    include/qhttpengine/server.h
    include/qhttpengine/socket.h
    "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine_export.h"
)

set(SRC
    src/filesystemhandler.cpp
    src/basicauthmiddleware.cpp
    src/handler.cpp
    src/parser.cpp
    src/range.cpp
    src/server.cpp
    src/socket.cpp
    src/qiodevicecopier.cpp
    src/localauthmiddleware.cpp
    src/localfile.cpp
    src/qobjecthandler.cpp
    src/proxyhandler.cpp
    src/proxysocket.cpp
)

if(WIN32)
    configure_file(resource.rc.in "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
    set(SRC ${SRC} "${CMAKE_CURRENT_BINARY_DIR}/resource.rc")
endif()

add_library(qhttpengine SHARED ${HEADERS} ${SRC})

set_target_properties(qhttpengine PROPERTIES
    CXX_STANDARD          11
    CXX_STANDARD_REQUIRED ON
    DEFINE_SYMBOL         QT_NO_SIGNALS_SLOTS_KEYWORDS
    DEFINE_SYMBOL         QHTTPENGINE_LIBRARY
    PUBLIC_HEADER         "${HEADERS}"
    VERSION               ${PROJECT_VERSION}
    SOVERSION             ${PROJECT_VERSION_MAJOR}
)

target_include_directories(qhttpengine PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>"
)

target_link_libraries(qhttpengine Qt5::Network)

install(TARGETS qhttpengine EXPORT qhttpengine-export
    RUNTIME DESTINATION "${BIN_INSTALL_DIR}"
    LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
    ARCHIVE DESTINATION "${LIB_INSTALL_DIR}"
    PUBLIC_HEADER DESTINATION "${INCLUDE_INSTALL_DIR}/qhttpengine"
)

install(EXPORT qhttpengine-export
    FILE        qhttpengineConfig.cmake
    DESTINATION "${LIB_INSTALL_DIR}/cmake/qhttpengine"
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/qhttpengineConfigVersion.cmake"
    VERSION       ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qhttpengineConfigVersion.cmake"
    DESTINATION "${LIB_INSTALL_DIR}/cmake/qhttpengine"
)

configure_file(qhttpengine.pc.in "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine.pc" @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/qhttpengine.pc"
    DESTINATION "${LIB_INSTALL_DIR}/pkgconfig"
)
