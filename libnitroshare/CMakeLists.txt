# Determine the relative path from the executable to the plugins
file(RELATIVE_PATH RELATIVE_PLUGIN_PATH /${CMAKE_RUNTIME_OUTPUT_DIRECTORY} /${PLUGIN_OUTPUT_DIRECTORY})

configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/nitroshare/config.h")

file(GLOB HEADERS include/nitroshare/*.h)
set(HEADERS "${HEADERS}" "${CMAKE_CURRENT_BINARY_DIR}/nitroshare/config.h")

set(SRC
    src/action/actionregistry_p.h
    src/action/actionregistry.cpp
    src/application/application_p.h
    src/application/application.cpp
    src/bundle/bundle_p.h
    src/bundle/bundle.cpp
    src/bundle/item.cpp
    src/device/device_p.h
    src/device/device.cpp
    src/device/deviceenumerator.cpp
    src/device/devicemodel_p.h
    src/device/devicemodel.cpp
    src/handler/handlerregistry_p.h
    src/handler/handlerregistry.cpp
    src/log/logger_p.h
    src/log/logger.cpp
    src/log/message_p.h
    src/log/message.cpp
    src/log/stderrwriter_p.h
    src/log/stderrwriter.cpp
    src/plugin/plugin_p.h
    src/plugin/plugin.cpp
    src/plugin/pluginmodel_p.h
    src/plugin/pluginmodel.cpp
    src/settings/category_p.h
    src/settings/category.cpp
    src/settings/setting_p.h
    src/settings/setting.cpp
    src/settings/settingsregistry_p.h
    src/settings/settingsregistry.cpp
    src/transfer/packet_p.h
    src/transfer/packet.cpp
    src/transfer/transfer_p.h
    src/transfer/transfer.cpp
    src/transfer/transfermodel_p.h
    src/transfer/transfermodel.cpp
    src/transport/transportserverregistry_p.h
    src/transport/transportserverregistry.cpp
    src/util/apiutil.cpp
    src/util/fileutil.cpp
    src/util/jsonutil.cpp
    src/util/proxymodel.cpp
    src/util/signalnotifier_p.h
    src/util/signalnotifier.cpp
)

add_library(nitroshare SHARED ${HEADERS} ${SRC})

set_target_properties(nitroshare PROPERTIES
    CXX_STANDARD  11
    DEFINE_SYMBOL QT_NO_SIGNALS_SLOTS_KEYWORDS
    DEFINE_SYMBOL NITROSHARE_LIBRARY
    PUBLIC_HEADER "${HEADERS}"
    VERSION       ${PROJECT_VERSION}
    SOVERSION     ${PROJECT_VERSION_MAJOR}
)

target_include_directories(nitroshare PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>"
    "$<INSTALL_INTERFACE:include>"
)

target_link_libraries(nitroshare Qt5::Network)

install(TARGETS nitroshare
    EXPORT        nitroshare-export
    RUNTIME       DESTINATION "${INSTALL_BIN_PATH}"
    LIBRARY       DESTINATION "${INSTALL_LIB_PATH}"
    ARCHIVE       DESTINATION "${INSTALL_LIB_PATH}"
    PUBLIC_HEADER DESTINATION include/nitroshare
)

install(EXPORT nitroshare-export
    FILE        nitroshareConfig.cmake
    DESTINATION "${INSTALL_LIB_PATH}/cmake/nitroshare"
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/nitroshareConfigVersion.cmake"
    VERSION       ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/nitroshareConfigVersion.cmake"
    DESTINATION "${INSTALL_LIB_PATH}/cmake/nitroshare"
)

if(WIN32)
    # Run windeployqt on the library
    include(DeployQt)
    windeployqt(nitroshare)

    # If the exe target exists, add the library as a dependency
    if(INNOSETUP_EXECUTABLE)
        add_dependencies(exe nitroshare)
    endif()
endif()

if(APPLE)
    include(DeployQt)
    macdeployqt(nitroshare)
endif()

if(BUILD_DOC)
    add_subdirectory(doc)
endif()

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()
