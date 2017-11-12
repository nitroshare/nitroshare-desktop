configure_file(config.h.in "${CMAKE_CURRENT_BINARY_DIR}/config.h")

set(SRC
    mockdevice.h
    mockdevice.cpp
    mockhandler.h
    mockhandler.cpp
    mockitem.h
    mockitem.cpp
    mocktransport.h
    mocktransport.cpp
    mocktransportserver.h
    mocktransportserver.cpp
)

add_library(mock SHARED ${SRC})

set_target_properties(mock PROPERTIES
    CXX_STANDARD             11
    DEFINE_SYMBOL            MOCK_LIBRARY
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/.."
)

target_include_directories(mock PUBLIC "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(mock nitroshare)
