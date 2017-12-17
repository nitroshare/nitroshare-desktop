configure_file(lan.json.in "${CMAKE_CURRENT_BINARY_DIR}/lan.json")
configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")

set(SRC
    lanplugin.h
    lanplugin.cpp
    lantransport.h
    lantransport.cpp
    lantransportserver.h
    lantransportserver.cpp
    server.h
    server.cpp
)

add_library(lan MODULE ${SRC})

set_target_properties(lan PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(lan PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(lan nitroshare Qt5::Network)

install(TARGETS lan
    DESTINATION "${INSTALL_PLUGIN_PATH}"
)

# If the OpenSSL DLLs live in Qt's bin dir, copy them to the runtime outout
# directory after building the plugin
if(WIN32)
    get_target_property(_qmake_executable Qt5::qmake IMPORTED_LOCATION)
    get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
    find_program(LIBEAY32 NAME libeay32.dll HINTS "${_qt_bin_dir}")
    find_program(SSLEAY32 NAME ssleay32.dll HINTS "${_qt_bin_dir}")
    if(LIBEAY32 AND SSLEAY32)
        add_custom_command(TARGET lan POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E
                copy_if_different "${LIBEAY32}" \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\"
            COMMAND "${CMAKE_COMMAND}" -E
                copy_if_different "${SSLEAY32}" \"${CMAKE_RUNTIME_OUTPUT_DIRECTORY}\"
            COMMENT "Copying OpenSSL libraries..."
        )
    endif()
endif()
