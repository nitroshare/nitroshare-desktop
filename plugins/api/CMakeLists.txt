configure_file(api.json.in "${CMAKE_CURRENT_BINARY_DIR}/api.json")

set(SRC
    actionhandler.h
    actionhandler.cpp
    actionsaction.h
    actionsaction.cpp
    apiplugin.h
    apiplugin.cpp
    apiserver.h
    apiserver.cpp
    quitaction.h
    quitaction.cpp
    resource.qrc
    versionaction.h
    versionaction.cpp
)

add_library(api MODULE ${SRC})

set_target_properties(api PROPERTIES
    CXX_STANDARD             11
    VERSION                  ${VERSION}
    SOVERSION                ${VERSION_MAJOR}
    RUNTIME_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
    LIBRARY_OUTPUT_DIRECTORY "${PLUGIN_OUTPUT_DIRECTORY}"
)

target_include_directories(api PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(api nitroshare qhttpengine)

install(TARGETS api
    LIBRARY DESTINATION "${INSTALL_PLUGIN_PATH}"
)

if(APPLE)
    include(SharedLibrary)
    copy_lib_mac(qhttpengine api)
endif()
